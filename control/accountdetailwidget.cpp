#include "accountdetailwidget.h"
#include "ui_accountdetailwidget.h"
#include "../tic.h"
#include "../rpcthread.h"
#include "../exportdialog.h"
#include "../commondialog.h"
#include "../extra/dynamicmove.h"

#include <QDateTime>
#include <QDebug>
#include <QClipboard>

AccountDetailWidget::AccountDetailWidget( QWidget *parent) :
    QWidget(parent),
    accountName(""),
    salary(0),
    produceOrNot(false),
    ui(new Ui::AccountDetailWidget)
{
    ui->setupUi(this);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(246,247,249));
    setPalette(palette);

    QString language = Fry::getInstance()->language;
    if( language.isEmpty() || language == "Simplified Chinese")
    {
        registeredLabelString = "pic2/registeredLabel.png";
    }
    else
    {
        registeredLabelString = "pic2/registeredLabel_En.png";
    }

    ui->recentTransactionTableWidget->installEventFilter(this);
    ui->recentTransactionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->recentTransactionTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->recentTransactionTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->recentTransactionTableWidget->setShowGrid(false);
    ui->recentTransactionTableWidget->setFrameShape(QFrame::NoFrame);
    ui->recentTransactionTableWidget->setMouseTracking(true);

    ui->recentTransactionTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->recentTransactionTableWidget->horizontalHeader()->setStyleSheet("QHeaderView{background:transparent;}"
                                                              "QHeaderView::section{background-color:rgb(246,247,249);border:0px solid #ffffff;}");
    ui->recentTransactionTableWidget->horizontalHeader()->setFixedHeight(25);
    ui->recentTransactionTableWidget->horizontalHeader()->setVisible(true);
    ui->recentTransactionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for( int i = 0; i < 3; i++)
    {
        QTableWidgetItem* columnHeaderItem = ui->recentTransactionTableWidget->horizontalHeaderItem(i);// 获得水平方向表头的Item对象
        columnHeaderItem->setTextColor(QColor(134,134,134)); // 设置文字颜色
        QFont font = columnHeaderItem->font();
        font.setPixelSize(11);
        columnHeaderItem->setFont(font);
    }

    ui->recentTransactionTableWidget->setColumnWidth(0,162);
    ui->recentTransactionTableWidget->setColumnWidth(1,173);
    ui->recentTransactionTableWidget->setColumnWidth(2,140);

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(pic2/close4.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    QLabel* exportBtnLabel = new QLabel(ui->exportBtn);
    exportBtnLabel->setGeometry(63,5,13,13);
    exportBtnLabel->setPixmap(QPixmap("pic2/export_icon.png"));
    exportBtnLabel->show();
    ui->exportBtn->setStyleSheet("QToolButton{background-color:#ffffff;color:rgb(123,123,123);border:1px solid rgb(221,221,221);border-radius:3px;}"
                                 "QToolButton:hover{color:rgb(150,150,150);}");

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/copyBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));

    ui->upgradeBtn->setStyleSheet("QToolButton{background-color:#469cfc;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:3px;}"
                               "QToolButton:hover{background-color:#62a9f8;}"
                                  "QToolButton:disabled{background-color:#cecece;}");

    QLabel* transferBtnLabel = new QLabel(ui->transferBtn);
    transferBtnLabel->setGeometry(18,6,13,13);
    transferBtnLabel->setPixmap(QPixmap("pic2/transfer3_icon.png"));
    transferBtnLabel->show();
    ui->transferBtn->setStyleSheet("QToolButton{background-color:#469cfc;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:3px;}"
                               "QToolButton:hover{background-color:#62a9f8;}"
                               "QToolButton:disabled{background-color:#cecece;}");


    qrCodeWidget = new QRCodeWidget(this);
    qrCodeWidget->setGeometry(223,53,87,87);

    ui->noTransactionLabel->hide();

}

AccountDetailWidget::~AccountDetailWidget()
{
    delete ui;
}

void AccountDetailWidget::setAccount(QString name)
{
    if( accountName != name)   // 如果accountname 没改 produceornot保留原值
    {
        produceOrNot = false;
    }

    accountName = name;
    QString showName;
    if( accountName.size() > 13)
    {
        showName = accountName.left(11) + "...";
    }
    else
    {
        showName = accountName;
    }
    ui->nameLabel->setText(showName);
    ui->nameLabel->adjustSize();
    ui->nameLabel->move( ( this->width() - ui->nameLabel->width()) / 2 , 13);
    ui->identityLabel->move( ui->nameLabel->x() - 25, ui->nameLabel->y());


    QString address = Fry::getInstance()->addressMapValue(accountName);
    ui->addressLabel->setText( address);
    ui->addressLabel->adjustSize();
    ui->addressLabel2->adjustSize();
    ui->addressLabel->move( (this->width() - ui->addressLabel->width()) / 2, 154);
    ui->addressLabel2->move( ui->addressLabel->x() - 38, ui->addressLabel->y());
    ui->copyBtn->move(ui->addressLabel->x() + ui->addressLabel->width() + 9, ui->addressLabel->y() - 1);
    qrCodeWidget->setString(address);

    QString balance = Fry::getInstance()->balanceMapValue(accountName);
    balance.remove(" TV");
    ui->balanceLabel->setText( QString( "<body><font style=\"font-size:15px\" color=#000000><B>") + balance + "</B></font><font style=\"font-size:10px\" color=#000000> TV</font></body>" );
    ui->balanceLabel->adjustSize();
    if( balance.toDouble() < 0.01 - 0.000001)
    {
        ui->transferBtn->setEnabled(false);
        ui->upgradeBtn->setEnabled(false);
    }
    else if(  balance.toDouble() < 0.01 + 0.000001)
    {
        ui->transferBtn->setEnabled(false);
        ui->upgradeBtn->setEnabled(true);
    }
    else
    {
        ui->transferBtn->setEnabled(true);
        ui->upgradeBtn->setEnabled(true);
    }

    Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_transaction_history_splite_" + accountName, "wallet_transaction_history_splite", QStringList() << accountName << ASSET_NAME << "0" << "2" ));
    // 如果是已注册账户
    if( Fry::getInstance()->registerMapValue(accountName) != "NO"
            && !Fry::getInstance()->registerMapValue(accountName).isEmpty())
    {
        ui->upgradeBtn->hide();
        ui->identityLabel->setPixmap(QPixmap(registeredLabelString));

        ui->balanceLabel2->move( (this->width() - ui->balanceLabel->width() - 42) / 2 ,195);
        ui->balanceLabel->move( ui->balanceLabel2->x() + 42, 195);
        ui->transferBtn->move( 213, 230);
    }
    else  // 如果账户未升级
    {
        ui->upgradeBtn->show();
        ui->identityLabel->setPixmap(QPixmap(""));

        ui->balanceLabel2->move( (this->width() - ui->balanceLabel->width() - 42) / 2 ,195);
        ui->balanceLabel->move( ui->balanceLabel2->x() + 42, 195);
        ui->transferBtn->move( 213, 230);
    }


}

void AccountDetailWidget::showRecentTransactions()
{
    QString result = Fry::getInstance()->jsonDataValue("id_wallet_transaction_history_splite_" + accountName);

    if( result == "\"result\":[]" || result.isEmpty())
    {
        ui->recentTransactionTableWidget->hide();
        ui->noTransactionLabel->show();
        ui->moreTransactionBtn->hide();
        return;
    }
    else
    {
        ui->recentTransactionTableWidget->show();
        ui->noTransactionLabel->hide();
    }


    QStringList transactionsList = result.split("},{\"is_virtual\"");
    QStringList listForShow;
    foreach (QString str, transactionsList)
    {
        int pos = str.indexOf("\"to_account\":") + 14;
        QString toAccount = str.mid( pos, str.indexOf( "\"", pos ) - pos );
        pos = str.indexOf("\"from_account\":") + 16;
        QString fromAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );
        pos = str.indexOf("\"amount\":{\"amount\":") + 19;
        QString amount = str.mid( pos, str.indexOf( ',', pos ) - pos );
        amount.remove('\"');

        if( toAccount == accountName && fromAccount == accountName
                &&  amount.toDouble() > 0 )
        {
            // 如果是自己给自己的转账 额外显示一条 且转出转入都做标记
            listForShow += "FIRST EXPENSE RECORD!" + str;
            listForShow += "SECOND INCOME RECORD!" + str;
        }
        else{
            listForShow += str;
        }
    }

    int size = listForShow.size();
    int rowCount = size;
    if( rowCount > 3 )
    {
        rowCount = 3;  // 最多显示3条
        ui->moreTransactionBtn->show();
    }
    else
    {
        ui->moreTransactionBtn->hide();
    }
    ui->recentTransactionTableWidget->setRowCount(rowCount);


    for(int i = rowCount - 1;i > -1; i--)
    {
        ui->recentTransactionTableWidget->setRowHeight(i,25);
        QString str = listForShow.at(  size - ( i + 1) );

        // 对方账户
        int pos = str.indexOf("\"to_account\":") + 14;
        QString toAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );
        pos = str.indexOf("\"from_account\":") + 16;
        QString fromAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );

        if( fromAccount != accountName)  // 如果 fromaccount 不为本账户，则 为对方账户
        {
            QTableWidgetItem* item = new QTableWidgetItem(fromAccount);
            if( fromAccount.mid(0,2) != ASSET_NAME)
            {
                item->setTextColor(QColor(64,154,255));
            }
            ui->recentTransactionTableWidget->setItem(i,1,item);
        }
        else   // 如果 fromaccount 为本账户， 则toaccount  为对方账户
        {
            QTableWidgetItem* item = new QTableWidgetItem(toAccount);
            if( toAccount.mid(0,2) != ASSET_NAME)
            {
                item->setTextColor(QColor(64,154,255));
            }
            ui->recentTransactionTableWidget->setItem(i,1,item);
        }

        // 时间
        pos = str.indexOf("\"timestamp\":") + 13;
        QString date = str.mid( pos, str.indexOf("\"", pos ) - pos );
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd");
        ui->recentTransactionTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));

        // 手续费
        pos = str.indexOf("\"fee\":{\"amount\":") + 16;
        double fee = str.mid( pos, str.indexOf(",", pos) - pos ).remove("\"").toDouble() / 100000;

        if( str.startsWith("SECOND INCOME RECORD!"))
        {
            fee = 0;
        }

        // 金额
        pos = str.indexOf("\"amount\":{\"amount\":") + 19;
        double amount = str.mid( pos, str.indexOf(",", pos) - pos ).remove("\"").toDouble() / 100000;
        pos = str.indexOf("\"trx_id\":\"") + 10;
        QString trxId = str.mid( pos, str.indexOf("\"", pos) - pos );
        if( fee > 0.000001)
        {
            QTableWidgetItem* item = new QTableWidgetItem("-" + doubleTo2Decimals(amount));
            item->setTextColor(QColor(255,80,63));
            ui->recentTransactionTableWidget->setItem(i,2,item);
        }
        else
        {
            QTableWidgetItem* item = new QTableWidgetItem("+" + doubleTo2Decimals(amount));
            item->setTextColor(QColor(71,178,156));
            ui->recentTransactionTableWidget->setItem(i,2,item);
        }

    }

}

void AccountDetailWidget::dynamicShow()
{
    show();
    DynamicMove* dynamicMove = new DynamicMove( this, QPoint(297,93), 20, 10, this);
    dynamicMove->start();
}

void AccountDetailWidget::dynamicHide()
{
    DynamicMove* dynamicMove = new DynamicMove( this, QPoint(826,93), 20, 10, this);
    connect( dynamicMove, SIGNAL(moveEnd()), this, SLOT(moveEnd()));
    dynamicMove->start();
}

void AccountDetailWidget::refresh()
{
//    QString balance = Fry::getInstance()->balanceMapValue(accountName);
//    balance.remove(" TV");
//    ui->balanceLabel->setText( QString( "<body><font style=\"font-size:15px\" color=#000000><B>") + balance + "</B></font><font style=\"font-size:10px\" color=#000000> TV</font></body>" );
//    ui->balanceLabel->adjustSize();
//    if( balance.toDouble() < 0.010001)
//    {
//        ui->transferBtn->setEnabled(false);
//        ui->upgradeBtn->setEnabled(false);
//    }
//    else
//    {
//        ui->transferBtn->setEnabled(true);
//        ui->upgradeBtn->setEnabled(true);
//    }

}

void AccountDetailWidget::on_closeBtn_clicked()
{
    emit back();
}

void AccountDetailWidget::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_transaction_history_splite_" + accountName )
    {
        showRecentTransactions();
        return;
    }


    if( id == "id_wallet_set_transaction_scanning" )
    {
//        qDebug() << id << Fry::getInstance()->jsonDataValue(id);
        return;
    }

}

void AccountDetailWidget::on_moreTransactionBtn_clicked()
{
    emit showAccountPage(accountName);
}

void AccountDetailWidget::on_exportBtn_clicked()
{
    ExportDialog exportDialog(accountName);
    exportDialog.pop();
}

void AccountDetailWidget::on_transferBtn_clicked()
{
    emit showTransferPage(accountName);
}

void AccountDetailWidget::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->addressLabel->text());

    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}

void AccountDetailWidget::on_upgradeBtn_clicked()
{
    // 如果是已注册账户
    if( Fry::getInstance()->registerMapValue(accountName) != "NO"
            && !Fry::getInstance()->registerMapValue(accountName).isEmpty())
    {

    }
    else
    {      
        emit upgrade(accountName);
    }

}

void AccountDetailWidget::moveEnd()
{
    // 移出mainpage显示范围的时候 最后一帧会导致mainpage挡住bottombar
    // 先隐藏 dynamicshow时再显示
    hide();
}
