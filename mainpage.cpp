#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QScrollBar>
#include<QMessageBox>
#include "mainpage.h"
#include "ui_mainpage.h"
#include "tic.h"
#include "debug_log.h"
#include "namedialog.h"
#include "deleteaccountdialog.h"
#include "rpcthread.h"
#include "accountcellwidget.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "commondialog.h"
#include "showcontentdialog.h"
#include "incomecellwidget.h"
#include "control/rightclickmenudialog.h"
#include "control/chooseaddaccountdialog.h"
#include "dialog/renamedialog.h"
#include "control/accountdetailwidget.h"


MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    hasDelegateOrNot(false),
    refreshOrNot(true),
    currentAccountIndex(-1),
    _totalAcsBalance(0),
    _total3dcBalance(0),
    _totalNbjBalance(0),
    _totalNotBalance(0),
    _first_login(false),
    ui(new Ui::MainPage)
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    ui->setupUi(this);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(22,24,28));
    setPalette(palette);

    ui->TvRaBtn->setChecked(true);

    ui->accountTableWidget->installEventFilter(this);
    ui->accountTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTableWidget->setFocusPolicy(Qt::NoFocus);

    ui->accountTableWidget->setMouseTracking(true);
    ui->accountTableWidget->setShowGrid(false);
    previousColorRow = 0;
    ui->accountTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->accountTableWidget->horizontalHeader()->setFixedHeight(26);
    ui->accountTableWidget->horizontalHeader()->setVisible(true);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->accountTableWidget->setColumnWidth(0,142);
    ui->accountTableWidget->setColumnWidth(1,413);
    ui->accountTableWidget->setColumnWidth(2,144);

    QString language = Fry::getInstance()->language;
    if( language.isEmpty())
    {
        retranslator("Simplified Chinese");
    }
    else
    {
        retranslator(language);
    }
    // 由于首页是第一个页面，第一次打开先等待x秒钟 再 updateAccountList
    QTimer::singleShot(500, this, SLOT(refresh()));
    ui->accountTableWidget->hide();
    ui->loadingWidget->setGeometry(0,93,827,448);
    ui->loadingLabel->move(314,101);
    ui->initLabel->hide();

    DLOG_QT_WALLET_FUNCTION_END;
}

MainPage::~MainPage()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;
  delete ui;
	DLOG_QT_WALLET_FUNCTION_END;
}

QString toThousandFigure( int);

void MainPage::importAccount()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ImportDialog importDialog;
    connect(&importDialog,SIGNAL(accountImported()),this,SLOT(refresh()));
    importDialog.pop();
    emit refreshAccountInfo();
    DLOG_QT_WALLET_FUNCTION_END;
}

void MainPage::addAccount()
{
    NameDialog nameDialog;
    QString name = nameDialog.pop();

    if (!name.isEmpty())
    {
        mutexForAddressMap.lock();
        int size = Fry::getInstance()->addressMap.size();
        mutexForAddressMap.unlock();

        emit showShadowWidget();
        Fry::getInstance()->write("wallet_account_create " + name + '\n');
        QString result = Fry::getInstance()->read();
        emit hideShadowWidget();

        if(result.left(5) == "20017") {
            qDebug() << "wallet_account_create " + name + '\n'  << result;
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Failed"));
            commonDialog.pop();
            return;
        } else {
            if(result.mid(0,2) == ASSET_NAME) { // 创建账号成功
                mutexForConfigFile.lock();
                Fry::getInstance()->configFile->setValue( QString("/accountInfo/") + QString::fromLocal8Bit("账户") + toThousandFigure(size+1),name);
                mutexForConfigFile.unlock();
                Fry::getInstance()->balanceMapInsert( name, "0.00 TV");
                Fry::getInstance()->registerMapInsert( name, "NO");
                Fry::getInstance()->addressMapInsert( name, Fry::getInstance()->getAddress(name));
            }
        }
        emit newAccount(name);
    }
}
// NBJ account list
void MainPage::updateNbjAccountList(){
    _totalNbjBalance = 0;
    ui->accountTableWidget->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, QVariant(QString::fromLocal8Bit("余额/NBJ").toStdString().c_str()));
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();

    // 如果还没有账户
    if (size == 0)
    {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();
        Fry::getInstance()->configFile->endGroup();
        mutexForConfigFile.unlock();
        return;
    }
    else
    {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }

    ui->accountTableWidget->setRowCount(size);
    for (int i = size - 1; i > -1; i--) {
        QString accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
        int rowNum = i;
        ui->accountTableWidget->setRowHeight(rowNum, 26);
        ui->accountTableWidget->setItem(rowNum, 0, new QTableWidgetItem(accountName));
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        ui->accountTableWidget->setItem(rowNum, 1, new QTableWidgetItem(addr));

        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName, "call_contract_offline", QStringList() << "CONMnbfb1uEhPMT11UjLHZjB7WRmNTB6dvTJ" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");

        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double item_ban = ban.toDouble() * 0.00001;
        ui->accountTableWidget->setItem(rowNum, 2, new QTableWidgetItem(doubleTo2Decimals(item_ban)));
        _totalNbjBalance += item_ban;

        ui->accountTableWidget->item(rowNum, 0)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 1)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextColor(Qt::red);

        if (rowNum == currentAccountIndex) {
            for( int i = 0; i < 3; i++) {
                ui->accountTableWidget->item(currentAccountIndex, i)->setBackgroundColor(QColor(245, 248, 248, 150));
            }
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    ui->loadingWidget->hide();
}

// NOT account list
void MainPage::updateNotAccountList() {
    _totalNotBalance = 0;
    ui->accountTableWidget->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, QVariant(QString::fromLocal8Bit("余额/NOT").toStdString().c_str()));
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();

    // 如果还没有账户
    if (size == 0)
    {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();
        Fry::getInstance()->configFile->endGroup();
        mutexForConfigFile.unlock();
        return;
    }
    else
    {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }

    ui->accountTableWidget->setRowCount(size);
    for (int i = size - 1; i > -1; i--) {
        QString accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
        int rowNum = i;
        ui->accountTableWidget->setRowHeight(rowNum, 26);
        ui->accountTableWidget->setItem(rowNum, 0, new QTableWidgetItem(accountName));
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        ui->accountTableWidget->setItem(rowNum, 1, new QTableWidgetItem(addr));

        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName, "call_contract_offline", QStringList() << "CONJD5RCLyYYnbwV3hxhneDBR8FWaL9JqcQw" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");

        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double item_ban = ban.toDouble() * 0.0001;
        ui->accountTableWidget->setItem(rowNum, 2, new QTableWidgetItem(doubleTo2Decimals(item_ban)));
        _totalNotBalance += item_ban;

        ui->accountTableWidget->item(rowNum, 0)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 1)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextColor(Qt::red);

        if (rowNum == currentAccountIndex) {
            for( int i = 0; i < 3; i++) {
                ui->accountTableWidget->item(currentAccountIndex, i)->setBackgroundColor(QColor(245, 248, 248, 150));
            }
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    ui->loadingWidget->hide();
}

//ACS account list
void MainPage::updateAcsAccountList() {
    _totalAcsBalance = 0;
    ui->accountTableWidget->horizontalHeader()->model()->setHeaderData(2,Qt::Horizontal, QVariant(QString::fromLocal8Bit("余额/ACS").toStdString().c_str()));
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();
    // 如果还没有账户
    if (size == 0) {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();
        Fry::getInstance()->configFile->endGroup();
        mutexForConfigFile.unlock();
        return;
    } else {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }

    ui->accountTableWidget->setRowCount(size);
    for (int i = size - 1; i > -1; i--) {
        QString accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
        int rowNum = i;
        ui->accountTableWidget->setRowHeight(rowNum, 26);
        ui->accountTableWidget->setItem(rowNum, 0, new QTableWidgetItem(accountName));
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        ui->accountTableWidget->setItem(rowNum, 1, new QTableWidgetItem(addr));

        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName, "call_contract_offline", QStringList() << "CONJiK5jcsBhHVDZWjGTtMek2q9h42JnQtvj" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");

        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double item_ban = ban.toDouble() * 0.00001;
        ui->accountTableWidget->setItem(rowNum, 2, new QTableWidgetItem(doubleTo2Decimals(item_ban)));
        _totalAcsBalance += item_ban;

        ui->accountTableWidget->item(rowNum, 0)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 1)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextColor(Qt::red);

        if (rowNum == currentAccountIndex) {
            for( int i = 0; i < 3; i++) {
                ui->accountTableWidget->item(currentAccountIndex, i)->setBackgroundColor(QColor(245, 248, 248, 150));
            }
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    ui->loadingWidget->hide();
}

//3dc account list
void MainPage::update3dcAccountList()
{
    _total3dcBalance = 0;
    ui->accountTableWidget->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, QVariant(QString::fromLocal8Bit("余额/3DC").toStdString().c_str()));
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();

    // 如果还没有账户
    if (size == 0)
    {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();
        Fry::getInstance()->configFile->endGroup();
        mutexForConfigFile.unlock();
        return;
    }
    else
    {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }

    ui->accountTableWidget->setRowCount(size);
    for (int i = size - 1; i > -1; i--) {
        QString accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
        int rowNum = i;
        ui->accountTableWidget->setRowHeight(rowNum, 26);
        ui->accountTableWidget->setItem(rowNum, 0, new QTableWidgetItem(accountName));
        QString addr = Fry::getInstance()->addressMapValue(accountName);
        ui->accountTableWidget->setItem(rowNum, 1, new QTableWidgetItem(addr));

        Fry::getInstance()->postRPC(toJsonFormat("id_call_contract_offline_" + accountName, "call_contract_offline", QStringList() << "CONGWG5DwjXC9aFokPMB39tpaWHyzTNLcBWo" << accountName << "balanceOf" << addr << "TV" << "1"));
        QStringList tmp = Fry::getInstance()->jsonDataValue("id_call_contract_offline_" + accountName).split(":");

        QString balance_str = tmp.last();
        QString ban = balance_str.mid(1, balance_str.length() - 2);
        double item_ban = ban.toDouble() * 0.00001;
        ui->accountTableWidget->setItem(rowNum, 2, new QTableWidgetItem(doubleTo2Decimals(item_ban)));
        _total3dcBalance += item_ban;

        ui->accountTableWidget->item(rowNum, 0)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 1)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextColor(Qt::red);

        if (rowNum == currentAccountIndex) {
            for( int i = 0; i < 3; i++) {
                ui->accountTableWidget->item(currentAccountIndex, i)->setBackgroundColor(QColor(245, 248, 248, 150));
            }
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    ui->loadingWidget->hide();
}

//TV account list
void MainPage::updateAccountList()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->accountTableWidget->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, QVariant(QString::fromLocal8Bit("余额/TV").toStdString().c_str()));
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();
    // 如果还没有账户
    if (size == 0) {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();
        Fry::getInstance()->configFile->endGroup();
        mutexForConfigFile.unlock();
        return;
    } else {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }

    ui->accountTableWidget->setRowCount(size);
    for (int i = size - 1; i > -1; i--) {
        QString accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
        int rowNum = i;
        ui->accountTableWidget->setRowHeight(rowNum,26);
        ui->accountTableWidget->setItem(rowNum, 0, new QTableWidgetItem(accountName));
        ui->accountTableWidget->setItem(rowNum, 1, new QTableWidgetItem(Fry::getInstance()->addressMapValue(accountName)));
        ui->accountTableWidget->setItem(rowNum, 2, new QTableWidgetItem(Fry::getInstance()->balanceMapValue(accountName).remove(ASSET_NAME)));
        ui->accountTableWidget->item(rowNum, 0)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 1)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextAlignment(Qt::AlignCenter);
        ui->accountTableWidget->item(rowNum, 2)->setTextColor(Qt::red);
        if (rowNum == currentAccountIndex) {
            for (int i = 0; i < 3; i++) {
                ui->accountTableWidget->item(currentAccountIndex,i)->setBackgroundColor(QColor(245, 248, 248, 150));
            }
        }
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    ui->loadingWidget->hide();
    DLOG_QT_WALLET_FUNCTION_END;
}

void MainPage::on_addAccountBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ChooseAddAccountDialog* chooseAddAccountDialog = new ChooseAddAccountDialog(this);
    chooseAddAccountDialog->move( ui->addAccountBtn->mapToGlobal( QPoint(0,-83) ) );
    connect( chooseAddAccountDialog, SIGNAL(newAccount()), this, SLOT( addAccount()));
    connect( chooseAddAccountDialog, SIGNAL(importAccount()), this, SLOT( importAccount()));
    chooseAddAccountDialog->exec();
	DLOG_QT_WALLET_FUNCTION_END;
}

void MainPage::on_accountTableWidget_cellClicked(int row, int column)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    emit openAccountPage( ui->accountTableWidget->item(row,0)->text());
    DLOG_QT_WALLET_FUNCTION_END;
}

void MainPage::on_accountTableWidget_cellEntered(int row, int column)
{
    if( ui->accountTableWidget->rowCount() > 0) {
        for( int i = 0; i < 3; i++) {
            ui->accountTableWidget->item(previousColorRow,i)->setBackgroundColor(QColor(22,24,28));
        }
    }

    for( int i = 0; i < 3; i++) {
        ui->accountTableWidget->item(row,i)->setBackgroundColor(QColor(37,39,43));
    }
    previousColorRow = row;
}

int tableWidgetPosToRow(QPoint pos, QTableWidget* table);


void MainPage::refresh()
{
    qDebug() << "mainpage refresh" << refreshOrNot;
    if (!refreshOrNot) return;
    if (ui->TvRaBtn->isChecked())
    {
        updateAccountList();
        updateTotalBalance();
    }
    else if (ui->TdcRaBtn->isChecked())
    {
        update3dcAccountList();
        update3dcTotalBalance();
    }
    else if (ui->AcsRaBtn->isChecked())
    {
        updateAcsAccountList();
        updateAcsTotalBalance();
    }
    else if (ui->NbjRaBtn->isChecked()) {
        updateNbjAccountList();
        updateNbjTotalBalance();
    }
    else if (ui->NotRaBtn->isChecked()) {
        updateNotAccountList();
        updateNotTotalBalance();
    }
}

void MainPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(22 ,24, 28), Qt::SolidLine));
    painter.setBrush(QBrush(QColor(22, 24, 28), Qt::SolidPattern));
    painter.drawRect(-1, -1, 794, 68);

    painter.setPen(QPen(QColor(32, 34, 38), Qt::SolidLine));
    painter.drawLine(QPoint(45, 100), QPoint(793, 100));
}

void MainPage::retranslator(QString language)
{
    ui->retranslateUi(this);
    if( language == "Simplified Chinese") {
    } else if( language == "English") {
    }
}

void MainPage::jsonDataUpdated(QString id)
{
    if (id.mid(0, 37) == "id_wallet_delegate_pay_balance_query_") {
        QString result = Fry::getInstance()->jsonDataValue(id);
        int pos = result.indexOf("\"pay_balance\":") + 14;
        QString payBal = result.mid( pos, result.indexOf("}", pos) - pos );
        payBal.remove("\"");
        Fry::getInstance()->delegateSalaryMap.insert(id.mid(37), payBal.toInt() / 100000.0);
        return;
    }


    if (id == "id_wallet_delegate_withdraw_pay") {
        QString result = Fry::getInstance()->jsonDataValue(id);
        if (result.mid(0, 9) == "\"result\":") {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Withdraw succeeded!") );
            commonDialog.pop();
            refresh();
        } else if (result.mid(0, 8) == "\"error\":") {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Withdraw failed!") );
            commonDialog.pop();
        }
        return;
    }
}
// total NOT
void MainPage::updateNotTotalBalance() {
    ui->totalBalanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(_totalNotBalance) + "</font><font style=\"font-size:12px\" color=#a9a9a9> NOT</font></body>");
    ui->totalBalanceLabel->adjustSize();
}

// total NBJ
void MainPage::updateNbjTotalBalance() {
    ui->totalBalanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(_totalNbjBalance) + "</font><font style=\"font-size:12px\" color=#a9a9a9> NBJ</font></body>");
    ui->totalBalanceLabel->adjustSize();
}

//total ACS
void MainPage::updateAcsTotalBalance()
{
    ui->totalBalanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(_totalAcsBalance) + "</font><font style=\"font-size:12px\" color=#a9a9a9> ACS</font></body>");
    ui->totalBalanceLabel->adjustSize();
}

//total 3DC
void MainPage::update3dcTotalBalance()
{
    ui->totalBalanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(_total3dcBalance) + "</font><font style=\"font-size:12px\" color=#a9a9a9> 3DC</font></body>");
    ui->totalBalanceLabel->adjustSize();
}

//total TV
void MainPage::updateTotalBalance()
{
    QString  result = Fry::getInstance()->jsonDataValue("id_balance");
    double totalBalance = 0;
    int pos = result.indexOf("[[0,") + 4;
    while (pos != -1 + 4) {
        QString amount = result.mid(pos, result.indexOf("]", pos) - pos);
        amount.remove("\""); // 太大的数字可能会用字符串表示，加了引号
        totalBalance += amount.toDouble() / 100000.0;
        pos = result.indexOf("[[0,", pos) + 4;
    }
    ui->totalBalanceLabel->setText("<body><font style=\"font-size:26px\" color=#a9a9a9>" + doubleTo2Decimals(totalBalance) + "</font><font style=\"font-size:12px\" color=#a9a9a9> TV</font></body>");
    ui->totalBalanceLabel->adjustSize();
}

void MainPage::updatePending()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    mutexForPending.lock();

    if( !Fry::getInstance()->pendingFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "pending.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( Fry::getInstance()->pendingFile->readAll());
    QString str(ba);
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForAddressMap.lock();
    QStringList keys = Fry::getInstance()->addressMap.keys();
    mutexForAddressMap.unlock();

    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("recordId");
    QStringList recordKeys = Fry::getInstance()->configFile->childKeys();
    Fry::getInstance()->configFile->endGroup();

    foreach (QString ss, strList) {
        QStringList sList = ss.split(",");

        if (!keys.contains(sList.at(1)) && keys.size() > 0)   // 如果账号被删除了， 删掉pending 中的记录   keys.size() 防止刚启动 addressmap为空
        {
            strList.removeAll(ss);
            continue;
        }
        // 如果config中recordId会被置为1， 则删除该记录
        if (Fry::getInstance()->configFile->value("recordId/" + sList.at(0)).toInt() != 0 ) {
            strList.removeAll(ss);
            continue;
        }

        // 如果config中recordId被删除了， 则删除该记录
        if (!Fry::getInstance()->configFile->contains("recordId/" + sList.at(0))) {
            strList.removeAll(ss);
            continue;
        }
    }
    mutexForConfigFile.unlock();

    ba.clear();
    foreach (QString ss, strList)
    {
        ba += QString(ss + ";").toUtf8();
    }
    ba = ba.toBase64();
    Fry::getInstance()->pendingFile->resize(0);
    QTextStream ts(Fry::getInstance()->pendingFile);
    ts << ba;
    Fry::getInstance()->pendingFile->close();

    mutexForPending.unlock();
    DLOG_QT_WALLET_FUNCTION_END;
}

//  tablewidget 从 pos 获取 item（每行第0个）
int tableWidgetPosToRow(QPoint pos, QTableWidget* table)
{
    int headerHeight = 26;
    int rowHeight = 26;

    // 获得当前滚动条的位置
    int scrollBarValue = table->verticalScrollBar()->sliderPosition();

    if( pos.y() < headerHeight || pos.y() > table->height())
    {
        return -1;
    }
    else
    {
        int rowCount = floor( (pos.y() - headerHeight) / rowHeight) + scrollBarValue;
        return rowCount;
    }
}

bool MainPage::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->accountTableWidget)
    {
        if( e->type() == QEvent::ContextMenu)
        {
            QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(e);
            int row = tableWidgetPosToRow(contextMenuEvent->pos(), ui->accountTableWidget);
            if( row == -1) return false;

            QString name = ui->accountTableWidget->item(row,0)->text();
            RightClickMenuDialog* rightClickMenuDialog = new RightClickMenuDialog(name, this);
            rightClickMenuDialog->move(ui->accountTableWidget->mapToGlobal(contextMenuEvent->pos()));
            connect(rightClickMenuDialog, SIGNAL(transferFromAccount(QString)), this, SIGNAL(showTransferPage(QString)));
            connect(rightClickMenuDialog, SIGNAL(renameAccount(QString)), this, SLOT(renameAccount(QString)));
            connect(rightClickMenuDialog, SIGNAL(exportAccount(QString)), this, SLOT(showExportDialog(QString)));
            rightClickMenuDialog->exec();

            return true;
        }
    }
    return QWidget::eventFilter(watched,e);
}

void MainPage::showExportDialog(QString name)
{
    ExportDialog exportDialog(name);
    exportDialog.pop();
}

void MainPage::stopRefresh()
{
    refreshOrNot = false;
}

void MainPage::startRefresh()
{
    refreshOrNot = true;
}

void MainPage::withdrawSalary(QString name, QString salary)
{
}

void MainPage::renameAccount(QString name)
{
    RenameDialog renameDialog;
    QString newName = renameDialog.pop();

    if( !newName.isEmpty() && newName != name)
    {
        Fry::getInstance()->write("wallet_account_rename " + name + " " + newName + '\n');
        QString result = Fry::getInstance()->read();
        qDebug() << result;
        if (result.mid(0, 2) == "OK")
        {
            mutexForConfigFile.lock();
            Fry::getInstance()->configFile->beginGroup("/accountInfo");
            QStringList keys = Fry::getInstance()->configFile->childKeys();
            foreach (QString key, keys)
            {
                if( Fry::getInstance()->configFile->value(key) == name)
                {
                    Fry::getInstance()->configFile->setValue(key, newName);
                    break;
                }
            }
            Fry::getInstance()->configFile->endGroup();
            mutexForConfigFile.unlock();
            Fry::getInstance()->balanceMapInsert( newName, Fry::getInstance()->balanceMapValue(name));
            Fry::getInstance()->balanceMapRemove(name);
            Fry::getInstance()->registerMapInsert( newName, Fry::getInstance()->registerMapValue(name));
            Fry::getInstance()->registerMapRemove(name);
            Fry::getInstance()->addressMapInsert( newName, Fry::getInstance()->addressMapValue(name));
            Fry::getInstance()->addressMapRemove(name);
            emit newAccount(newName);
        }
        else
        {
            return;
        }
    }
}

void MainPage::deleteAccount(QString name)
{
    DeleteAccountDialog deleteACcountDialog( name);
    if (deleteACcountDialog.pop())
    {
        previousColorRow = 0;
        currentAccountIndex = -1;
        refresh();
    }
}

void MainPage::onTvRaBtnChecked() {
    refresh();
}

void MainPage::onAcsRaBtnChecked() {
    refresh();
}

void MainPage::on3dcRaBtnChecked() {
    refresh();
}

void MainPage::onNbjRaBtnChecked(){
    refresh();
}
void MainPage::onNotRaBtnChecked() {
    refresh();
}

