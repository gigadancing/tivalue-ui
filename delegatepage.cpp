#include <QPainter>
#include <QDebug>

#include "delegatepage.h"
#include "ui_delegatepage.h"
#include "fry.h"
#include "debug_log.h"
#include "searchoptionwidget.h"
#include "rpcthread.h"
#include "commondialog.h"

#define DELEGATEPAGE_BUTTON_SELECTED   "QToolButton{background-color:#e9e9e9;color:#353535;border:0px;}"
#define DELEGATEPAGE_BUTTON_UNSELECTED "QToolButton{background-color:#ffffff;color:#353535;border:0px;}"

DelegatePage::DelegatePage(QWidget *parent) :
    QWidget(parent),
    currentPageIndex(1),
    currentPageIndex2(1),
    ui(new Ui::DelegatePage)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    setPalette(palette);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
    searchOptionWidget = new SearchOptionWidget(this);
    searchOptionWidget->hide();
    searchOptionWidget->setFocusProxy(ui->searchLineEdit);
    searchOptionWidget->move( 594, 79);
    ui->searchBtn->setFocusProxy(ui->searchLineEdit);

    jsonDataUpdated( "id_wallet_get_all_approved_accounts");
    updateDelegateList();
    showDelegates(1);

    updateIncomeLabel();

    on_totalBtn_clicked();

    ui->delegateLabel2->adjustSize();

    ui->incomeLabel3->move(ui->delegateLabel2->x() + ui->delegateLabel2->width() + 20, 37);
    ui->incomeLabel3->adjustSize();
    ui->incomeLabel->move(ui->incomeLabel3->x() + ui->incomeLabel3->width() + 13, 25);
    ui->incomeLabel->adjustSize();

    QLabel* applyBtnLabel = new QLabel(ui->applyBtn);
    applyBtnLabel->setGeometry(18,11,16,14);
    applyBtnLabel->setPixmap(QPixmap("pic2/applyDelegate_icon.png"));
    applyBtnLabel->show();
    applyBtnLabel->setStyleSheet("background:transparent;");
    ui->applyBtn->setStyleSheet("QToolButton{background-color:#469cfc;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:3px;}"
                               "QToolButton:hover{background-color:#62a9f8;}");

    QString language = Fry::getInstance()->language;
    if( language.isEmpty() || language == "Simplified Chinese")
    {

        ui->tipLabel1->setPixmap( QPixmap("pic2/supportTip.png"));
        ui->tipLabel1->setGeometry(317,91,169,67);
        ui->tipLabel2->setPixmap( QPixmap("pic2/stabilityTip.png"));
        ui->tipLabel2->setGeometry(475,91,176,67);
        ui->infoBtn1->move(393,160);
        ui->infoBtn2->move(553,160);
    }
    else
    {
        ui->tipLabel1->setPixmap( QPixmap("pic2/supportTip_En.png"));
        ui->tipLabel1->setGeometry(282,91,300,67);
        ui->tipLabel2->setPixmap( QPixmap("pic2/stabilityTip_En.png"));
        ui->tipLabel2->setGeometry(377,91,406,67);
        ui->infoBtn1->move(416,160);
        ui->infoBtn2->move(575,160);

    }


    ui->applyBtn->installEventFilter(this);

    ui->favoriteDelegateTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->favoriteDelegateTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->favoriteDelegateTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->favoriteDelegateTableWidget->setColumnWidth(0,115);
    ui->favoriteDelegateTableWidget->setColumnWidth(1,200);
    ui->favoriteDelegateTableWidget->setColumnWidth(2,160);
    ui->favoriteDelegateTableWidget->setColumnWidth(3,150);
    ui->favoriteDelegateTableWidget->setColumnWidth(4,125);
    ui->favoriteDelegateTableWidget->setColumnWidth(5,0);
    ui->favoriteDelegateTableWidget->setShowGrid(false);
    ui->favoriteDelegateTableWidget->setStyleSheet("QTableView::item { border-bottom: 1px dashed rgb(180,180,180); }");
    ui->favoriteDelegateTableWidget->setFrameShape(QFrame::NoFrame);
    ui->favoriteDelegateTableWidget->setMouseTracking(true);

    ui->favoriteDelegateTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->favoriteDelegateTableWidget->horizontalHeader()->setStyleSheet("QHeaderView{background:transparent;}"
                                                                       "QHeaderView::section{background-color:white;border:0px solid #ffffff;}");
    ui->favoriteDelegateTableWidget->horizontalHeader()->setFixedHeight(36);
    ui->favoriteDelegateTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for( int i = 0; i < 5; i++)
    {
        QTableWidgetItem* columnHeaderItem = ui->favoriteDelegateTableWidget->horizontalHeaderItem(i);// 获得水平方向表头的Item对象
        columnHeaderItem->setTextColor(QColor(134,134,134)); // 设置文字颜色
    }

    ui->searchLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->searchLineEdit->setTextMargins(8,0,60,0);
    ui->searchLineEdit->installEventFilter(this);
    ui->searchLineEdit->setPlaceholderText(tr("Search keywords"));

    ui->delegateTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->delegateTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->delegateTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->delegateTableWidget->setColumnWidth(0,115);
    ui->delegateTableWidget->setColumnWidth(1,200);
    ui->delegateTableWidget->setColumnWidth(2,160);
    ui->delegateTableWidget->setColumnWidth(3,150);
    ui->delegateTableWidget->setColumnWidth(4,125);
    ui->delegateTableWidget->setColumnWidth(5,0);
    ui->delegateTableWidget->setShowGrid(false);
    ui->delegateTableWidget->setStyleSheet("QTableView::item { border-bottom: 1px dashed rgb(180,180,180); }");
    ui->delegateTableWidget->setFrameShape(QFrame::NoFrame);
    ui->delegateTableWidget->setMouseTracking(true);

    ui->delegateTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->delegateTableWidget->horizontalHeader()->setStyleSheet("QHeaderView{background:transparent;}"
                                                               "QHeaderView::section{background-color:white;border:0px solid #ffffff;}");
    ui->delegateTableWidget->horizontalHeader()->setFixedHeight(36);
    ui->delegateTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for( int i = 0; i < 5; i++)
    {
        QTableWidgetItem* columnHeaderItem = ui->delegateTableWidget->horizontalHeaderItem(i);// 获得水平方向表头的Item对象
        columnHeaderItem->setTextColor(QColor(134,134,134)); // 设置文字颜色
    }

    ui->prePageBtn->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#469cfc;}");
    ui->nextPageBtn->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#469cfc;}");
    ui->prePageBtn2->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#469cfc;}");
    ui->nextPageBtn2->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#469cfc;}");

    ui->searchBtn->setIcon(QPixmap("pic2/search"));
    ui->searchBtn->setIconSize(QSize(16,16));
    ui->searchBtn->setStyleSheet("background:transparent");


    ui->infoBtn1->setStyleSheet("QToolButton{background-image:url(pic2/info.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->infoBtn2->setStyleSheet("QToolButton{background-image:url(pic2/info.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->infoBtn1->installEventFilter(this);
    ui->infoBtn2->installEventFilter(this);

    ui->tipLabel1->hide();
    ui->tipLabel2->hide();

    ui->pageLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->pageLineEdit->setText("1");
    ui->pageLineEdit2->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->pageLineEdit2->setText("1");
    QIntValidator *validator = new QIntValidator(1,9999,this);
    ui->pageLineEdit->setValidator( validator );
    QIntValidator *validator2 = new QIntValidator(1,9999,this);
    ui->pageLineEdit2->setValidator( validator2 );
    ui->pageLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pageLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);


    ui->stackedWidget->setCurrentIndex(0);

    ui->loadingWidget->setGeometry(0,93,827,448);
    ui->loadingLabel->move(314,101);

    DLOG_QT_WALLET_FUNCTION_END;
}

DelegatePage::~DelegatePage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}


void DelegatePage::updateDelegateList()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_list_delegates", "blockchain_list_delegates", QStringList() << "1" << "9999"));

    DLOG_QT_WALLET_FUNCTION_END;
}

void DelegatePage::on_delegateTableWidget_cellClicked(int row, int column)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    // 防止未显示出来时 点击
    if( searchList.isEmpty()) return;

    if( column == 4)
    {
        QString name = ui->delegateTableWidget->item(row,1)->text();

        QLabel* approveLabel = new QLabel();
        approveLabel->setMargin(4);
        approveLabel->setCursor(Qt::PointingHandCursor);

        if( Fry::getInstance()->jsonDataValue("id_wallet_get_all_approved_accounts").contains("\"name\":\"" + name + "\"") )
        {
            Fry::getInstance()->postRPC( toJsonFormat( "id_approve", "approve", QStringList() << name << "0" ));

            approveLabel->setPixmap(QPixmap("pic2/notapprove.png"));
        }
        else
        {

            Fry::getInstance()->postRPC( toJsonFormat( "id_approve", "approve", QStringList() << name << "1" ));

            approveLabel->setPixmap(QPixmap("pic2/approve.png"));
        }

//        updateDelegateList();
        ui->delegateTableWidget->setCellWidget(row,4, approveLabel);

    }
    DLOG_QT_WALLET_FUNCTION_END;
}

void DelegatePage::updateFavoriteDelegateList()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

//    RpcThread* rpcThread = new RpcThread;
//    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//    rpcThread->setLogin("a","b");
//    rpcThread->setWriteData( toJsonFormat( "id_wallet_get_all_approved_accounts", "wallet_get_all_approved_accounts", QStringList() << "1") );
//    rpcThread->start();

    Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_get_all_approved_accounts", "wallet_get_all_approved_accounts", QStringList() << "1"));

    DLOG_QT_WALLET_FUNCTION_END;

}

void DelegatePage::on_favoriteDelegateTableWidget_cellClicked(int row, int column)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( column == 4)
    {
        QString name = ui->favoriteDelegateTableWidget->item(row,1)->text();

//        RpcThread* rpcThread = new RpcThread;
//        connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//        rpcThread->setLogin("a","b");
//        rpcThread->setWriteData( toJsonFormat( "id_approve", "approve", QStringList() << name << "0" ));
//        rpcThread->start();

        Fry::getInstance()->postRPC( toJsonFormat( "id_approve", "approve", QStringList() << name << "0" ));
    }
    DLOG_QT_WALLET_FUNCTION_END;
}

void DelegatePage::updateIncomeLabel()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

//    RpcThread* rpcThread = new RpcThread;
//    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//    rpcThread->setWriteData( toJsonFormat( "id_blockchain_get_info", "blockchain_get_info", QStringList() << ""));
//    rpcThread->start();

    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_info", "blockchain_get_info", QStringList() << ""));

    DLOG_QT_WALLET_FUNCTION_END;
}


void DelegatePage::on_applyBtn_clicked()
{
    if( applyEnable() > 0 )
    {
        emit applyDelegate("");
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText( tr("No account can apply for an agent.") );
        commonDialog.pop();
    }


}

void DelegatePage::on_searchBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    searchList.clear();
    switch (searchOptionWidget->currentChoice)
    {
        case 0:
//            mutexForDelegateList.lock();
            for(QString &str : Fry::getInstance()->delegateList)
            {

                int pos = str.indexOf("\"name\":") + 8;
                QString name  = str.mid( pos, str.indexOf( "\"", pos) - pos);
                QString index = str.mid( str.indexOf("\"index\":") + 8);

                if( name.contains( ui->searchLineEdit->text().simplified().toLower()) || index.contains( ui->searchLineEdit->text().simplified().toLower()))
                {
                    searchList += str;
                }
            }
//            mutexForDelegateList.unlock();

            break;
        case 1:
//            mutexForDelegateList.lock();
            for(QString &str : Fry::getInstance()->delegateList)
            {
                int pos = str.indexOf("\"name\":") + 8;
                QString name  = str.mid( pos, str.indexOf( "\"", pos) - pos);

                if( name.contains( ui->searchLineEdit->text().simplified().toLower()) )
                {
                    searchList += str;
                }
            }
//            mutexForDelegateList.unlock();
            break;
        case 2:
//            mutexForDelegateList.lock();
            for(QString &str : Fry::getInstance()->delegateList)
            {
                QString index = str.mid( str.indexOf("\"index\":") + 8);

                if( index.contains( ui->searchLineEdit->text().simplified().toLower()))
                {
                    searchList += str;
                }
            }
//            mutexForDelegateList.unlock();
            break;
        default:
            break;
    }

    currentPageIndex = 1;
    showDelegates(1);
    ui->pageLineEdit->setText("1");
    ui->pageLabel->setText("/" + QString::number((searchList.size() - 1)/10 + 1));
    DLOG_QT_WALLET_FUNCTION_END;
}

void DelegatePage::showDelegates(int first)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    QString infoStr = Fry::getInstance()->jsonDataValue("id_info");
    int posOfShare = infoStr.indexOf("\"blockchain_share_supply\":") + 26;
    QString sharesAmount = infoStr.mid( posOfShare, infoStr.indexOf( ",", posOfShare) - posOfShare);
    sharesAmount.remove("\"");

    int row = searchList.size() - first + 1;

    ui->numberLabel->setText( tr("total ") + QString::number( searchList.size()) + tr(" ,"));
    if( row > 10)
    {
        row = 10;
        ui->nextPageBtn->setEnabled(true);
    }
    else
    {
        ui->nextPageBtn->setEnabled(false);
    }

    ui->delegateTableWidget->setRowCount(row);
    for(int i = 0;i < row; i++)
    {
        ui->delegateTableWidget->setRowHeight(i,56);

        QString str = searchList.at(first + i - 1);

        ui->delegateTableWidget->setItem(i,0,new QTableWidgetItem(  str.mid( str.indexOf("\"index\":") + 8) ) );

        int pos = str.indexOf("\"name\":") + 8;
        QString name = str.mid( pos, str.indexOf( "\"", pos) - pos);
        QTableWidgetItem* item = new QTableWidgetItem( name );
        item->setTextColor(QColor(64,154,255));
        ui->delegateTableWidget->setItem(i,1,item);

        pos = str.indexOf("\"votes_for\":") + 12;
        QString votes = str.mid( pos, str.indexOf( "\,", pos) - pos);
        votes.remove("\"");
        ui->delegateTableWidget->setItem(i,2,new QTableWidgetItem( doubleTo2Decimals( votes.toDouble() * 100 / sharesAmount.toDouble()) + "\%" ));

        pos = str.indexOf("\"blocks_produced\":") + 18;
        double blockProduced = str.mid( pos, str.indexOf( ",", pos) - pos).toDouble();
        pos = str.indexOf("\"blocks_missed\":") + 16;
        double blockMissed   = str.mid( pos, str.indexOf( "}", pos) - pos).toDouble();
        if( blockProduced < 0.000001 && blockMissed < 0.000001)
        {
            ui->delegateTableWidget->setItem(i,3,new QTableWidgetItem( "N/A" ));
        }
        else if( blockProduced < 0.000001 && blockMissed > 0.000001)
        {
            ui->delegateTableWidget->setItem(i,3,new QTableWidgetItem( "0\%" ));
        }
        else
        {
            double stability = blockProduced / ( blockProduced + blockMissed) * 100;
            ui->delegateTableWidget->setItem(i,3,new QTableWidgetItem( doubleTo2Decimals( stability) + "\%" ));
        }

        QLabel* approveLabel = new QLabel();
        approveLabel->setMargin(4);
        approveLabel->setCursor(Qt::PointingHandCursor);

        if( Fry::getInstance()->jsonDataValue("id_wallet_get_all_approved_accounts").contains("\"name\":\"" + name + "\"") )
        {
            approveLabel->setPixmap(QPixmap("pic2/approve.png"));
        }
        else
        {
            approveLabel->setPixmap(QPixmap("pic2/notapprove.png"));
        }

        ui->delegateTableWidget->setCellWidget(i,4, approveLabel);
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void DelegatePage::showFavoriteDelegates(int first)
{

    QString infoStr = Fry::getInstance()->jsonDataValue("id_info");
    int posOfShare = infoStr.indexOf("\"blockchain_share_supply\":") + 26;
    QString sharesAmount = infoStr.mid( posOfShare, infoStr.indexOf( ",", posOfShare) - posOfShare);
    sharesAmount.remove("\"");

    int row = approveList.size() - first + 1;

    ui->numberLabel2->setText( tr("total ") + QString::number( approveList.size()) + tr(" ,"));
    if( row > 10)
    {
        row = 10;
        ui->nextPageBtn2->setEnabled(true);
    }
    else if( row == 0 && currentPageIndex2 > 1)  // 如果删除后正好是整10个 少了一页
    {
        currentPageIndex2--;
        ui->nextPageBtn2->setEnabled(false);
        ui->pageLineEdit2->setText( QString::number(currentPageIndex2));
        on_pageLineEdit2_editingFinished();
        return;
    }
    else
    {
        ui->nextPageBtn2->setEnabled(false);
    }
    ui->pageLabel2->setText("/" + QString::number((approveList.size() - 1)/10 + 1));


//    ui->favoriteDelegateTableWidget->clearContents();

    ui->favoriteDelegateTableWidget->setRowCount(row);
//    ui->favoriteDelegateTableWidget->setGeometry(50,2,750,51 + row * 45);

//        mutexForDelegateList.lock();
    for (int i = 0; i < row; i++)
    {
        ui->favoriteDelegateTableWidget->setRowHeight(i,56);

        int pos2 = approveList.at(first + i - 1).indexOf(",\"name\":\"") + 9;
        QString approveName = approveList.at(first + i - 1).mid( pos2, approveList.at(first + i - 1).indexOf("\"", pos2) - pos2);

        QStringList singleList = Fry::getInstance()->delegateList.filter( "\"name\":\"" + approveName + "\"");

        if( singleList.size() == 0) return;

        QString str = singleList.at(0);
        ui->favoriteDelegateTableWidget->setItem(i,0,new QTableWidgetItem(  str.mid( str.indexOf("\"index\":") + 8) ) );

        int pos = str.indexOf("\"name\":") + 8;
        QString name = str.mid( pos, str.indexOf( "\"", pos) - pos);
        QTableWidgetItem* item = new QTableWidgetItem( name );
        item->setTextColor(QColor(64,154,255));
        ui->favoriteDelegateTableWidget->setItem(i,1,item);

        pos = str.indexOf("\"votes_for\":") + 12;
        QString votes = str.mid( pos, str.indexOf( ",", pos) - pos);
        votes.remove("\"");
        ui->favoriteDelegateTableWidget->setItem(i,2,new QTableWidgetItem( doubleTo2Decimals( votes.toDouble() * 100 / sharesAmount.toDouble()) + "\%" ));

        pos = str.indexOf("\"blocks_produced\":") + 18;
        double blockProduced = str.mid( pos, str.indexOf( ",", pos) - pos).toDouble();
        pos = str.indexOf("\"blocks_missed\":") + 16;
        double blockMissed   = str.mid( pos, str.indexOf( "}", pos) - pos).toDouble();
        if( blockProduced < 0.000001 && blockMissed < 0.000001)
        {
            ui->favoriteDelegateTableWidget->setItem(i,3,new QTableWidgetItem( "N/A" ));
        }
        else if( blockProduced < 0.000001 && blockMissed > 0.000001)
        {
            ui->favoriteDelegateTableWidget->setItem(i,3,new QTableWidgetItem( "0\%" ));
        }
        else
        {
            double stability = blockProduced / ( blockProduced + blockMissed) * 100;
            ui->favoriteDelegateTableWidget->setItem(i,3,new QTableWidgetItem( doubleTo2Decimals( stability) + "\%" ));
        }

        QLabel* approveLabel = new QLabel();
        approveLabel->setMargin(4);
        approveLabel->setPixmap(QPixmap("pic2/approve.png"));
        approveLabel->setCursor(Qt::PointingHandCursor);
        ui->favoriteDelegateTableWidget->setCellWidget(i,4, approveLabel);

//            ui->favoriteDelegateTableWidget->sortByColumn(0, Qt::AscendingOrder);
    }

}

bool DelegatePage::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->infoBtn1)
    {
        if( e->type() == QEvent::Leave)
        {
            ui->tipLabel1->hide();
            return true;
        }
    }

    if( watched == ui->infoBtn2)
    {
        if( e->type() == QEvent::Leave)
        {
            ui->tipLabel2->hide();
            return true;
        }
    }

//    if(watched == ui->scrollAreaWidgetContents)
//    {
//        if(e->type() == QEvent::Paint){
//            paintOnScrollarea(ui->scrollAreaWidgetContents);
//            return true;
//        }
//    }

    if(watched == ui->searchLineEdit)
    {
        if(e->type() == QEvent::FocusIn)
        {
            searchOptionWidget->show();
        }
        else if( e->type() == QEvent::FocusOut)
        {
            if( !searchOptionWidget->hasFocus())
            {
                searchOptionWidget->hide();
            }
        }
    }

    return QWidget::eventFilter(watched,e);
}

//void DelegatePage::paintOnScrollarea(QWidget *w)
//{
//    QPainter painter(w);
//    painter.setPen(QPen(QColor(177,183,188),Qt::SolidLine));
//    painter.drawLine( QPoint(31,59), QPoint( 797,58));
//}



void DelegatePage::on_prePageBtn_clicked()
{
    ui->delegateTableWidget->scrollToTop();
    if( currentPageIndex == 1) return;
    currentPageIndex--;
    showDelegates(currentPageIndex * 10 - 9);
    ui->pageLabel->setText("/" + QString::number((searchList.size() - 1)/10 + 1));
    ui->pageLineEdit->setText( QString::number(currentPageIndex));
}

void DelegatePage::on_nextPageBtn_clicked()
{
    if( currentPageIndex >=  ((searchList.size() - 1)/10 + 1))  return;

    currentPageIndex++;
    showDelegates(currentPageIndex * 10 - 9);
    ui->pageLabel->setText("/" + QString::number((searchList.size() - 1)/10 + 1));
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->delegateTableWidget->scrollToTop();
}

int applyEnable()
{
    int numberOfAccountsCanApply = 0;      //  非代理的账号个数

    mutexForRegisterMap.lock();
    QStringList keys = Fry::getInstance()->registerMap.keys();
    mutexForRegisterMap.unlock();

    foreach (QString key, keys)
    {
        if(  !Fry::getInstance()->delegateAccountList.contains(key))
        {
            numberOfAccountsCanApply++;
        }
    }
    return numberOfAccountsCanApply;

}

void DelegatePage::on_searchLineEdit_textChanged(const QString &arg1)
{
    on_searchBtn_clicked();
}

void DelegatePage::refresh()
{
    qDebug() << "DelegatePage refresh";
    updateDelegateList();
    updateFavoriteDelegateList();
    updateIncomeLabel();
    showDelegates(1);
}

void DelegatePage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(229,229,229));
    painter.setPen(QColor(177,183,188));
    painter.drawRect(QRect(-1,-1,828,90));

    painter.setPen(QPen(QColor(177,183,188),Qt::SolidLine));
    painter.drawLine( QPoint(31,150), QPoint( 797,150));
}


void DelegatePage::on_pageLineEdit_editingFinished()
{
    currentPageIndex = ui->pageLineEdit->text().toInt();
    showDelegates(currentPageIndex * 10 - 9);
}


void DelegatePage::on_pageLineEdit_textEdited(const QString &arg1)
{
    if( arg1.at(0) == '+')
    {
        ui->pageLineEdit->setText( arg1.mid(1));
        return;
    }

    if( arg1.at(0) == '0')
    {
        ui->pageLineEdit->setText( arg1.mid(1));
        return;
    }

    if( arg1.toInt() > (searchList.size() - 1)/10 + 1)
    {
        ui->pageLineEdit->setText( QString::number( (searchList.size() - 1)/10 + 1));
        return;
    }
}

void DelegatePage::on_infoBtn1_clicked()
{
    ui->tipLabel1->show();
}

void DelegatePage::on_infoBtn2_clicked()
{
    ui->tipLabel2->show();
}

void DelegatePage::on_totalBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->searchLineEdit->show();
    ui->searchBtn->show();

    ui->totalBtn->setStyleSheet(DELEGATEPAGE_BUTTON_SELECTED);
    ui->favoriteBtn->setStyleSheet(DELEGATEPAGE_BUTTON_UNSELECTED);

}

void DelegatePage::on_favoriteBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->searchLineEdit->hide();
    ui->searchBtn->hide();

    ui->totalBtn->setStyleSheet(DELEGATEPAGE_BUTTON_UNSELECTED);
    ui->favoriteBtn->setStyleSheet(DELEGATEPAGE_BUTTON_SELECTED);
}

void DelegatePage::jsonDataUpdated(QString id)
{
    if( id == "id_blockchain_get_info")
    {
        QString result = Fry::getInstance()->jsonDataValue( id);

        int pos = result.indexOf( "\"max_delegate_pay_issued_per_block\":") + 36;
        double payIssued = result.mid( pos, result.indexOf( ",", pos) - pos).toDouble() / 100000;

        ui->incomeLabel->setText("<body><font style=\"font-size:26px\" color=#47b29c>" + doubleTo2Decimals( payIssued) + "</font><font style=\"font-size:12px\" color=#47b29c>GOP</font><font style=\"font-size:12px\" color=#282828>/" + tr("Block") + "</font></body>");
        ui->incomeLabel->adjustSize();
        return;
    }

    if( id == "id_wallet_get_all_approved_accounts")
    {
        QString result = Fry::getInstance()->jsonDataValue( id);

        if( result == "\"result\":[]" || result.isEmpty())  // 如果没有支持的代理或还未收到返回
        {
            approveList.clear();
        }
        else
        {
            approveList = result.split("},{\"id\":");
        }

        
//        mutexForDelegateList.unlock();

        showFavoriteDelegates(currentPageIndex2 * 10 - 9);
        showDelegates(currentPageIndex * 10 - 9);

        return;
    }

    if( id == "id_approve")
    {
        updateFavoriteDelegateList();
        return;
    }

}

void DelegatePage::delegateListUpdated()
{
    // 如果在搜索 不刷新
    if( !ui->searchLineEdit->text().isEmpty())  return;

//    searchList = Fry::getInstance()->delegateList;
    searchList.clear();

    QStringList tempDelegateList = Fry::getInstance()->delegateList;

    // 本地代理账户放在前面
    foreach (QString delegateName, Fry::getInstance()->delegateAccountList)
    {
        QStringList singleList = tempDelegateList.filter( "\"name\":\"" + delegateName + "\"");
        if( singleList.size() > 0)
        {
            tempDelegateList.removeAll(singleList.at(0));
            searchList += singleList.at(0);
        }
    }

    stringListSort(searchList);

    QStringList favoriteDelegatesList;

    int size = approveList.size();
    for (int i = 0; i < size; i++)
    {
        int pos2 = approveList.at(i).indexOf(",\"name\":\"") + 9;
        QString approveName = approveList.at(i).mid( pos2, approveList.at(i).indexOf("\"", pos2) - pos2);
        favoriteDelegatesList += approveName;
    }

    // 支持的代理账户放在本地账户后面
    foreach (QString favoriteDelegateName, favoriteDelegatesList)
    {
        QStringList singleList = tempDelegateList.filter( "\"name\":\"" + favoriteDelegateName + "\"");
        if( singleList.size() > 0)
        {
            tempDelegateList.removeAll(singleList.at(0));
            searchList += singleList.at(0);
        }
    }
    searchList += tempDelegateList;

    showDelegates( currentPageIndex * 10 - 9);
    ui->pageLabel->setText("/" + QString::number((searchList.size() - 1)/10 + 1));

    updateFavoriteDelegateList();

    ui->loadingWidget->hide();
    return;

}

// 代理stringlist 按排名排序
void DelegatePage::stringListSort(QStringList &stringList)
{
    QMap<int,QString> indexStringMap;
    foreach (QString str, stringList)
    {
        int pos  = str.indexOf("\"index\":") + 8;
        if( pos == -7)  return;
        int rank = str.mid(pos).toInt();

        indexStringMap.insert(rank,str);
    }

    stringList.clear();
    for( QMap<int,QString>::const_iterator it = indexStringMap.constBegin();
         it != indexStringMap.constEnd(); it++)
    {
        stringList += it.value();
    }

}

void DelegatePage::on_prePageBtn2_clicked()
{
    ui->favoriteDelegateTableWidget->scrollToTop();
    if( currentPageIndex2 == 1) return;
    currentPageIndex2--;
    showFavoriteDelegates(currentPageIndex2 * 10 - 9);
    ui->pageLabel2->setText("/" + QString::number((approveList.size() - 1)/10 + 1));
    ui->pageLineEdit2->setText( QString::number(currentPageIndex2));

}



void DelegatePage::on_nextPageBtn2_clicked()
{
    if( currentPageIndex2 >=  ((approveList.size() - 1)/10 + 1))  return;

    currentPageIndex2++;
    showFavoriteDelegates(currentPageIndex2 * 10 - 9);
    ui->pageLabel2->setText("/" + QString::number((approveList.size() - 1)/10 + 1));
    ui->pageLineEdit2->setText( QString::number(currentPageIndex2));

    ui->favoriteDelegateTableWidget->scrollToTop();
}

void DelegatePage::on_pageLineEdit2_editingFinished()
{
    currentPageIndex2 = ui->pageLineEdit2->text().toInt();
    showFavoriteDelegates(currentPageIndex2 * 10 - 9);
}

void DelegatePage::on_pageLineEdit2_textEdited(const QString &arg1)
{
    if( arg1.at(0) == '+')
    {
        ui->pageLineEdit2->setText( arg1.mid(1));
        return;
    }

    if( arg1.at(0) == '0')
    {
        ui->pageLineEdit2->setText( arg1.mid(1));
        return;
    }

    if( arg1.toInt() > (approveList.size() - 1)/10 + 1)
    {
        ui->pageLineEdit2->setText( QString::number( (approveList.size() - 1)/10 + 1));
        return;
    }
}
