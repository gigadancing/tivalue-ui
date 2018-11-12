#include <QDebug>
#include <QPainter>
#include <QTextCodec>
#include <QDir>
#include <windows.h>
#include <QMessageBox>
#include "transferpage.h"
#include "ui_transferpage.h"
#include "tic.h"
#include "debug_log.h"
#include "tic_common_define.h"
#include "contactdialog.h"
#include "remarkdialog.h"
#include "commondialog.h"
#include "transferconfirmdialog.h"
#include "rpcthread.h"

const double transfer_rate = 0.002;

TransferPage::TransferPage(QString name,QWidget *parent) :
    QWidget(parent),
    accountName(name),
    inited(false),
    ui(new Ui::TransferPage)
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);
    connect(Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(22,24,28));
    setPalette(palette);


    ui->addContactBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/contactBtn2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    // 如果是点击账单跳转
    if (accountName.isEmpty()) {
        if (Fry::getInstance()->addressMap.size() > 0) {
            accountName = Fry::getInstance()->addressMap.keys().at(0);
        } else { //如果还没有账户
            emit back();    // 跳转在functionbar  这里并没有用
            return;
        }
    }

    // 账户下拉框按字母顺序排序
    QStringList keys = Fry::getInstance()->addressMap.keys();
    ui->accountComboBox->addItems(keys);
    if (accountName.isEmpty()) {
        ui->accountComboBox->setCurrentIndex(0);
    } else {
        ui->accountComboBox->setCurrentText(accountName);
    }

    QString showName = Fry::getInstance()->addressMapValue(accountName);
    ui->addressLabel->setText(showName);

    ui->amountLineEdit->setStyleSheet("color:rgb(102,102,102);border:none;background:rgb(37,39,43);");
    ui->amountLineEdit->setTextMargins(8,0,0,0);
    QRegExp rx1("^([0]|[1-9][0-9]{0,10})(?:\\.\\d{1,2})?$|(^\\t?$)");
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->sendtoLineEdit->setStyleSheet("color:rgb(102,102,102);border:none;background:rgb(37,39,43);");
    ui->sendtoLineEdit->setTextMargins(8,0,0,0);
    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->sendtoLineEdit->setValidator(validator);
    ui->sendtoLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->feeLineEdit->setStyleSheet("color:rgb(102,102,102);border:none;background:rgb(37,39,43);");
    ui->feeLineEdit->setTextMargins(8,0,0,0);
    ui->feeLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->feeLineEdit->setEnabled(false);

    QRegExp rx("^([0]|[1-9][0-9]{0,5})(?:\\.\\d{1,2})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->feeLineEdit->setValidator(pReg);

    ui->messageLineEdit->setStyleSheet("color:rgb(102,102,102);border:none;background:rgb(37,39,43);");
    ui->messageLineEdit->setTextMargins(8,0,0,0);
    ui->tipLabel3->hide();
    ui->tipLabel4->hide();
    ui->tipLabel6->hide();
    on_amountLineEdit_textChanged(ui->amountLineEdit->text());
    ui->accountComboBox->setStyleSheet("QComboBox {background:rgb(37,39,43);color:rgb(190,190,190);border:none;padding: 1px 2px 1px 8px;min-width: 9em;}"
                                           "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 26px;"
                                                                  "border-left-width: 1px;border-left-color: rgb(37,39,43);border-left-style: solid;"
                                                                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                           "QComboBox::down-arrow {image: url(:/pic/ticpic/comboBoxArrow.png);}"
                                           );
    ui->assetComboBox->addItem("TV");
    ui->assetComboBox->addItem("ACS");
    ui->assetComboBox->addItem("3DC");
    ui->assetComboBox->addItem("NBJ");
    ui->assetComboBox->addItem("NOT");
    ui->assetComboBox->setCurrentIndex(0);
    ui->assetComboBox->setStyleSheet("QComboBox {background:rgb(37,39,43);color:rgb(190,190,190);border:none;padding: 1px 2px 1px 8px;min-width: 9em;}"
                                     "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 26px;"
                                                            "border-left-width: 1px;border-left-color: rgb(37,39,43);border-left-style: solid;"
                                                            "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                     "QComboBox::down-arrow {image: url(:/pic/ticpic/comboBoxArrow.png);}");
    connect(ui->assetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAssetComboChanged(int)));

    getContactsList();
    inited = true;
	DLOG_QT_WALLET_FUNCTION_END;
}

TransferPage::~TransferPage()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
	DLOG_QT_WALLET_FUNCTION_END;
}

void TransferPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;
    if (!inited)  return;
    accountName = arg1;
    QString balance = Fry::getInstance()->balanceMapValue(accountName);
    balance = balance.mid(0, balance.indexOf(' '));
    balance.remove(',');
    //ui->balanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + balance + "</font><font style=\"font-size:12px\" color=#a9a9a9> TV</font></body>" );
    //ui->balanceLabel->adjustSize();
    QString showName = Fry::getInstance()->addressMapValue(accountName);
    ui->addressLabel->setText(showName);
    on_amountLineEdit_textChanged(ui->amountLineEdit->text());
	DLOG_QT_WALLET_FUNCTION_END;
}

void TransferPage::on_sendBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    //check address
    QString addr = ui->sendtoLineEdit->text();
    Fry::getInstance()->postRPC(toJsonFormat("id_wallet_check_address_" + addr, "wallet_check_address", QStringList() << addr));
    QString res = "";
    do {
      res = Fry::getInstance()->jsonDataValue("id_wallet_check_address_" + addr);
    } while(res.length() == 0);
    QStringList lst = res.split(":");
    if (lst[1]!= "true") {
      CommonDialog tipDialog(CommonDialog::OkOnly);
      tipDialog.setText(tr("Invalid address"));
      tipDialog.pop();
      return;
    }

    if (ui->amountLineEdit->text().size() == 0 || ui->sendtoLineEdit->text().size() == 0) {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText(tr("Please enter the amount and address."));
        tipDialog.pop();
        return;
    }
    if (ui->amountLineEdit->text().toDouble() > -0.00001 && ui->amountLineEdit->text().toDouble() < 0.00001) {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText(tr("The amount can not be 0"));
        tipDialog.pop();
        return;
    }
    if (ui->feeLineEdit->text().toDouble() > -0.00001 && ui->feeLineEdit->text().toDouble() < 0.00001) {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText( tr("The fee can not be 0"));
        tipDialog.pop();
        return;
    }
    QString remark = ui->messageLineEdit->text();
    //转地址如果没有备注 会自动添加 TO TVDD...   所以添加空格
    if (remark.size() == 0) {
        remark = " ";
    }

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(remark);
    if (ba.size() > 40) {
        CommonDialog tipDialog(CommonDialog::OkOnly);
        tipDialog.setText(tr("Message length more than 40 bytes!"));
        tipDialog.pop();
        return;
    }

    TransferConfirmDialog transferConfirmDialog(ui->sendtoLineEdit->text(), ui->amountLineEdit->text(), ui->feeLineEdit->text(), remark, this);
    bool yOrN = transferConfirmDialog.pop();
    if (yOrN)
    {
//        QString str = "wallet_set_transaction_fee " + ui->feeLineEdit->text() + '\n';
//        Fry::getInstance()->write(str);
//        QString result = Fry::getInstance()->read();
        if (!ui->sendtoLineEdit->text().isEmpty())
        {
            if (ui->assetComboBox->currentText() == "TV")
            {
                //TV
                Fry::getInstance()->postRPC(toJsonFormat("id_wallet_transfer_to_address_" + accountName, "wallet_transfer_to_address",
                QStringList() << ui->amountLineEdit->text() << ASSET_NAME << accountName << ui->sendtoLineEdit->text() << remark ));
            }
            else if (ui->assetComboBox->currentText() == "ACS")
            {
                //ACS
                double amount = ui->amountLineEdit->text().toDouble() * 100000;
                QString tmp = ui->sendtoLineEdit->text() + "," + QString::number(amount) + "," + remark;
                Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_" + accountName, "call_contract",
                QStringList() << "CONJiK5jcsBhHVDZWjGTtMek2q9h42JnQtvj" << accountName << "transfer" << tmp << "TV" << "1" ));
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText(tr("Transaction has been sent,please wait for confirmation"));
                tipDialog.pop();
            }
            else if (ui->assetComboBox->currentText() == "3DC")
            {
                //3DC
                double amount = ui->amountLineEdit->text().toDouble() * 100000;
                QString tmp = ui->sendtoLineEdit->text() + "," + QString::number(amount) + "," + remark;
                Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_" + accountName, "call_contract",
                QStringList() << "CONGWG5DwjXC9aFokPMB39tpaWHyzTNLcBWo" << accountName << "transfer" << tmp << "TV" << "1" ));
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText(tr("Transaction has been sent,please wait for confirmation"));
                tipDialog.pop();
            }
            else if (ui->assetComboBox->currentText() == "NBJ")
            {
                // NBJ
                double amount = ui->amountLineEdit->text().toDouble() * 100000;
                QString tmp = ui->sendtoLineEdit->text() + "," + QString::number(amount) + "," + remark;
                Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_" + accountName, "call_contract",
                QStringList() << "CONMnbfb1uEhPMT11UjLHZjB7WRmNTB6dvTJ" << accountName << "transfer" << tmp << "TV" << "1" ));
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText(tr("Transaction has been sent,please wait for confirmation"));
                tipDialog.pop();
            }
            else if (ui->assetComboBox->currentText() == "NOT")
            {
                // NOT
                double amount = ui->amountLineEdit->text().toDouble() * 10000;
                QString tmp = ui->sendtoLineEdit->text() + "," + QString::number(amount) + "," + remark;
                Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_" + accountName, "call_contract",
                QStringList() << "CONJD5RCLyYYnbwV3hxhneDBR8FWaL9JqcQw" << accountName << "transfer" << tmp << "TV" << "1" ));
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText(tr("Transaction has been sent,please wait for confirmation"));
                tipDialog.pop();
            }
        }
    }
	DLOG_QT_WALLET_FUNCTION_END;
}

QComboBox* TransferPage::getAssetCombo() {
    return ui->assetComboBox;
}

void TransferPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(32, 34, 38), Qt::SolidLine));
    painter.setBrush(QBrush(QColor(22, 24, 28), Qt::SolidPattern));
    painter.drawRect(-1, -1, 794, 114);
}

void TransferPage::on_amountLineEdit_textChanged(const QString &arg1)
{
    double amount = ui->amountLineEdit->text().toDouble();
    double fee = 0.01;
    if (ui->assetComboBox->currentText() == "TV")
    {
        fee = max(amount * transfer_rate, 0.01);
    }

    ui->feeLineEdit->setText(QString::number(fee, 'f', 2));
    if (ui->assetComboBox->currentText() == "TV")
    {
      QString strBalanceTemp = Fry::getInstance()->balanceMapValue(accountName).remove(",");
      strBalanceTemp = strBalanceTemp.remove(" TV");
      double dBalance = strBalanceTemp.remove(",").toDouble();

      if (amount + fee > dBalance + 0.000001)
      {
          ui->tipLabel3->show();
          ui->sendBtn->setEnabled(false);
      }
      else
      {
          ui->tipLabel3->hide();
          ui->sendBtn->setEnabled(true);
      }
    }
    else if(ui->assetComboBox->currentText() == "ACS")
    {
      QString addr = Fry::getInstance()->addressMapValue(accountName);
      Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
          "call_contract_offline", QStringList() << "CONJiK5jcsBhHVDZWjGTtMek2q9h42JnQtvj" << accountName << "balanceOf" << addr << "TV" << "1"));
      QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
      QString balance_str = tmp.last();
      QString ban = balance_str.mid(1, balance_str.length() - 2);
      double ban_d = ban.toDouble() * 0.00001;
      if (amount > ban_d)
      {
          ui->tipLabel3->show();
          ui->sendBtn->setEnabled(false);
      }
      else
      {
          ui->tipLabel3->hide();
          ui->sendBtn->setEnabled(true);
      }
    }
    else if(ui->assetComboBox->currentText() == "3DC")
    {
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONGWG5DwjXC9aFokPMB39tpaWHyzTNLcBWo" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double ban_d = ban.toDouble() * 0.00001;
        if (amount > ban_d)
        {
            ui->tipLabel3->show();
            ui->sendBtn->setEnabled(false);
        }
        else
        {
            ui->tipLabel3->hide();
            ui->sendBtn->setEnabled(true);
        }
    }
    else if(ui->assetComboBox->currentText() == "NBJ")
    {
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONMnbfb1uEhPMT11UjLHZjB7WRmNTB6dvTJ" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double ban_d = ban.toDouble() * 0.00001;
        if (amount > ban_d)
        {
            ui->tipLabel3->show();
            ui->sendBtn->setEnabled(false);
        }
        else
        {
            ui->tipLabel3->hide();
            ui->sendBtn->setEnabled(true);
        }
    }
    else if(ui->assetComboBox->currentText() == "NOT")
    {
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONJD5RCLyYYnbwV3hxhneDBR8FWaL9JqcQw" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double ban_d = ban.toDouble() * 0.0001;
        if (amount > ban_d)
        {
            ui->tipLabel3->show();
            ui->sendBtn->setEnabled(false);
        }
        else
        {
            ui->tipLabel3->hide();
            ui->sendBtn->setEnabled(true);
        }
    }
}

void TransferPage::on_sendtoLineEdit_textChanged(const QString &arg1)
{
    if (ui->sendtoLineEdit->text().contains(" ") || ui->sendtoLineEdit->text().contains("\n")) { // 不判断就remove的话 右键菜单撤销看起来等于不能用
        ui->sendtoLineEdit->setText( ui->sendtoLineEdit->text().simplified().remove(" "));
    }

    if (ui->sendtoLineEdit->text().isEmpty() || ui->sendtoLineEdit->text().mid(0,2) == ASSET_NAME) {
        ui->tipLabel4->hide();
        return;
    }

    if (ui->sendtoLineEdit->text().toInt() == 0) { // 不能是纯数字
        Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_account_" + ui->sendtoLineEdit->text(), "blockchain_get_account", QStringList() << ui->sendtoLineEdit->text() ));
    } else {
        ui->tipLabel4->setText(tr("Invalid add."));
        ui->tipLabel4->show();
    }

}

void TransferPage::refresh()
{
    qDebug() << "transferPage: refresh begin";
    if (ui->assetComboBox->currentText() == "TV")
    {
        ui->tipLabel6_2->show();
        QString balance = Fry::getInstance()->balanceMapValue(accountName);
        balance = balance.mid(0, balance.indexOf(' '));
        balance.remove(',');
        ui->balanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + balance + "</font><font style=\"font-size:12px\" color=#a9a9a9> TV</font></body>");

        double amount = ui->amountLineEdit->text().toDouble();
        double fee = amount * transfer_rate;
        ui->feeLineEdit->setText(QString::number(fee, 10, 2));
        ui->balanceLabel->adjustSize();
    }
    else if (ui->assetComboBox->currentText() == "ACS")
    {
        ui->tipLabel6_2->hide();
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONJiK5jcsBhHVDZWjGTtMek2q9h42JnQtvj" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        ui->feeLineEdit->setText(QString("0.01"));
        ui->balanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(ban.toDouble() * 0.00001) + "</font><font style=\"font-size:12px\" color=#a9a9a9> ACS</font></body>");
        ui->balanceLabel->adjustSize();
    }
    else if (ui->assetComboBox->currentText() == "3DC")
    {
        ui->tipLabel6_2->hide();
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONGWG5DwjXC9aFokPMB39tpaWHyzTNLcBWo" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        ui->balanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(ban.toDouble() * 0.00001) + "</font><font style=\"font-size:12px\" color=#a9a9a9> 3DC</font></body>");
        ui->feeLineEdit->setText(QString("0.01"));
        ui->balanceLabel->adjustSize();
    }
    else if (ui->assetComboBox->currentText() == "NBJ")
    {
        ui->tipLabel6_2->hide();
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONMnbfb1uEhPMT11UjLHZjB7WRmNTB6dvTJ" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        ui->balanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(ban.toDouble() * 0.00001) + "</font><font style=\"font-size:12px\" color=#a9a9a9> NBJ</font></body>");
        ui->feeLineEdit->setText(QString("0.01"));
        ui->balanceLabel->adjustSize();
    }
    else if (ui->assetComboBox->currentText() == "NOT")
    {
        ui->tipLabel6_2->hide();
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName,
            "call_contract_offline", QStringList() << "CONJD5RCLyYYnbwV3hxhneDBR8FWaL9JqcQw" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");
        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        ui->balanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(ban.toDouble() * 0.00001) + "</font><font style=\"font-size:12px\" color=#a9a9a9> NOT</font></body>");
        ui->feeLineEdit->setText(QString("0.01"));
        ui->balanceLabel->adjustSize();
    }
    qDebug() << "transferPage: refresh end";
}

void TransferPage::on_addContactBtn_clicked()
{
    ContactDialog contactDialog(this);
    connect(&contactDialog,SIGNAL(contactSelected(QString)), this, SLOT(contactSelected(QString)));
    contactDialog.move( ui->addContactBtn->mapToGlobal( QPoint(0,0)));
    contactDialog.exec();
}

void TransferPage::contactSelected(QString contact)
{
    ui->sendtoLineEdit->setText(contact);
}


void TransferPage::getContactsList()
{
    if( !Fry::getInstance()->contactsFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "contact.dat not exist";
        contactsList.clear();
        return;
    }
    QString str = QByteArray::fromBase64( Fry::getInstance()->contactsFile->readAll());
    QStringList strList = str.split(";");
    strList.removeLast();
    int size = strList.size();

    for( int i = 0; i < size; i++)
    {
        QString str2 = strList.at(i);
        contactsList += str2.left( str2.indexOf("="));
    }


    Fry::getInstance()->contactsFile->close();
}

QString TransferPage::getCurrentAccount()
{
    return accountName;
}

void TransferPage::setAddress(QString address)
{
    ui->sendtoLineEdit->setText(address);
}

void TransferPage::jsonDataUpdated(QString id)
{
    if (id == "id_wallet_transfer_to_address_" + accountName || id == "id_wallet_transfer_to_public_account_" + accountName) {
        QString result = Fry::getInstance()->jsonDataValue(id);
        qDebug() << id << result;
        if (result.mid(0, 18) == "\"result\":{\"index\":") { // 成功
            QString recordId = result.mid( result.indexOf("\"record_id\"") + 13, 40);
            mutexForPendingFile.lock();
            mutexForConfigFile.lock();
            Fry::getInstance()->configFile->setValue("/recordId/" + recordId , 0);
            mutexForConfigFile.unlock();

            if(!Fry::getInstance()->pendingFile->open(QIODevice::ReadWrite)) {
                qDebug() << "pending.dat open fail";
                return;
            }

            QByteArray ba = QByteArray::fromBase64(Fry::getInstance()->pendingFile->readAll());
            ba += QString(recordId + "," + accountName + "," + ui->sendtoLineEdit->text() + "," + ui->amountLineEdit->text() + "," + ui->feeLineEdit->text() + ";").toUtf8();
            ba = ba.toBase64();
            Fry::getInstance()->pendingFile->resize(0);
            QTextStream ts(Fry::getInstance()->pendingFile);
            ts << ba;

            Fry::getInstance()->pendingFile->close();

            mutexForPendingFile.unlock();

            CommonDialog tipDialog(CommonDialog::OkOnly);
            tipDialog.setText(tr("Transaction has been sent,please wait for confirmation"));
            tipDialog.pop();

            if (!contactsList.contains(ui->sendtoLineEdit->text())) {
                CommonDialog addContactDialog(CommonDialog::OkAndCancel);
                addContactDialog.setText(tr("Add this address to contacts?"));
                if (addContactDialog.pop()) {
                    RemarkDialog remarkDialog(ui->sendtoLineEdit->text());
                    remarkDialog.pop();
                    getContactsList();
                }
            }
            emit showAccountPage(accountName);
        } else {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);
            qDebug() << "errorMessage : " << errorMessage;

            if (errorMessage == "Assert Exception") {
                if (result.contains("\"format\":\"my->is_receive_account( from_account_name ): Invalid account name\",")) {
                    CommonDialog tipDialog(CommonDialog::OkOnly);
                    tipDialog.setText( tr("This name has been registered, please rename this account!"));
                    tipDialog.pop();
                } else {
                    CommonDialog tipDialog(CommonDialog::OkOnly);
                    tipDialog.setText( tr("Wrong address!"));
                    tipDialog.pop();
                }
            } else if (errorMessage == "imessage size bigger than soft_max_lenth") {
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr("Message too long!"));
                tipDialog.pop();
            } else if (errorMessage == "invalid transaction expiration") {
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr("Failed: You need to wait for synchronization to complete"));
                tipDialog.pop();
            } else if (errorMessage == "insufficient funds"){
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr("Not enough TVs!"));
                tipDialog.pop();
            } else if (errorMessage == "Out of Range") {
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr("Wrong address!"));
                tipDialog.pop();
            } else if (errorMessage == "Parse Error") {
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr("Wrong address!"));
                tipDialog.pop();
            } else {
                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr("Transaction sent failed"));
                tipDialog.pop();
            }
        }
        return;
    }

    if (id.mid(0, 26) == "id_blockchain_get_account_") {
        // 如果跟当前输入框中的内容不一样，则是过时的rpc返回，不用处理
        if (id.mid(26) != ui->sendtoLineEdit->text())  return;
        QString result = Fry::getInstance()->jsonDataValue(id);

        if (result != "\"result\":null") {
            ui->tipLabel4->setText(tr("<body><font color=green>Valid add.</font></body>"));
            ui->tipLabel4->show();
        } else {
            ui->tipLabel4->setText(tr("Invalid add."));
            ui->tipLabel4->show();
        }
        return;
    }
}

void TransferPage::on_feeLineEdit_textChanged(const QString &arg1)
{
    on_amountLineEdit_textChanged("");
}

void TransferPage::on_messageLineEdit_textChanged(const QString &arg1)
{
    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(arg1);
    if (ba.size() > 40) {
        ui->tipLabel6->show();
    } else {
        ui->tipLabel6->hide();
    }
}

void TransferPage::onAssetComboChanged(int index) {
    refresh();
}
