#include "applydelegatepage.h"
#include "ui_applydelegatepage.h"
#include "fry.h"
#include "commondialog.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "gop_common_define.h"

#include <QPainter>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>

ApplyDelegatePage::ApplyDelegatePage( QString name, QWidget *parent) :
    QWidget(parent),
    inited(false),
    ui(new Ui::ApplyDelegatePage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    setPalette(palette);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    mutexForRegisterMap.lock();
    for (QMap<QString,QString>::const_iterator i = Fry::getInstance()->registerMap.constBegin(); i != Fry::getInstance()->registerMap.constEnd(); ++i)
    {

        if( i.value() != "" && !Fry::getInstance()->delegateAccountList.contains( i.key()))
        {
            ui->accountComboBox->addItem( i.key());
        }
    }
    mutexForRegisterMap.unlock();

    if(  name.isEmpty())
    {
        ui->accountComboBox->setCurrentIndex(0);
        accountName = ui->accountComboBox->currentText();
    }
    else
    {
        ui->accountComboBox->setCurrentText(name);
        accountName = name;
    }

//    setStyleSheet("#ApplyDelegatePage{background-color: rgb(255, 255, 255);}");
    ui->accountComboBox->setStyleSheet("QComboBox {border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;min-width: 9em;}"
                                           "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                                                                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                                                                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                           "QComboBox::down-arrow {image: url(pic2/comboBoxArrow.png);}"
                                           );

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->addressNameLineEdit->setValidator( validator );
    ui->addressNameLineEdit->setPlaceholderText( tr("Set your account name"));
    ui->addressNameLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->addressNameLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->addressNameLineEdit->setTextMargins(8,0,0,0);

    ui->wrongLabel->hide();
    ui->wrongLabel2->hide();
    ui->correctLabel->hide();

    init();

    inited = true;
}

ApplyDelegatePage::~ApplyDelegatePage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}

void ApplyDelegatePage::init()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    // 如果是已注册账户
    if( Fry::getInstance()->registerMapValue(accountName) != "NO"
            && !Fry::getInstance()->registerMapValue(accountName).isEmpty())
    {
        ui->label_6->hide();
        ui->addressNameLineEdit->hide();
        ui->correctLabel->hide();
        ui->wrongLabel->hide();
        ui->wrongLabel2->hide();
        ui->tipLabel2->hide();
        ui->upgradeBtn->move(180,346);
    }
    else
    {
        ui->label_6->show();
        ui->addressNameLineEdit->show();
        ui->tipLabel2->show();
        ui->upgradeBtn->move(180,430);

        ui->addressNameLineEdit->setText( accountName);
    }

    balance = Fry::getInstance()->balanceMapValue(accountName);
    balance = balance.mid(0,balance.indexOf(' '));
    balance.remove(',');
    ui->balanceLabel->setText( balance + " GOP" );

    if( balance.toDouble() > 1221.83 - 0.0000001)
    {
        ui->upgradeBtn->setEnabled(true);
        ui->tipLabel->hide();
    }
    else
    {
        ui->upgradeBtn->setEnabled(false);
        ui->tipLabel->show();
    }

    ui->addressLabel->setText(Fry::getInstance()->addressMapValue(accountName));


    DLOG_QT_WALLET_FUNCTION_END;
}

void ApplyDelegatePage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(229,229,229));
    painter.setPen(QColor(177,177,177));
    painter.drawRect(QRect(-1,-1,828,92));
}


void ApplyDelegatePage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( !inited) return;

    accountName = arg1;
    init();
}

void ApplyDelegatePage::refresh()
{
    init();
}

void ApplyDelegatePage::retranslator()
{
    ui->retranslateUi(this);
}

void ApplyDelegatePage::jsonDataUpdated(QString id)
{
    if( id == "id_blockchain_get_account")
    {
        QString result = Fry::getInstance()->jsonDataValue(id);
        if( result == "\"result\":null")
        {
            ui->wrongLabel->hide();
            ui->wrongLabel2->hide();
            ui->correctLabel->show();

            if( balance.toDouble() >= 1221.83 - 0.0000001)
            {
                ui->upgradeBtn->setEnabled(true);
            }
        }
        else
        {
            ui->wrongLabel->show();
            ui->wrongLabel2->hide();
            ui->correctLabel->hide();
            ui->upgradeBtn->setEnabled(false);
        }

        return;
    }

    if( id.mid(0,25) == "id_wallet_account_rename_")
    {
        QString result = Fry::getInstance()->jsonDataValue(id);
        QString newName = id.mid(25);

        if( result == "\"result\":null")
        {
            mutexForConfigFile.lock();
            Fry::getInstance()->configFile->beginGroup("/accountInfo");
            QStringList accountNameKey = Fry::getInstance()->configFile->childKeys();
            for( int i = 0; i < accountNameKey.size(); i++)
            {
                if( Fry::getInstance()->configFile->value( accountNameKey.at(i)) == accountName)
                {
                    Fry::getInstance()->configFile->setValue( accountNameKey.at(i), newName);
                    break;
                }
            }
            Fry::getInstance()->configFile->endGroup();
            mutexForConfigFile.unlock();

            Fry::getInstance()->addressMapRemove( accountName);
            Fry::getInstance()->balanceMapRemove( accountName);
            Fry::getInstance()->registerMapRemove( accountName);

            accountName = newName;

            emit accountRenamed();

            Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_set_transaction_fee", "wallet_set_transaction_fee", QStringList() << "0.01" ));
        }
        else
        {
            emit hideShadowWidget();
            qDebug() << "rename " + accountName + " to " + newName + " failed :" + result;
        }
        return;
    }

    if( id == "id_wallet_set_transaction_fee")
    {
        QString result = Fry::getInstance()->jsonDataValue(id);

        if( result == "\"result\":{\"amount\":1000,\"asset_id\":0}")
        {
            if( Fry::getInstance()->registerMapValue(accountName) != "NO" && !Fry::getInstance()->registerMapValue(accountName).isEmpty() )
            {
                Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_account_update_registration", "wallet_account_update_registration", QStringList() << accountName << accountName << "[]" << "100" ));
            }
            else
            {
                 Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_account_register", "wallet_account_register", QStringList() << accountName << accountName << "" << "100" ));
            }


        }
        else
        {
            emit hideShadowWidget();
            qDebug() << "set transaction fee failed: " << result;
        }

        return;
    }

    if( id == "id_wallet_account_update_registration")
    {
        emit hideShadowWidget();
        QString result = Fry::getInstance()->jsonDataValue(id);

        if( result.mid(0,20) == "\"result\":{\"index\":0,")
        {
            // 记录申请代理的账户， 为了防止重复申请
            QString recordId = result.mid( result.indexOf("\"entry_id\"") + 12, 40);  // 记录申请代理的id 为了可以查询有没有被确认
            mutexForConfigFile.lock();
            Fry::getInstance()->configFile->setValue("applyingForDelegateAccount/" + accountName,recordId);         
            Fry::getInstance()->configFile->setValue("/recordId/" + recordId , 0);
            mutexForConfigFile.unlock();

            mutexForPendingFile.lock();
            if( !Fry::getInstance()->pendingFile->open(QIODevice::ReadWrite))
            {
                qDebug() << "pending.dat not exist";
                return;
            }

            QByteArray ba = QByteArray::fromBase64( Fry::getInstance()->pendingFile->readAll());
            ba += QString( recordId + "," + accountName + "," + accountName + "," + "0" + "," + "1221.83" + ";").toUtf8();
            ba = ba.toBase64();
            Fry::getInstance()->pendingFile->resize(0);
            QTextStream ts(Fry::getInstance()->pendingFile);
            ts << ba;

            Fry::getInstance()->pendingFile->close();
            mutexForPendingFile.unlock();


            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Application for a delegate has been submitted."));
            commonDialog.pop();

            emit back(accountName);
        }
        else
        {
            qDebug() << "wallet_account_update_registration failed: " << result;

            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "invalid transaction expiration")
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("You need to wait for synchronization to complete."));
                commonDialog.pop();
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("Upgrade failed!"));
                commonDialog.pop();
            }
        }

        return;
    }

    if( id == "id_wallet_account_register")
    {
        emit hideShadowWidget();
        QString result = Fry::getInstance()->jsonDataValue(id);

        if( result.mid(0,20) == "\"result\":{\"index\":0,")
        {   
            // 记录申请代理的账户， 为了防止重复申请
            QString recordId = result.mid( result.indexOf("\"entry_id\"") + 12, 40);  // 记录申请代理的id 为了可以查询有没有被确认
            mutexForConfigFile.lock();
            Fry::getInstance()->configFile->setValue("applyingForDelegateAccount/" + accountName,recordId);
            Fry::getInstance()->configFile->setValue("/recordId/" + recordId , 0);
            mutexForConfigFile.unlock();

            mutexForPendingFile.lock();
            if( !Fry::getInstance()->pendingFile->open(QIODevice::ReadWrite))
            {
                qDebug() << "pending.dat not exist";
                return;
            }

            QByteArray ba = QByteArray::fromBase64( Fry::getInstance()->pendingFile->readAll());
            ba += QString( recordId + "," + accountName + "," + accountName + "," + "0" + "," + "1221.83" + ";").toUtf8();
            ba = ba.toBase64();
            Fry::getInstance()->pendingFile->resize(0);
            QTextStream ts(Fry::getInstance()->pendingFile);
            ts << ba;

            Fry::getInstance()->pendingFile->close();
            mutexForPendingFile.unlock();

//            Fry::getInstance()->configFile->setValue("/recordId/" + recordId , 0);
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Application for a delegate has been submitted."));
            commonDialog.pop();

            emit back(accountName);
        }
        else
        {
            qDebug() << "wallet_account_register delegate failed: " << result;

            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "invalid transaction expiration")
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("You need to wait for synchronization to complete."));
                commonDialog.pop();
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("Upgrade failed!"));
                commonDialog.pop();
            }

        }

        return;
    }
}

void ApplyDelegatePage::on_upgradeBtn_clicked()
{
    // 如果是已注册账户
    if( Fry::getInstance()->registerMapValue(accountName) != "NO"
            && !Fry::getInstance()->registerMapValue(accountName).isEmpty())
    {
        Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_set_transaction_fee", "wallet_set_transaction_fee", QStringList() << "0.01" ));
        emit showShadowWidget();
    }
    else
    {
        //rename
        QString addrUpdateName = ui->addressNameLineEdit->text();
        if( accountName != addrUpdateName)      // 如果改了地址名
        {
            Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_account_rename_" + addrUpdateName, "wallet_account_rename", QStringList() << accountName << addrUpdateName ));
            emit showShadowWidget();
        }
        else
        {
            Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_set_transaction_fee", "wallet_set_transaction_fee", QStringList() << "0.01" ));
            emit showShadowWidget();
        }
    }


}

bool isExistInWallet(QString strName);

void ApplyDelegatePage::on_addressNameLineEdit_textChanged(const QString &arg1)
{
    if( arg1.isEmpty())
    {
        ui->correctLabel->hide();
        ui->wrongLabel->hide();
        ui->wrongLabel2->hide();
        ui->upgradeBtn->setEnabled(false);
        return;
    }

    QString addrName = arg1;

    if( ADDRNAME_MAX_LENGTH < addrName.size() )
    {

        ui->upgradeBtn->setEnabled(false);
        ui->correctLabel->hide();
        ui->wrongLabel->hide();
        ui->wrongLabel2->show();

        return;
    }

    //检查地址名是否在钱包内已经存在
    if( isExistInWallet(addrName) && addrName != accountName)
    {
        ui->wrongLabel->show();
        ui->wrongLabel2->hide();
        ui->correctLabel->hide();
        ui->upgradeBtn->setEnabled(false);
        return;
    }

    //检查地址名是否已经在链上注册了 blockchain_get_account(accountName)
    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_account", "blockchain_get_account", QStringList() << ui->addressNameLineEdit->text() ));

}
