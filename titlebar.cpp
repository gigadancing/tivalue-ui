#include <QDebug>

#include "titlebar.h"
#include "ui_titlebar.h"
#include "debug_log.h"
#include <QPainter>
#include "setdialog.h"
#include "consoledialog.h"
#include "tic.h"
#include "rpcthread.h"
#include "newsdialog.h"
#include "commondialog.h"

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    ui->setupUi(this);    

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background , QColor(33,33,36));
    setPalette(palette);

//    ui->transferBtn->setStyleSheet("QToolButton{background-image:url(pic2/transfer.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
//    ui->newsBtn2->setStyleSheet( "QToolButton{background-image:url(pic2/newsNum.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;color:white;}");
//    ui->newsBtn->setStyleSheet("QToolButton{background-image:url(pic2/news.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->newsBtn->hide();
    ui->newsBtn2->hide();

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/minimize.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/close.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->menuBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/set.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->lockBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/lockBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");


//    ui->divLineLabel->setPixmap(QPixmap("pic2/divLine.png"));
//    ui->divLineLabel->setScaledContents(true);

    ui->logoLabel->setPixmap(QPixmap(":/pic/ticpic/logo101x28.png"));

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->setInterval(10000);
    timer->start();

    onTimeOut();
	DLOG_QT_WALLET_FUNCTION_END;
}

TitleBar::~TitleBar()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    delete ui;

	DLOG_QT_WALLET_FUNCTION_END;
}

void TitleBar::on_minBtn_clicked()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( Fry::getInstance()->minimizeToTray)
    {
        emit tray();
    }
    else
    {  
        emit minimum();
    }

	DLOG_QT_WALLET_FUNCTION_END;
}

void TitleBar::on_closeBtn_clicked()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( Fry::getInstance()->closeToMinimize)
    {
        emit tray();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr( "Sure to close Fry Wallet?"));
        bool choice = commonDialog.pop();

        if( choice)
        {
            emit closeGOP();
        }
        else
        {
            return;
        }

    }

	DLOG_QT_WALLET_FUNCTION_END;
}

void TitleBar::on_menuBtn_clicked()
{
    SetDialog setDialog;
    connect(&setDialog,SIGNAL(settingSaved()),this,SLOT(saved()));
    setDialog.pop();

}

void TitleBar::saved()
{
    emit settingSaved();
}

void TitleBar::retranslator()
{
    ui->retranslateUi(this);
}

void TitleBar::jsonDataUpdated(QString id)
{
    if( id == "id_blockchain_list_pending_transactions")
    {
        QString pendingTransactions = Fry::getInstance()->jsonDataValue(id);
        // 查询一遍 config中记录的交易ID
        mutexForConfigFile.lock();
        Fry::getInstance()->configFile->beginGroup("/recordId");
        QStringList keys = Fry::getInstance()->configFile->childKeys();
        Fry::getInstance()->configFile->endGroup();

        int numOfNews = 0;
        foreach (QString key, keys)
        {
            if( Fry::getInstance()->configFile->value("/recordId/" + key).toInt() == 2)
            {
                // 失效的交易
                numOfNews++;
                continue;
            }

            if( !pendingTransactions.contains(key))  // 如果不在pending区, 看看是否在链上
            {
                Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_transaction_" + key, "blockchain_get_transaction", QStringList() << key  ));
            }

            if( Fry::getInstance()->configFile->value("/recordId/" + key).toInt() == 1)
            {
                numOfNews++;
            }
        }
        mutexForConfigFile.unlock();

//        if( numOfNews == 0)
//        {
//            ui->newsBtn->hide();
//            ui->newsBtn2->hide();
//            ui->newsBtn2->setText( QString::number( numOfNews));
//        }
//        else
//        {
//            ui->newsBtn->show();
//            ui->newsBtn2->setText( QString::number( numOfNews));
//            ui->newsBtn2->show();
//        }

        return;
    }

    if( id.startsWith("id_blockchain_get_transaction"))
    {

        QString result = Fry::getInstance()->jsonDataValue(id);

        if( result.mid(0,22).contains("exception") || result.mid(0,22).contains("error"))
        {
            // 若不在pending区也不在链上  则为失效交易  recordId置为2
            mutexForConfigFile.lock();

            Fry::getInstance()->configFile->setValue("/recordId/" + id.right(40), 2);

            mutexForConfigFile.unlock();

            return;
        }
        else   //  如果已经被打包进区块，则将config中记录置为1
        {
            mutexForConfigFile.lock();

            Fry::getInstance()->configFile->setValue("/recordId/" + id.right(40), 1);

            mutexForConfigFile.unlock();
        }

        return;
    }

}

void TitleBar::onTimeOut()
{
//    RpcThread* rpcThread = new RpcThread;
//    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//    rpcThread->setWriteData( toJsonFormat( "id_blockchain_list_pending_transactions", "blockchain_list_pending_transactions", QStringList() << "" ));
//    rpcThread->start();
    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_list_pending_transactions", "blockchain_list_pending_transactions", QStringList() << "" ));

    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/applyingForDelegateAccount");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    Fry::getInstance()->configFile->endGroup();
    foreach (QString key, keys)
    {
        // 如果申请代理的recordId 被删除了 或者被确认了（=1）或者失效了（=2） 则 删除applyingForDelegateAccount的记录
        if( !Fry::getInstance()->configFile->contains("/recordId/" + Fry::getInstance()->configFile->value("/applyingForDelegateAccount/" + key).toString())
            ||  Fry::getInstance()->configFile->value("/recordId/" + Fry::getInstance()->configFile->value("/applyingForDelegateAccount/" + key).toString()).toInt() != 0 )
        {
            Fry::getInstance()->configFile->remove("/applyingForDelegateAccount/" + key);
        }
    }

    Fry::getInstance()->configFile->beginGroup("/registeringAccount");
    keys = Fry::getInstance()->configFile->childKeys();
    Fry::getInstance()->configFile->endGroup();
    foreach (QString key, keys)
    {
        // 如果注册升级的recordId 被删除了 或者被确认了（=1）或者失效了（=2） 则 删除registeringAccount的记录
        if( !Fry::getInstance()->configFile->contains("/recordId/" + Fry::getInstance()->configFile->value("/registeringAccount/" + key).toString())
            ||  Fry::getInstance()->configFile->value("/recordId/" + Fry::getInstance()->configFile->value("/registeringAccount/" + key).toString()).toInt() != 0 )
        {
            Fry::getInstance()->configFile->remove("/registeringAccount/" + key);
        }
    }

    mutexForConfigFile.unlock();
}

void TitleBar::on_newsBtn_clicked()
{
    if( ui->newsBtn2->text().toInt() == 0)
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText( tr("No news!") );
        commonDialog.pop();
        return;
    }

    NewsDialog newsDialog;
    connect(&newsDialog, SIGNAL(showAccountPage(QString)), this, SIGNAL(showAccountPage(QString)));
    newsDialog.pop();
    onTimeOut();
}

void TitleBar::on_newsBtn2_clicked()
{
    on_newsBtn_clicked();
}

void TitleBar::on_consoleBtn_clicked()
{
    ConsoleDialog consoleDialog;
    consoleDialog.pop();

}

void TitleBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(133,16,16));
    painter.setPen(QColor(133,16,16));
    painter.drawRect(QRect(0,48,960,1));
}

void TitleBar::on_lockBtn_clicked()
{
    qDebug() << "TitleBar::on_lockBtn_clicked ";
    emit lock();
}
