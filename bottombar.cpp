#include "bottombar.h"
#include "ui_bottombar.h"
#include "consoledialog.h"
#include "tic.h"
#include "debug_log.h"
#include "commontip.h"
#include "rpcthread.h"
#include "commondialog.h"
#include "extra/dynamicmove.h"

#include <QPainter>
#include <QTimer>
#include <QMovie>
#include <QMouseEvent>

BottomBar::BottomBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BottomBar)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    ui->networkLabel->setPixmap(QPixmap(":/pic/pic2/network.png"));
    ui->syncLabel->setStyleSheet("color:rgb(102,102,102)");
    ui->syncLabel->setToolTip(QString::fromLocal8Bit("本地最新区块/网络最新区块"));


    connect(Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)),this, SLOT(jsonDataUpdated(QString)));
    jsonDataUpdated("id_info");

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(25,27,31));
    setPalette(palette);

    timer = new QTimer(this);
    timer->start(5000);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateNumOfConnections()));
    updateNumOfConnections();

    connectionTip = new CommonTip;

    DLOG_QT_WALLET_FUNCTION_END;
}

BottomBar::~BottomBar()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}



void BottomBar::updateNumOfConnections()
{
//    DLOG_QT_WALLET_FUNCTION_BEGIN;

    QString result = Fry::getInstance()->jsonDataValue("id_info");
    if( result.isEmpty())  return;

    int pos = result.indexOf("\"network_num_connections\"") + 26;
    QString num = result.mid( pos, result.indexOf("," , pos) - pos);

    ui->nodeNumLabel->setText(num);


//    DLOG_QT_WALLET_FUNCTION_END;
}


void BottomBar::retranslator()
{
    ui->retranslateUi(this);
}

void BottomBar::jsonDataUpdated(QString id)
{
    if( id != "id_info")  return;

    QString result = Fry::getInstance()->jsonDataValue( id);
    if( result.isEmpty() )  return;

    int pos = result.indexOf( "\"blockchain_head_block_age\":") + 28;
    QString seconds = result.mid( pos, result.indexOf("\"blockchain_head_block_timestamp\":") - pos - 1);

    int pos2 = result.indexOf( "\"blockchain_head_block_num\":") + 28;
    QString num = result.mid( pos2, result.indexOf("\"blockchain_head_block_age\":") - pos2 - 1);

    if( seconds.toInt() < 0)  seconds = "0";
    int numToSync = seconds.toInt()/ 10;


    ui->syncLabel->setText( num + " / " + QString::number( num.toInt() + numToSync) );


    int pos3 = result.indexOf( "\"wallet_scan_progress\":") + 23;
    QString scanProgress = result.mid( pos3, result.indexOf(",\"wallet_block_production_enabled\":") - pos3);

    scanProgress.remove('\"');
    double scanPercent = scanProgress.toDouble();

    if( Fry::getInstance()->needToScan && scanPercent > 0.99999)
    {
        Fry::getInstance()->postRPC( toJsonFormat( "id_scan", "scan", QStringList() << "0"));
        Fry::getInstance()->needToScan = false;
    }

}

void BottomBar::refresh()
{
    Fry::getInstance()->postRPC( toJsonFormat( "id_info", "info", QStringList() << ""));
}
