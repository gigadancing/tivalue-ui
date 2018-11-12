#include <QDebug>
#include <QPainter>
#include <QHelpEvent>
#include <QDateTime>
#include <QTextCodec>
#include <QScrollBar>

#include <QDesktopServices>
#include <QAxObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QModelIndex>

#include <QUrl>

#include "accountpage.h"
#include "ui_accountpage.h"
#include "tic.h"
#include "debug_log.h"
#include <QClipboard>
#include "commondialog.h"
#include "showcontentdialog.h"
#include "rpcthread.h"
#include "control/remarkcellwidget.h"

AccountPage::AccountPage(QString name, QWidget *parent) :
    QWidget(parent),
    accountName(name),
    transactionType(0),
    address(""),
    inited(false),
    ui(new Ui::AccountPage)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(22, 24, 28));
    setPalette(palette);

    connect(Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    //
    ui->queryAssetCombo->addItem("ACS");
    ui->queryAssetCombo->addItem("3DC");
    ui->queryAssetCombo->addItem("NBJ");
    ui->queryAssetCombo->addItem("NOT");
    ui->queryAssetCombo->setCurrentIndex(0);
    ui->queryAssetCombo->setStyleSheet("QComboBox {background:rgb(37,39,43);color:rgb(190,190,190);border:none;padding: 1px 2px 1px 8px;min-width: 9em;}"
                                     "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 26px;"
                                                            "border-left-width: 1px;border-left-color: rgb(37,39,43);border-left-style: solid;"
                                                            "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                     "QComboBox::down-arrow {image: url(:/pic/ticpic/comboBoxArrow.png);}");


    mutexForAddressMap.lock();
    if (accountName.isEmpty()) { // 如果是点击账单跳转
        if (Fry::getInstance()->addressMap.size() > 0) {
            accountName = Fry::getInstance()->addressMap.keys().at(0);
        } else { // 如果还没有账户
            mutexForAddressMap.unlock();
            emit back();
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
    mutexForAddressMap.unlock();

    ui->pageLineEdit->setStyleSheet("color:rgb(190,190,190);background:rgb(37,39,43);border:1px solid rgb(37,39,43);border-radius:3px;");
    ui->pageLineEdit->setText("1");
    QIntValidator *validator = new QIntValidator(1,9999,this);
    ui->pageLineEdit->setValidator( validator );
    ui->pageLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    currentPageIndex = 1;
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->prePageBtn->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#469cfc;}");
    ui->nextPageBtn->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#469cfc;}");


    ui->accountComboBox->setStyleSheet("QComboBox {background:rgb(37,39,43);color:rgb(190,190,190);border:none;padding: 1px 2px 1px 8px;min-width: 9em;}"
                                       "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 26px;"
                                                              "border-left-width: 1px;border-left-color: rgb(37,39,43);border-left-style: solid;"
                                                              "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                       "QComboBox::down-arrow {image: url(:/pic/ticpic/comboBoxArrow.png);}");

    ui->transactionTypeComboBox->setStyleSheet("QComboBox {background:rgb(37,39,43);color:rgb(190,190,190);border:none;padding: 1px 2px 1px 8px;min-width: 9em;}"
                                               "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 26px;"
                                                                      "border-left-width: 1px;border-left-color: rgb(37,39,43);border-left-style: solid;"
                                                                      "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                               "QComboBox::down-arrow {image: url(:/pic/ticpic/comboBoxArrow.png);}");

    ui->preExportBtn->setStyleSheet("QPushButton {background:rgb(37,39,43);color:rgb(190,190,190);}");
    ui->queryBtn->setStyleSheet("QPushButton {background:rgb(37,39,43);color:rgb(190,190,190);}");

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/copyBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));

    //ui->queryBtn->hide();
    ui->preExportBtn->hide();
    ui->accountTransactionsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTransactionsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTransactionsTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->accountTransactionsTableWidget->setColumnWidth(0, 120);
    ui->accountTransactionsTableWidget->setColumnWidth(1, 130);
    ui->accountTransactionsTableWidget->setColumnWidth(2, 120);
    ui->accountTransactionsTableWidget->setColumnWidth(3, 120);
    ui->accountTransactionsTableWidget->setColumnWidth(4, 120);
    ui->accountTransactionsTableWidget->setColumnWidth(5, 120);
    ui->accountTransactionsTableWidget->setShowGrid(true);
    ui->accountTransactionsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->accountTransactionsTableWidget->setMouseTracking(true);

    ui->initLabel->hide();
    retranslator();
    init();
    updateTransactionsList();
    showTransactions( ui->transactionTypeComboBox->currentIndex());
    inited = true;

    DLOG_QT_WALLET_FUNCTION_END;
}

QString discard(const QString &str)
{
    int dotPos = str.indexOf(".");
    if( dotPos != -1)
    {
        return str.left( dotPos + 3);
    }
    else
    {
		DLOG_QT_WALLET(" no dot!");
        return NULL;
    }
}

AccountPage::~AccountPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}

void AccountPage::init()
{
    QString showName = Fry::getInstance()->addressMapValue(accountName);
    address = showName;
    showName = showName.left(11) + "......" + showName.right(3);
    ui->addressLabel->setText( showName);
    ui->addressLabel->adjustSize();
    ui->addressLabel->setGeometry(ui->addressLabel->x(),ui->addressLabel->y(),ui->addressLabel->width(),24);
    ui->copyBtn->move(ui->addressLabel->x() + ui->addressLabel->width() + 9, 39);
    ui->accountComboBox->setCurrentText(accountName);
    jsonDataUpdated("id_balance");
}

void AccountPage::updateTransactionsList()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    Fry::getInstance()->postRPC(toJsonFormat( "id_history_" + accountName, "history", QStringList() << accountName << ASSET_NAME << "9999999" ));
    DLOG_QT_WALLET_FUNCTION_END;
}

void AccountPage::refresh()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    init();
    updateTransactionsList();

    DLOG_QT_WALLET_FUNCTION_END;
}

bool isExistInWallet(QString);

//void AccountPage::showTransactions(int first)
//{
//    DLOG_QT_WALLET_FUNCTION_BEGIN;
//    ui->pageLineEdit->setText(QString::number(currentPageIndex) );
//    ui->pageLabel->setText("/" + QString::number((transactionsList.size() - 1)/10 + 1));
////    ui->numberLabel->setText( QString::fromLocal8Bit("共") + QString::number( transactionsList.size()) + QString::fromLocal8Bit("条 ,"));
//    ui->numberLabel->setText( tr("total ") + QString::number( transactionsList.size()) + tr(" ,"));

//    int size = transactionsList.size();
//    int row = size - first + 1;
//    if( row > 10)  row = 10;
//    ui->accountTransactionsTableWidget->setRowCount(row);
//    ui->accountTransactionsTableWidget->setGeometry(25,21,765,60 + row * 45);
//    if( row > 3)
//    {
//        ui->scrollAreaWidgetContents->setMinimumHeight(301 + 45 * (row - 3));
//        ui->scrollAreaWidgetContents->setGeometry(0,0,827,301 + 45 * (row - 3));
//    }
//    else
//    {
//        ui->scrollAreaWidgetContents->setMinimumHeight(301);
//        ui->scrollAreaWidgetContents->setGeometry(0,0,827,301);
//    }

//    int height = row * 45;
//    ui->prePageBtn->move(530,97 + height);
//    ui->numberLabel->move(600, 97 + height);
//    ui->pageLineEdit->move(670, 102 + height);
//    ui->pageLabel->move(700,97 + height);
//    ui->nextPageBtn->move(730,97 + height);
//    for(int i = 0;i < row; i++)
//    {
//        ui->accountTransactionsTableWidget->setRowHeight(i,45);
//        QString str = transactionsList.at(size - i - first).simplified();
//        QStringList sList = str.split(" |");

//        QString date = sList.at(0);
//        date.replace(QString("T"),QString(" "));
////         date.remove("|");
//        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
//        time = time.addSecs(28800);       // 时间加8小时
//        QString currentDateTime = time.toString("yyyy-MM-dd hh:mm");
//        ui->accountTransactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));

//        QTableWidgetItem* item1 = new QTableWidgetItem(sList.at(1));

////        if( isExistInWallet( sList.at(1)))
////        {
////            item1->setTextColor(QColor(64,154,255));
////        }
//        ui->accountTransactionsTableWidget->setItem(i,1,item1);

//        QTableWidgetItem* item2 = new QTableWidgetItem(sList.at(2));
//        if( isExistInWallet( sList.at(2)))
//        {
//            item2->setTextColor(QColor(64,154,255));
//        }
//        ui->accountTransactionsTableWidget->setItem(i,2,item2);

//         ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem(discard( sList.at(3)) ));

//         if(  sList.at(1) == accountName)
//         {
//             ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem(discard( sList.at(4)) ));
//         }
//         else
//         {
//             ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "-" ));
//         }

//         ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem( sList.at(6)));

////         if( sList.at(sList.size() - 4) != "N/A")
////         {
////             ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem(discard( sList.at(sList.size() - 5)) ));
////         }
////         else
////         {
////             ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem(QString::fromLocal8Bit("交易等待确认")));
////         }

////         QString message;
////         int j = 6;
////         if( sList.contains("N/A"))
////         {
////             for( ; j < sList.size() - 4; j++)
////             {
////                 message += sList.at(j) + " ";
////             }
////         }
////         else
////         {
////             for( ; j < sList.size() - 5; j++)
////             {
////                 message += sList.at(j) + " ";
////             }
////         }


//    }
//    DLOG_QT_WALLET_FUNCTION_END;
//}


void AccountPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(address);

    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}


void AccountPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(22,24,28),Qt::SolidPattern));
    painter.drawRect(-1,-1,794,68);
}

void AccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( inited)  // 防止accountComboBox当前值改变的时候触发
    {
        accountName = arg1;
        currentPageIndex = 1;
        ui->pageLineEdit->setText( QString::number(currentPageIndex));
        init();
        updateTransactionsList();
        showTransactions( ui->transactionTypeComboBox->currentIndex());
    }
}

void AccountPage::retranslator()
{
    ui->retranslateUi(this);
}

void AccountPage::jsonDataUpdated(QString id)
{
    if( id == "id_history_" + accountName)
    {
        QString result = Fry::getInstance()->jsonDataValue(id);
        showTransactions( ui->transactionTypeComboBox->currentIndex());
        return;
    }

    if( id == "id_balance")
    {
        QString  result = Fry::getInstance()->jsonDataValue(id);

        QStringList strList = result.split("],[").filter( "\"" + accountName + "\",");

        if( strList.isEmpty())
        {
            ui->balanceLabel->setText( "<body><font style=\"font-size:26px\" color=#a9a9a9>0.00</font><font style=\"font-size:12px\" color=#a9a9a9> TV</font></body>" );
            return;
        }

        QString str = strList.at(0);
        int pos = str.indexOf("[[0,") + 4;
        QString amount = str.mid( pos, str.indexOf("]", pos) - pos);
        amount.remove("\"");
        amount = doubleTo2Decimals( amount.toDouble() / 100000);

        ui->balanceLabel->setText( "<body><font style=\"font-size:26px\" color=#a9a9a9>" + amount + "</font><font style=\"font-size:12px\" color=#a9a9a9> TV</font></body>" );
        ui->balanceLabel->adjustSize();
        return;
    }
}

void AccountPage::on_transactionTypeComboBox_currentIndexChanged(int index)
{
    currentPageIndex = 1;
    ui->pageLineEdit->setText( QString::number(currentPageIndex));
    showTransactions( index);
}

void AccountPage::showTransactions(int type)
{
    QString result = Fry::getInstance()->jsonDataValue("id_history_" + accountName);

    if( result == "\"result\":[]" || result.isEmpty())
    {
        ui->accountTransactionsTableWidget->setRowCount(0);
        ui->prePageBtn->hide();
        ui->numberLabel->hide();
        ui->pageLineEdit->hide();
        ui->pageLabel->hide();
        ui->nextPageBtn->hide();

        if( result == "\"result\":[]")
        {
            ui->initLabel->show();
        }
        else
        {
            ui->initLabel->hide();
        }

        return;
    }

    ui->prePageBtn->show();
    ui->numberLabel->show();
    ui->pageLineEdit->show();
    ui->pageLabel->show();
    ui->nextPageBtn->show();

    ui->initLabel->hide();

    QStringList transactionsList = result.split("},{\"is_virtual\"");
    QStringList listForShow;
    switch (type)
    {
    case 0:     // 显示全部交易
        foreach (QString str, transactionsList)
        {
            int pos = str.indexOf("\"to_account\":") + 14;
            QString toAccount = str.mid( pos, str.indexOf( "\"", pos ) - pos );
            pos = str.indexOf("\"from_account\":") + 16;
            QString fromAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );
            pos = str.indexOf("\"amount\":{\"amount\":") + 19;
            QString amount = str.mid( pos, str.indexOf( ',', pos ) - pos );
            amount.remove('\"');

            if( toAccount == accountName && fromAccount == accountName &&  amount.toDouble() > 0 )
            {
                // 如果是自己给自己的转账 额外显示一条 且转出转入都做标记
                listForShow += "FIRST EXPENSE RECORD!" + str;
                listForShow += "SECOND INCOME RECORD!" + str;
            }
            else
            {
                listForShow += str;
            }
        }

        break;
    case 1:     // 显示转入交易

        foreach (QString str, transactionsList)
        {
            int pos = str.indexOf("\"to_account\":") + 14;
            QString toAccount = str.mid( pos, str.indexOf( "\"", pos ) - pos );
            pos = str.indexOf("\"from_account\":") + 16;
            QString fromAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );
            pos = str.indexOf("\"amount\":{\"amount\":") + 19;
            QString amount = str.mid( pos, str.indexOf( ',', pos ) - pos );
            amount.remove('\"');

            if( toAccount != accountName)   // 如果接收账户不为本账户，则肯定不是转入交易
            {
                continue;
            }

            if( amount.toDouble() < 0.001) // 如果金额为0，意味着是register或升级代理的交易 不在收入里显示
            {
                continue;
            }

            if( fromAccount == accountName ) // 如果是自己给自己的转账
            {
                listForShow += "SECOND INCOME RECORD!" + str;
            }
            else
            {
                listForShow += str;
            }
        }

        break;

    case 2:     // 显示转出交易

        foreach (QString str, transactionsList)
        {
            int pos = str.indexOf("\"to_account\":") + 14;
            QString toAccount = str.mid( pos, str.indexOf( "\"", pos ) - pos );
            pos = str.indexOf("\"from_account\":") + 16;
            QString fromAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );
            pos = str.indexOf("\"amount\":{\"amount\":") + 19;
            QString amount = str.mid( pos, str.indexOf( ',', pos ) - pos );
            amount.remove('\"');


            if( fromAccount != accountName)
            {
                continue;
            }

            if( toAccount == accountName && amount > 0)
            {
                listForShow += "FIRST EXPENSE RECORD!" + str;
            }
            else
            {
                listForShow += str;
            }
        }

        break;
    default:
        break;
    }

    int size = listForShow.size();
    ui->numberLabel->setText( tr("total ") + QString::number( size) + tr(" ,"));
    ui->pageLabel->setText( "/" + QString::number( (size - 1)/10 + 1 ) );

    int rowCount = size - (currentPageIndex - 1) * 10;
    if( rowCount > 10 )  rowCount = 10;  // 一页最多显示10行
    ui->accountTransactionsTableWidget->setRowCount(rowCount);


    mutexForConfigFile.lock();
    QStringList unconfirmedIdList;
    Fry::getInstance()->configFile->beginGroup("/recordId");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    foreach (QString key, keys)
    {
        if( Fry::getInstance()->configFile->value(key).toInt() == 0)
        {
            unconfirmedIdList += key;
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();

    for(int i = rowCount - 1; i > -1; i--)
    {
        ui->accountTransactionsTableWidget->setRowHeight(i,57);
        QString str = listForShow.at(  size - ( i + 1) - (currentPageIndex - 1) * 10 );

        // 对方账户
        int pos = str.indexOf("\"to_account\":") + 14;
        QString toAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );
        pos = str.indexOf("\"from_account\":") + 16;
        QString fromAccount = str.mid( pos, str.indexOf("\"", pos ) - pos );

        if( fromAccount != accountName)  // 如果 fromaccount 不为本账户，则 为对方账户
        {
            QTableWidgetItem* item = new QTableWidgetItem(fromAccount);
            if( fromAccount.mid(0, 2) != ASSET_NAME)
            {
                item->setTextColor(QColor(64,154,255));
            }
            ui->accountTransactionsTableWidget->setItem(i,1,item);
        }
        else   // 如果 fromaccount 为本账户， 则toaccount  为对方账户
        {
            QTableWidgetItem* item = new QTableWidgetItem(toAccount);
            if( toAccount.mid(0, 2) != ASSET_NAME)
            {
                item->setTextColor(QColor(64,154,255));
            }
            ui->accountTransactionsTableWidget->setItem(i,1,item);
        }

        // 时间
        pos = str.indexOf("\"timestamp\":") + 13;
        QString date = str.mid( pos, str.indexOf("\"", pos ) - pos );
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd\r\nhh:mm:ss");
        ui->accountTransactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));

        // 手续费
        pos = str.indexOf("\"fee\":{\"amount\":") + 16;
        double fee = str.mid( pos, str.indexOf(",", pos) - pos ).remove("\"").toDouble() / 100000;

        if( str.startsWith("SECOND INCOME RECORD!"))
        {
            fee = 0;
        }

        if( fee > 0.000001)
        {
            ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem( doubleTo2Decimals(fee, true)));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem( "--"));
        }

        // 金额
        pos = str.indexOf("\"amount\":{\"amount\":") + 19;
        double amount = str.mid( pos, str.indexOf(",", pos) - pos ).remove("\"").toDouble() / 100000;
        pos = str.indexOf("\"trx_id\":\"") + 10;
        QString trxId = str.mid( pos, str.indexOf("\"", pos) - pos );
        if( fee > 0.000001)
        {
            QTableWidgetItem* item = new QTableWidgetItem("-" + doubleTo2Decimals(amount));
            if( unconfirmedIdList.contains( trxId))
            {
                item->setTextColor(QColor(153,153,153));
            }
            else
            {
                item->setTextColor(QColor(184,37,37));
            }

            ui->accountTransactionsTableWidget->setItem(i,2,item);
        }
        else
        {
            QTableWidgetItem* item = new QTableWidgetItem("+" + doubleTo2Decimals(amount));
            if( unconfirmedIdList.contains( trxId))
            {
                item->setTextColor(QColor(153,153,153));
            }
            else
            {
                item->setTextColor(QColor(49,198,51));
            }

            ui->accountTransactionsTableWidget->setItem(i,2,item);
        }

//        // 账户余额
//        pos = str.indexOf("[[0,{\"amount\":") + 14;
//        QString balance = str.mid( pos, str.indexOf(",", pos) - pos );
//        balance.remove("\"");
//        if( str.startsWith("FIRST EXPENSE RECORD!"))
//        {
//            // 如果是自己转自己的 支出部分 余额里面把 交易金额扣除
//            balance =  doubleTo2Decimals(balance.toDouble() / 100000 - doubleTo2Decimals(amount).toDouble() );
//        }
//        else
//        {
//           balance = doubleTo2Decimals(balance.toDouble() / 100000);
//        }

//        ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( balance ));

        // 交易ID
        ui->accountTransactionsTableWidget->setItem(i, 4, new QTableWidgetItem( trxId));

        // 备注
        pos = str.indexOf("\"memo\":\"") + 8;
        QString remark = str.mid( pos, str.indexOf("\"", pos) - pos );

        ui->accountTransactionsTableWidget->setItem(i, 5, new QTableWidgetItem( remark));

        for(int j = 0; j < 5; j++)
        {
            ui->accountTransactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void AccountPage::on_accountTransactionsTableWidget_cellClicked(int row, int column)
{
    if( column == 1 )
    {
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);
        int scrollBarValue = ui->accountTransactionsTableWidget->verticalScrollBar()->sliderPosition();
        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(120, 57 * (row - scrollBarValue) + 42)));
        showContentDialog.exec();

        return;
    }

    if( column == 4 )
    {
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);
        int scrollBarValue = ui->accountTransactionsTableWidget->verticalScrollBar()->sliderPosition();
        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(490, 57 * (row - scrollBarValue) + 42)));
        showContentDialog.exec();

        return;
    }

    if( column == 5)
    {

        QString remark = ui->accountTransactionsTableWidget->item(row, column)->text();
        remark.remove(' ');
        if( remark.isEmpty() )  return;
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);
        int scrollBarValue = ui->accountTransactionsTableWidget->verticalScrollBar()->sliderPosition();
        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(610, 57 * (row - scrollBarValue) + 42)));
        showContentDialog.exec();
        return;
    }
}

void AccountPage::on_prePageBtn_clicked()
{
    ui->accountTransactionsTableWidget->scrollToTop();
    if( currentPageIndex == 1) return;
    currentPageIndex--;
    showTransactions( ui->transactionTypeComboBox->currentIndex());
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

}

void AccountPage::on_nextPageBtn_clicked()
{
//    if( currentPageIndex >=  ((searchList.size() - 1)/10 + 1))  return;
    int totalPageNum = ui->pageLabel->text().remove("/").toInt();
    if(  currentPageIndex >= totalPageNum )  return;

    currentPageIndex++;
    showTransactions( ui->transactionTypeComboBox->currentIndex());
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->accountTransactionsTableWidget->scrollToTop();
}

void AccountPage::on_pageLineEdit_editingFinished()
{
    currentPageIndex = ui->pageLineEdit->text().toInt();
    showTransactions( ui->transactionTypeComboBox->currentIndex());
}

void AccountPage::on_pageLineEdit_textEdited(const QString &arg1)
{
    if( arg1.at(0) == '0')
    {
        ui->pageLineEdit->setText( arg1.mid(1));
        return;
    }
    int totalPageNum = ui->pageLabel->text().remove("/").toInt();

    if( arg1.toInt() > totalPageNum)
    {
        ui->pageLineEdit->setText( QString::number( totalPageNum));
        return;
    }
}

void AccountPage::on_preExportBtn_clicked()
{
    QTableWidget *table = ui->accountTransactionsTableWidget;
    QDateTime curDateTime =QDateTime::currentDateTime();
    QString title =curDateTime.toString("yyyy.MM.dd hh:mm:ss ddd");
    QString fileName = QFileDialog::getSaveFileName(table, tr("Save"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Excel File(*.xlsx *.xls)"));

    if (fileName != "") {
        QAxObject *excel = new QAxObject;
        if (excel->setControl("Excel.Application")) { //连接Excel控件
            excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);

            int colcount=table->columnCount();
            int rowcount=table->rowCount();

            QAxObject *cell,*col;
            //标题行
            cell=worksheet->querySubObject("Cells(int,int)", 1, 1);
            cell->dynamicCall("SetValue(const QString&)", title);
            cell->querySubObject("Font")->setProperty("Size", 18);
            //调整行高
            worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);
            //合并标题行
            QString cellTitle;
            cellTitle.append("A1:");
            cellTitle.append(QChar(colcount - 1 + 'A'));
            cellTitle.append(QString::number(1));
            QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
            range->setProperty("WrapText", true);
            range->setProperty("MergeCells", true);
            range->setProperty("HorizontalAlignment", -4108);//xlCenter
            range->setProperty("VerticalAlignment", -4108);//xlCenter

            //列标题
            for(int i = 0; i < colcount; i++) {
                QString columnName;
                columnName.append(QChar(i  + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", table->columnWidth(i) / 6);
                cell=worksheet->querySubObject("Cells(int,int)", 2, i + 1);

                columnName=table->horizontalHeaderItem(i)->text();  //QTableWidget 获取表格头部文字信息

                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }

            //QTableWidget 获取表格数据部分
            for(int i = 0; i < rowcount; i++) {
                for (int j = 0; j < colcount; j++) {
                    worksheet->querySubObject("Cells(int,int)", i + 3, j + 1)->dynamicCall("SetValue(const QString&)", table->item(i, j) ? table->item(i, j)->text() : "");
                }
            }

            //画框线
            QString lrange;
            lrange.append("A2:");
            lrange.append(colcount - 1 + 'A');
            lrange.append(QString::number(table->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", lrange);
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
            //调整数据区行高
            QString rowsName;
            rowsName.append("2:");
            rowsName.append(QString::number(table->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", rowsName);
            range->setProperty("RowHeight", 20);
            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));//保存至fileName
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Export transaction records completed."));
            commonDialog.exec();

            delete excel;
            excel = nullptr;
        } else {
            QMessageBox::warning(NULL, tr("warning"), tr("please install Excel"), QMessageBox::Apply);
        }
    }
}

//
void AccountPage::on_queryBtn_clicked(bool clicked) {
    QString account_name = ui->accountComboBox->currentText();
    QString account_address = Fry::getInstance()->addressMapValue(account_name);
    bool is_opened = false;
//    if (ui->queryAssetCombo->currentText() == "ACS")
//    {
//        QUrl url(QString("http://t.top/"));
//        is_opened = QDesktopServices::openUrl(url);
//    }
//    else if (ui->queryAssetCombo->currentText() == "3DC")
//    {
//        QUrl url(QString("http://t.top/"));
//        is_opened = QDesktopServices::openUrl(url);
//    }
    if (ui->queryAssetCombo->currentText() != QString("")) {
        QUrl url(QString("http://t.top/"));
        is_opened = QDesktopServices::openUrl(url);
    }

    if (!is_opened)
    {
      CommonDialog tipDialog(CommonDialog::OkOnly);
      tipDialog.setText(tr("Invalid url"));
      tipDialog.pop();
    }
}




