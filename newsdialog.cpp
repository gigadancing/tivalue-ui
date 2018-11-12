#include "newsdialog.h"
#include "ui_newsdialog.h"
#include "tic.h"
#include "commondialog.h"
#include <QDebug>

NewsDialog::NewsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewsDialog)
{
    ui->setupUi(this);


//    Fry::getInstance()->appendCurrentDialogVector(this);
    setParent(Fry::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");

    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/recordId");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    foreach (QString key, keys)
    {
        int type = Fry::getInstance()->configFile->value(key).toInt();

        if( type == 1 || type == 2)
        {
            recordIdTypeMap.insert(key,type);
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();

    page = 1;
    totalNum = recordIdTypeMap.size();
    ui->numLabel->setText( tr("News") + QString("(1/%0)").arg( totalNum));

    if( totalNum == 0)
    {
        qDebug() << "No record!";
        return;
    }
    else if( totalNum == 1)
    {
        ui->nextBtn->setText( tr("Close"));
    }

    showNews( recordIdTypeMap.keys().at(page - 1));

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(pic2/close3.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->checkBtn->setStyleSheet("background:transparent;color: rgb(161, 160, 156);");
    ui->nextBtn->setStyleSheet("QToolButton{background-color:#469cfc;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:3px;}"
                               "QToolButton:hover{background-color:#62a9f8;}");
}

NewsDialog::~NewsDialog()
{
    delete ui;
//    Fry::getInstance()->removeCurrentDialogVector(this);
}

void NewsDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void NewsDialog::on_nextBtn_clicked()
{
    page ++;

    if( page > totalNum)
    {
        close();
//        emit accepted();
        return;
    }
    else if( page == totalNum)
    {
        ui->nextBtn->setText( tr("Close"));
    }

    ui->numLabel->setText( tr("News") + QString("(%0/%1)").arg(page).arg( totalNum));
    showNews( recordIdTypeMap.keys().at(page - 1));
}

void NewsDialog::on_closeBtn_clicked()
{
    if( page < totalNum)
    {
        CommonDialog commonDialog(CommonDialog::YesOrNo);
        commonDialog.setText( tr("Ignore all news?") );

        if( commonDialog.pop() )
        {
            mutexForConfigFile.lock();

            int size = recordIdTypeMap.keys().size();
            for( int i = page; i < size; i++)
            {
                Fry::getInstance()->configFile->remove( "recordId/" + recordIdTypeMap.keys().at( i));
            }
            mutexForConfigFile.unlock();

            close();
    //        emit accepted();
        }
        else
        {
            close();
    //        emit accepted();
        }

    }
    else
    {
        close();
//        emit accepted();
    }



}

void NewsDialog::showNews(QString id)
{
    // 阅读了该消息后 将 recordId 从config中删除
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->remove( "recordId/" + id);
    mutexForConfigFile.unlock();

    if( recordIdTypeMap.value(id) == 1)
    {
        // 如果是成功交易
        QString result = Fry::getInstance()->jsonDataValue("id_blockchain_get_transaction_" + id);
        int pos = result.indexOf("{\"from_account\":\"") + 17;
        QString fromAccount = result.mid( pos, result.indexOf( "\"", pos) - pos);
        ui->fromLabel->setText( fromAccount);

        pos = result.indexOf( ",\"to_account\":\"") + 15;
        QString toAddress = result.mid( pos, result.indexOf( "\"", pos) - pos);
        ui->toLabel->setText( toAddress);

        pos = result.indexOf( ",\"amount\":{\"amount\":") + 20;
        QString amount = result.mid( pos, result.indexOf( ",", pos) - pos);
        amount.remove("\"");
        ui->amountLabel->setText( doubleTo2Decimals( amount.toDouble() / 100000) + " TV" );

        ui->newsTypeLabel->setText( QString::fromLocal8Bit("交易已被系统确认"));
        ui->checkBtn->show();
    }
    else if(recordIdTypeMap.value(id) == 2)
    {
        // 如果是失败交易
        QString info = Fry::getInstance()->getPendingInfo(id);
        if( !info.isEmpty())
        {
            QStringList strList = info.split(",");
            ui->fromLabel->setText( strList.at(1));
            ui->toLabel->setText( strList.at(2));
            ui->amountLabel->setText( strList.at(3));
            ui->newsTypeLabel->setText( QString::fromLocal8Bit("交易已失效"));
            ui->checkBtn->hide();
        }

    }

}

void NewsDialog::on_checkBtn_clicked()
{
    QString fromAccount = ui->fromLabel->text();

    if( fromAccount.mid(0,2) == ASSET_NAME)
    {
        QString address = ui->fromLabel->text();
        fromAccount = "";

        mutexForAddressMap.lock();
        for (QMap<QString,QString>::const_iterator i = Fry::getInstance()->addressMap.constBegin(); i != Fry::getInstance()->addressMap.constEnd(); ++i)
        {
            if( i.value() == address)
            {
                fromAccount = i.key();
                break;
            }

        }
        mutexForAddressMap.unlock();

    }

    emit showAccountPage(fromAccount);
    close();
//    emit accepted();
}
