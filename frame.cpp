#include <QPainter>
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <QMenu>
#include <QApplication>

#include "firstlogin.h"
#include "normallogin.h"
#include "mainpage.h"
#include "accountpage.h"
#include "transferpage.h"
#include "bottombar.h"
#include "lockpage.h"
#include "titlebar.h"
#include "frame.h"
#include "tic.h"
#include "debug_log.h"
#include "waitingforsync.h"
#include "functionbar.h"
#include "contactpage.h"
#include "upgradepage.h"
#include "selectgoppathwidget.h"
#include "rpcthread.h"
#include "control/shadowwidget.h"
#include "control/showbottombarwidget.h"
#include "uploadpage.h"
#include "uploadbar.h"
#include "storagefilepage.h"
#include "storagefilebar.h"
#include "downloadfilepage.h"
#include "downloadbar.h"

Frame::Frame(): timer(NULL),
    firstLogin(NULL),
    normalLogin(NULL),
    mainPage(NULL),
    accountPage(NULL),
    transferPage(NULL),
    bottomBar(NULL),
    centralWidget(NULL),
    lockPage(NULL),
    titleBar(NULL),
    contactPage(NULL),
    upgradePage(NULL),
    functionBar(NULL),
    shadowWidget(NULL),
    timerForAutoRefresh(NULL),
    waitingForSync(NULL),
    selectGopPathWidget(NULL),
    needToRefresh(false),
    uploadFilePage(NULL),
    uploadPageBar(NULL),
    storagePage(NULL),
    storageBar(NULL),
    downloadFilePage(NULL),
    downloadBar(NULL)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    setWindowFlags(Qt::Window|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);

    ::SystemParametersInfo(SPI_GETWORKAREA , 0 , &this->rtConfined , 0);
    ::GetWindowRect(::GetDesktopWindow() , &this->rtDefault);

    setFrameShape(QFrame::NoFrame);
//    setMouseTracking(true);

//    setStyleSheet("Frame{background-color:white; border: 4px solid #CCCCCC;border-radius:5px;}"
//                  "QScrollBar:vertical{width:8px;background:transparent;margin:0px,0px,0px,0px;padding-top:2px;padding-bottom:3px;}"
//                  "QScrollBar::handle:vertical{width:8px;background:rgba(130,137,143,20%);border-radius:4px;min-height:20;}"
//                  "QScrollBar::handle:vertical:hover{width:8px;background:rgba(130,137,143,100%);border-radius:4px;min-height:20;}"
//                  "QScrollBar::add-line:vertical{height:9px;width:8px;border-image:url(:/images/a/3.png);subcontrol-position:bottom;}"
//                  "QScrollBar::sub-line:vertical{height:9px;width:8px;border-image:url(:/images/a/1.png);subcontrol-position:top;}"
//                  "QScrollBar::add-line:vertical:hover{height:9px;width:8px;border-image:url(:/images/a/4.png);subcontrol-position:bottom;}"
//                  "QScrollBar::sub-line:vertical:hover{height:9px;width:8px;border-image:url(:/images/a/2.png);subcontrol-position:top;}"
//                  "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,0%);border-radius:4px;}"
//                  );

    mouse_press = false;
    currentPageNum = 0;
    lastPage = "";
    currentAccount = "";


    connect(Fry::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QString language = Fry::getInstance()->language;
    if( language.isEmpty())
    {
        setLanguage("Simplified Chinese");
    }
    else
    {
        setLanguage(language);
    }

    setGeometry(240,110,960,580);
    shadowWidget = new ShadowWidget(this);
    shadowWidget->init(this->size());

    //  如果是第一次使用(未设置 blockchain 路径)
    mutexForConfigFile.lock();

    if( !Fry::getInstance()->configFile->contains("/settings/fryPath") )
    {
        selectGopPathWidget = new SelectGopPathWidget(this);
        selectGopPathWidget->setAttribute(Qt::WA_DeleteOnClose);
        selectGopPathWidget->move(0,0);
        connect( selectGopPathWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
        connect( selectGopPathWidget,SIGNAL(minimum()),this,SLOT(showMinimized()));
        connect( selectGopPathWidget,SIGNAL(closeGOP()),this,SLOT(stop()));
        connect( selectGopPathWidget,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
        connect( selectGopPathWidget,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

        selectGopPathWidget->show();

    }
    else
    {
        QString path = Fry::getInstance()->configFile->value("/settings/fryPath").toString();
        QDir dir(path);
        QDir dir2(path + "/wallets");
        QDir dir3(path + "/chain");

        if( !dir.exists() || ( !dir2.exists() && !dir3.exists())  )  // 如果数据文件被删除了
        {
            selectGopPathWidget = new SelectGopPathWidget(this);
            selectGopPathWidget->setAttribute(Qt::WA_DeleteOnClose);
            selectGopPathWidget->move(0,0);
            connect( selectGopPathWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
            connect( selectGopPathWidget,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( selectGopPathWidget,SIGNAL(closeGOP()),this,SLOT(stop()));
            connect( selectGopPathWidget,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( selectGopPathWidget,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

            selectGopPathWidget->show();

            Fry::getInstance()->configFile->clear();
            Fry::getInstance()->configFile->setValue("/settings/lockMinutes",5);
            Fry::getInstance()->lockMinutes     = 5;
            Fry::getInstance()->configFile->setValue("/settings/notAutoLock",false);
            Fry::getInstance()->notProduce      =  true;
            Fry::getInstance()->configFile->setValue("/settings/language","Simplified Chinese");
            Fry::getInstance()->language = "Simplified Chinese";
            Fry::getInstance()->minimizeToTray  = false;
            Fry::getInstance()->configFile->setValue("/settings/minimizeToTray",false);
            Fry::getInstance()->closeToMinimize = false;
            Fry::getInstance()->configFile->setValue("/settings/closeToMinimize",false);

        }
        else
        {
            waitingForSync = new WaitingForSync(this);
            waitingForSync->setAttribute(Qt::WA_DeleteOnClose);
            waitingForSync->move(0,0);
            connect( waitingForSync,SIGNAL(sync()), this, SLOT(syncFinished()));
            connect( waitingForSync,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( waitingForSync,SIGNAL(tray()),this,SLOT(hide()));
            connect( waitingForSync,SIGNAL(closeGOP()),this,SLOT(stop()));
            connect( waitingForSync,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( waitingForSync,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
            waitingForSync->show();
            waitingForSync->getUpdateXml();

            Fry::getInstance()->appDataPath = Fry::getInstance()->configFile->value("/settings/fryPath").toString();

            QStringList strList;
            strList << "--data-dir" << Fry::getInstance()->configFile->value("/settings/fryPath").toString()
                    << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT) << "--server";
            Fry::getInstance()->proc->start("Ti_Value.exe",strList );

            if( Fry::getInstance()->proc->waitForStarted())
            {
                qDebug() << "laungh Ti_Value.exe succeeded";
//         qDebug()  <<     Fry::getInstance()->read();
            }
            else
            {
                qDebug() << "laungh Ti_Value.exe failed";
            }
        }
    }
    ipfsprocess = new QProcess();
    ipfsprocess->start("ipfs.exe", QStringList()<<"daemon"<<"--init");
    if(ipfsprocess->waitForStarted())
    {
        qDebug() << "laungh ipfs.exe succeeded";
    }
    mutexForConfigFile.unlock();
    trayIcon = new QSystemTrayIcon(this);
    //放在托盘提示信息、托盘图标
    trayIcon ->setToolTip(QString("TVWallet ") + TICWALLET_VERSION);
    trayIcon ->setIcon(QIcon(":/pic/ticpic/tic.ico"));
    //点击托盘执行的事件
    connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
    createTrayIconActions();
    createTrayIcon();






    DLOG_QT_WALLET_FUNCTION_END;
}

Frame::~Frame()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    qDebug() << "~Frame begin;";

    if(centralWidget)
    {
        delete centralWidget;
        centralWidget = NULL;
    }

	if (titleBar)
	{
		delete titleBar;
		titleBar = NULL;
	}

    if (timer)
	{
		delete timer;
		timer = NULL;
	}

	if (bottomBar)
	{
		delete bottomBar;
		bottomBar = NULL;
	}

    if (uploadPageBar)
    {
        delete uploadPageBar;
        uploadPageBar = NULL;
    }

    if (lockPage)
    {
        delete lockPage;
        lockPage = NULL;
    }

    if( waitingForSync)
    {
        delete waitingForSync;
        waitingForSync = NULL;
    }

    if( functionBar)
    {
        delete functionBar;
        functionBar = NULL;
    }
    if(ipfsprocess)
    {
        ipfsprocess->execute("taskkill /im ipfs.exe /f");
        ipfsprocess->close();
        delete ipfsprocess;
        ipfsprocess = NULL;
    }
qDebug() << "~Frame end;";
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::alreadyLogin()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    titleBar = new TitleBar(this);
    titleBar->setGeometry(0,0,960,50);
    connect(titleBar,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect(titleBar,SIGNAL(closeGOP()),this,SLOT(stop()));
    connect(titleBar,SIGNAL(tray()),this,SLOT(hide()));
    connect(titleBar,SIGNAL(settingSaved()),this,SLOT(settingSaved()));
    connect(titleBar,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(titleBar,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(titleBar,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(titleBar,SIGNAL(lock()),this,SLOT(showLockPage()));


    titleBar->show();

    centralWidget = new QWidget(this);
    centralWidget->setGeometry(167,50,793,532);
    centralWidget->show();

    bottomBar = new BottomBar(this);
    bottomBar->move(167,540);
    bottomBar->show();

    uploadPageBar = new UploadBar(this);
    uploadPageBar->move(167,540);

    storageBar = new StorageFileBar(this);
    storageBar->move(167,540);


    downloadBar = new DownloadBar(this);
    downloadBar->move(167,540);

//    showBottomBarWidget = new ShowBottomBarWidget(centralWidget);
//    showBottomBarWidget->setGeometry(0,525,827,20);
//    connect(showBottomBarWidget, SIGNAL(showBottomBar()), bottomBar, SLOT(dynamicShow()) );
//    showBottomBarWidget->show();

    functionBar = new FunctionBar(this);
    functionBar->move(0,50);
    functionBar->show();
    connect(functionBar, SIGNAL(showMainPage()), this, SLOT( showMainPage()));
    connect(functionBar, SIGNAL(showAccountPage(QString)), this, SLOT( showAccountPage(QString)));
    connect(functionBar, SIGNAL(showTransferPage()), this, SLOT( showTransferPage()));
    connect(functionBar, SIGNAL(showContactPage()), this, SLOT( showContactPage()));
    connect(functionBar,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(functionBar,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(functionBar, SIGNAL(showUploadPage()), this, SLOT( showUploadPage()));
    connect(functionBar, SIGNAL(showStorageFilePage()), this, SLOT( showStoragePage()));
    connect(functionBar, SIGNAL(showDownloadFilePage()), this, SLOT( showDownloadPage()));

    /***uploadsuccess****/
    connect(uploadPageBar, SIGNAL(uploadSuccess(QString, QString)), this, SLOT(uploadSuccess(QString, QString)));
    connect(storageBar, SIGNAL(applayStorage()), this, SLOT(applayStorage()));
    connect(downloadBar, SIGNAL(downloadFile()), this, SLOT(downloadFile()));

    /**************/

    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(openAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(mainPage,SIGNAL(showUpgradePage(QString)),this,SLOT(showUpgradePage(QString)));
    connect(mainPage,SIGNAL(showTransferPage(QString)),this,SLOT(showTransferPage(QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));

    timer = new QTimer(this);               //  自动锁定
    connect(timer,SIGNAL(timeout()),this,SLOT(autoLock()));
    if(Fry::getInstance()->notProduce)
    {
        timer->start(Fry::getInstance()->lockMinutes * 60000);
    }

    startTimerForAutoRefresh();              // 自动刷新

    init();

    DLOG_QT_WALLET_FUNCTION_END;
}

QString toThousandFigure( int number)     // 转换为0001,0015  这种数字格式
{
    if( number <= 9999 && number >= 1000)
    {
        return QString::number(number);
    }

    if( number <= 999 && number >= 100)
    {
        return QString( "0" + QString::number(number));
    }

    if( number <= 99 && number >= 10)
    {
        return QString( "00" + QString::number(number));
    }

    if( number <= 9 && number >= 0)
    {
        return QString( "000" + QString::number(number));
    }
    return "99999";
}

void Frame::getAccountInfo()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_list_my_addresses", "wallet_list_my_addresses", QStringList() << ""));

    Fry::getInstance()->postRPC( toJsonFormat( "id_balance", "balance", QStringList() << ""));

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showAccountPage(QString accountName)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    currentAccount = accountName;
    getAccountInfo();

    closeCurrentPage();
    accountPage = new AccountPage(accountName,centralWidget);
    accountPage->setAttribute(Qt::WA_DeleteOnClose);
    accountPage->show();
    currentPageNum = 1;
    connect(accountPage,SIGNAL(back()),this,SLOT(showMainPage()));
    connect(accountPage,SIGNAL(accountChanged(QString)),this,SLOT(showAccountPage(QString)));
    connect(accountPage,SIGNAL(showUpgradePage(QString)),this,SLOT(showUpgradePage(QString)));
    connect(accountPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(accountPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

//    functionBar->choosePage(2);
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showTransferPage(QString accountName)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage(accountName,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    transferPage->show();

    currentPageNum = 3;
    functionBar->choosePage(2);

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showUploadPage()
{
    //toto
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    closeCurrentPage();
    uploadFilePage = new UploadFilePage(centralWidget);
    uploadFilePage->show();
    currentPageNum = 8;
    functionBar->choosePage(4);//4 is uploadfilepage
    bottomBar->hide();
    uploadPageBar->show();
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::uploadSuccess(QString filename, QString filesize)
{
    uploadFilePage->uploadSuccess(filename, filesize);
}

void Frame::applayStorage()
{
    storagePage->applayStorage();
}

void Frame::downloadFile()
{
    downloadFilePage->downloadFile();
}

void Frame::showStoragePage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    closeCurrentPage();
    storagePage = new StorageFilePage(centralWidget);
    storagePage->show();
    currentPageNum = 9;
    functionBar->choosePage(5);//4 is uploadfilepage
    bottomBar->hide();
    uploadPageBar->hide();
    downloadBar->hide();
    storageBar->show();
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showDownloadPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    closeCurrentPage();
    downloadFilePage = new DownloadFilePage(centralWidget);
    downloadFilePage->show();
    currentPageNum = 10;
    functionBar->choosePage(6);//4 is uploadfilepage
    bottomBar->hide();
    uploadPageBar->hide();
    storageBar->hide();
    downloadBar->show();
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showLockPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    timer->stop();
    shadowWidgetShow();


    Fry::getInstance()->postRPC( toJsonFormat( "id_lock", "lock", QStringList() << "" ));
qDebug() << "lock ";
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::autoLock()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    timer->stop();

    Fry::getInstance()->postRPC( toJsonFormat( "id_lock", "lock", QStringList() << "" ));
qDebug() << "autolock ";
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::unlock()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( Fry::getInstance()->notProduce)
    {
        timer->start(Fry::getInstance()->lockMinutes * 60000);
    }
    centralWidget->show();
    bottomBar->show();
    titleBar->show();
    qDebug() << "lockPage " << lockPage;
    if( lockPage)
    {
        lockPage->close();
        lockPage = NULL;
    }

qDebug() << "unlock showCurrentDialog";
    Fry::getInstance()->showCurrentDialog();

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::updateTimer()
{
    if( timer != NULL && lockPage == NULL)
    {
        if( Fry::getInstance()->notProduce)
        {
            timer->stop();
            timer->start(Fry::getInstance()->lockMinutes * 60000);
        }
    }
}

void Frame::settingSaved()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    if( !Fry::getInstance()->notProduce)
    {
        timer->stop();
    }
    else
    {
        timer->start( Fry::getInstance()->lockMinutes * 60000);
    }

    QString language = Fry::getInstance()->language;
    if( language.isEmpty())
    {
        setLanguage("Simplified Chinese");
    }
    else
    {
        setLanguage(language);
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::privateKeyImported()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    getAccountInfo();
    mainPage->updateAccountList();
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::mousePressEvent(QMouseEvent *event)
{
//    if( Fry::getInstance()->notProduce)
//    {
//        updateTimer();
//    }

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }
    ::ClipCursor(&rtConfined);

}

void Frame::mouseMoveEvent(QMouseEvent *event)
{

    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }

}

void Frame::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
    ::ClipCursor(&rtDefault);
}

void Frame::refreshAccountInfo()
{
    needToRefresh = true;
    getAccountInfo();
}

void Frame::startTimerForAutoRefresh()
{
    if( timerForAutoRefresh != NULL)
    {
        timerForAutoRefresh->stop();
        delete timerForAutoRefresh;
    }

    timerForAutoRefresh = new QTimer(this);
    connect(timerForAutoRefresh,SIGNAL(timeout()),this,SLOT(autoRefresh()));
    timerForAutoRefresh->start(AUTO_REFRESH_TIME);
}

void Frame::syncFinished()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    waitingForSync->timer->stop();

    RpcThread* rpcThread = new RpcThread;
    connect(rpcThread, SIGNAL(finished()), rpcThread, SLOT(deleteLater()));
    rpcThread->setLogin("a", "b");
    rpcThread->setWriteData( toJsonFormat( "id_open", "open", QStringList() << "wallet" ));
    rpcThread->start();

    Fry::getInstance()->initWorkerThreadManager();

    if( Fry::getInstance()->contactsFile == NULL)
    {
        Fry::getInstance()->getContactsFile();
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::closeCurrentPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    qDebug() << " closeCurrentPage :" << currentPageNum;

    switch (currentPageNum) {
    case 0:
        mainPage->close();
        mainPage = NULL;
        break;
    case 1:
        accountPage->close();
        accountPage = NULL;
        break;
    case 2:
        break;
    case 3:
        transferPage->close();
        transferPage = NULL;
        break;
    case 4:
        contactPage->close();
        contactPage = NULL;
        break;
    case 5:

        break;
    case 6:
        break;
    case 7:
        upgradePage->close();
        upgradePage = NULL;
        break;
    case 8:
        uploadFilePage->close();
        uploadFilePage = NULL;
        uploadPageBar->hide();
        bottomBar->show();
        //todo uploadfilePage
        break;
    case 9:
        storagePage->close();
        storagePage = NULL;
        storageBar->hide();
        bottomBar->show();
        //todo storagefilepae
        break;
    case 10:
        downloadFilePage->close();
        downloadFilePage = NULL;
        downloadBar->hide();
        bottomBar->show();
        //todo downfilepage
        break;
    default:
        break;
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::refresh()
{
//    getAccountInfo();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        accountPage->refresh();
        break;
    case 2:

        break;
    case 3:
        transferPage->refresh();
        break;
    case 4:
        break;
    case 5:
//        showAccountPage(currentAccount);

        break;
    case 6:
        break;
    case 7:
//        showUpgradePage();
        break;
    case 8:
        break;
    default:
        break;
    }
}

void Frame::autoRefresh()
{
    getAccountInfo();

    bottomBar->refresh();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        if( lockPage == NULL)     // 锁定的时候 refresh会崩溃
        {
            accountPage->refresh();
        }
        break;
    case 2:
        break;
    case 3:
//        showTransferPage( transferPage->getCurrentAccount());
        transferPage->refresh();
        break;
    case 4:
        break;
    case 5:
//        showAccountPage(currentAccount);
//        if( lockPage == NULL)     // 锁定的时候 refresh会崩溃
//        {
//            accountPage->refresh();
//        }
        break;
    case 6:
        break;
    case 7:
//        showUpgradePage();
        break;
    case 8:
        break;
    default:
        break;
    }
}


void Frame::showMainPage()
{
    closeCurrentPage();
    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(openAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(mainPage,SIGNAL(showUpgradePage(QString)),this,SLOT(showUpgradePage(QString)));
    connect(mainPage,SIGNAL(refreshAccountInfo()),this,SLOT(refreshAccountInfo()));
    connect(mainPage,SIGNAL(showTransferPage(QString)),this,SLOT(showTransferPage(QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));

}

void Frame::showTransferPage()
{
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage("",centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    transferPage->show();
    currentPageNum = 3;
    functionBar->choosePage(2);
}

void Frame::showContactPage()
{
    closeCurrentPage();    
    getAccountInfo();
    contactPage = new ContactPage(centralWidget);
    connect(contactPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(contactPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(contactPage,SIGNAL(gotoTransferPage(QString)),this,SLOT(showTransferPageWithAddress(QString)));
    contactPage->setAttribute(Qt::WA_DeleteOnClose);
    contactPage->show();
    currentPageNum = 4;
}

void Frame::showUpgradePage(QString name)
{
    closeCurrentPage();
    upgradePage = new UpgradePage(name, centralWidget);
    upgradePage->setAttribute(Qt::WA_DeleteOnClose);
    connect(upgradePage,SIGNAL(back(QString)), this, SLOT(showAccountPage(QString)));
    connect(upgradePage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(upgradePage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(upgradePage,SIGNAL(accountRenamed()),this,SLOT(refreshAccountInfo()));
    upgradePage->show();
    currentPageNum = 7;
}


bool Frame::eventFilter(QObject* watched, QEvent* e)
{
    if( (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::KeyPress)  )
    {
        updateTimer();
    }

    // currentDialog 上的鼠标事件也会移动 frame 和 本身
    if( Fry::getInstance()->currentDialogVector.contains(  (QWidget*)watched) )
    {
        if( e->type() == QEvent::MouseMove)
        {
            mouseMoveEvent( (QMouseEvent*)e);
        }
        else if( e->type() == QEvent::MouseButtonPress)
        {
            mousePressEvent( (QMouseEvent*)e);
        }
        else if( e->type() == QEvent::MouseButtonRelease)
        {
            mouseReleaseEvent( (QMouseEvent*)e);
        }

        Fry::getInstance()->resetPosOfCurrentDialog();  // currentDialog 一起移动

        return false;
    }

    return false;
}

void Frame::shadowWidgetShow()
{
    qDebug() << "shadowWidgetShow";
    shadowWidget->raise();
    shadowWidget->show();
}

void Frame::shadowWidgetHide()
{
    qDebug() << "shadowWidgetHide";
    shadowWidget->hide();
}

void Frame::showTransferPageWithAddress(QString address)
{
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage("",centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    transferPage->setAddress(address);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    transferPage->show();
    currentPageNum = 3;
    functionBar->choosePage(2);
}

void Frame::setLanguage(QString language)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    menuTranslator.load(QString(":/qm/qt_zh_cn"));
    translatorForTextBrowser.load(":/language/widgets.qm");

    if( language == "Simplified Chinese")
    {
        translator.load(":/language/gop_simplified_Chinese.qm");
        QApplication::installTranslator(&menuTranslator);
        QApplication::installTranslator(&translatorForTextBrowser);
    }
    else if( language == "English")
    {
        translator.load(":/language/gop_English.qm");
        QApplication::removeTranslator(&menuTranslator);
        QApplication::removeTranslator(&translatorForTextBrowser);
    }

    QApplication::installTranslator(&translator);


    if( titleBar != NULL)       // 已经登录
    {
        functionBar->retranslator();
        titleBar->retranslator();
        bottomBar->retranslator();
        shadowWidget->retranslator();

        switch (currentPageNum) {
        case 0:
            showMainPage();
            break;
        case 1:
//            mainPage->retranslator(language);
            showAccountPage( currentAccount);
            break;
        case 2:
            break;
        case 3:
//            transferPage->retranslator(language);
            showTransferPage(currentAccount);
            break;
        case 4:
//            contactPage->retranslator(language);
            showContactPage();
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
//            upgradePage->retranslator();
            showUpgradePage(currentAccount);
            break;
        case 8:
            break;
        default:
            break;
        }
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showWaittingForSyncWidget()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( selectGopPathWidget)
    {
        selectGopPathWidget->close();
        selectGopPathWidget = NULL;
    }

    waitingForSync = new WaitingForSync(this);
    waitingForSync->setAttribute(Qt::WA_DeleteOnClose);
    waitingForSync->move(0,0);
    connect( waitingForSync,SIGNAL(sync()), this, SLOT(syncFinished()));
    connect( waitingForSync,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect( waitingForSync,SIGNAL(tray()),this,SLOT(hide()));
    connect( waitingForSync,SIGNAL(closeGOP()),this,SLOT(stop()));
    connect( waitingForSync,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect( waitingForSync,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

    waitingForSync->show();

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::jsonDataUpdated(QString id)
{
    if( id == "id_open")
    {
        waitingForSync->close();
        waitingForSync = NULL;

        activateWindow();

        if( Fry::getInstance()->jsonDataValue(id) == "\"result\":null")
        {
            normalLogin = new NormalLogin(this);
            firstLogin = NULL;
            normalLogin->setGeometry(0,0,960,580);
            normalLogin->show();
            connect( normalLogin,SIGNAL(login()), this, SLOT(alreadyLogin()));
            connect( normalLogin,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( normalLogin,SIGNAL(closeGOP()),this,SLOT(stop()));
            connect( normalLogin,SIGNAL(tray()),this,SLOT(hide()));
            connect( normalLogin,SIGNAL(showShadowWidget()), this, SLOT(shadowWidgetShow()));
            connect( normalLogin,SIGNAL(hideShadowWidget()), this,SLOT(shadowWidgetHide()));
        }
        else
        {
            firstLogin = new FirstLogin(this);
            normalLogin = NULL;
            firstLogin->setGeometry(0,0,960,580);
            firstLogin->show();
            connect( firstLogin,SIGNAL(login()), this, SLOT(alreadyLogin()));
            connect( firstLogin,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( firstLogin,SIGNAL(closeGOP()),this,SLOT(stop()));
            connect( firstLogin,SIGNAL(tray()),this,SLOT(hide()));
            connect( firstLogin,SIGNAL(showShadowWidget()), this, SLOT(shadowWidgetShow()));
            connect( firstLogin,SIGNAL(hideShadowWidget()), this,SLOT(shadowWidgetHide()));
        }
        return;
    }

    if( id == "id_wallet_list_my_addresses")
    {
        QString result = Fry::getInstance()->jsonDataValue(id);

        QStringList accountList;

        QStringList strList = result.split("},{");

        foreach (QString str, strList)
        {
            int pos = str.indexOf("\"name\":\"") + 8;
            if( pos == 7)  break;   // 如果还没有账号

            accountList += str.mid(pos, str.indexOf("\",", pos) - pos);
        }

        mutexForConfigFile.lock();
        Fry::getInstance()->configFile->beginGroup("/accountInfo");
        QStringList keys = Fry::getInstance()->configFile->childKeys();
        Fry::getInstance()->configFile->endGroup();


        foreach (QString key, keys)
        {
            QString addName = Fry::getInstance()->configFile->value("/accountInfo/" + key).toString();
            if( !accountList.contains( addName) && !addName.isEmpty())
            {
                // 如果config记录的账户钱包中没有 则清除config文件中记录以及内存中各map
                Fry::getInstance()->deleteAccountInConfigFile( addName);
                Fry::getInstance()->addressMapRemove( addName);
                Fry::getInstance()->balanceMapRemove( addName);
                Fry::getInstance()->registerMapRemove( addName);
            }
        }

        Fry::getInstance()->configFile->beginGroup("/accountInfo");
        keys = Fry::getInstance()->configFile->childKeys();

        foreach (QString key, keys)
        {
            QString addName = Fry::getInstance()->configFile->value(key).toString();
            Fry::getInstance()->balanceMapInsert(addName, Fry::getInstance()->getBalance( addName));
            Fry::getInstance()->registerMapInsert(addName, Fry::getInstance()->getRegisterTime( addName));
            Fry::getInstance()->addressMapInsert(addName, Fry::getInstance()->getAddress (addName));
            accountList.removeAll( addName);     // 如果钱包中有账号 config中未记录， 保留在accountList里
        }
        Fry::getInstance()->configFile->endGroup();

        if( !accountList.isEmpty())    // 把config中没有记录的账号写入config中
        {
            for( int i = 0; i < accountList.size(); i++)
            {
                QString addName = accountList.at(i);
                mutexForBalanceMap.lock();
                QString accountName = QString(QString::fromLocal8Bit("账户") + toThousandFigure(Fry::getInstance()->balanceMap.size() + 1));
                mutexForBalanceMap.unlock();
                Fry::getInstance()->configFile->setValue("/accountInfo/" + accountName, addName);
                Fry::getInstance()->balanceMapInsert(addName, Fry::getInstance()->getBalance( addName));
                Fry::getInstance()->registerMapInsert(addName, Fry::getInstance()->getRegisterTime( addName));
                Fry::getInstance()->addressMapInsert(addName, Fry::getInstance()->getAddress ( addName));
            }
        }
        mutexForConfigFile.unlock();

        if( needToRefresh)
        {
            refresh();
            needToRefresh = false;
        }

        return;
    }


    if( id == "id_lock")
    {
        if( lockPage )
        {
            qDebug() << "already exist a lockpage";
            return;
        }

        QString result = Fry::getInstance()->jsonDataValue(id);
qDebug() << id << result;
        if( result == "\"result\":null")
        {
            shadowWidgetHide();

            lockPage = new LockPage(this);
            lockPage->setAttribute(Qt::WA_DeleteOnClose);
            lockPage->setGeometry(0,0,960,580);
            connect( lockPage,SIGNAL(unlock()),this,SLOT(unlock()));
            connect( lockPage,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( lockPage,SIGNAL(closeGOP()),this,SLOT(stop()));
            connect( lockPage,SIGNAL(tray()),this,SLOT(hide()));
            connect( lockPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( lockPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
            lockPage->show();

            Fry::getInstance()->hideCurrentDialog();

        }
        return;
    }


    if( id.mid(0,37) == "id_wallet_get_account_public_address-" )
    {
        QString  result = Fry::getInstance()->jsonDataValue(id);
        QString name = id.mid(37);

        if( result.mid(0,9) == "\"result\":")
        {
            QString address = result.mid(10);
            address.remove('\"');
            Fry::getInstance()->addressMapInsert(name, address);

            refreshAccountInfo();
        }

        return;
    }
}

void Frame::closeEvent(QCloseEvent *e)
{

    hide();
    e->ignore();

}

void Frame::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        //点击托盘显示窗口
        case QSystemTrayIcon::Trigger:
        {
            showNormalAndActive();
            break;
        }

        default:
            break;
    }
}

void Frame::createTrayIconActions()
{
     minimizeAction = new QAction(tr("Minimize"), this);
     connect(minimizeAction, SIGNAL(triggered()), this, SLOT(showMinimized()));

     restoreAction = new QAction(tr("Restore"), this);
     connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormalAndActive()));

     quitAction = new QAction(tr("Quit"), this);
     connect(quitAction, SIGNAL(triggered()), this, SLOT(stop()));
}

void Frame::createTrayIcon()
{
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(minimizeAction);
     trayIconMenu->addAction(restoreAction);
     trayIconMenu->addSeparator();
     trayIconMenu->addAction(quitAction);
     trayIcon->setContextMenu(trayIconMenu);
}

void Frame::showNormalAndActive()
{
    showNormal();
    activateWindow();
}


// 提前载入，防止切换到别的界面显示不出来
void Frame::init()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_list_pending_transactions", "blockchain_list_pending_transactions", QStringList() << "" ));

    Fry::getInstance()->postRPC(  toJsonFormat( "id_wallet_set_transaction_scanning", "wallet_set_transaction_scanning", QStringList() << "true" ) );

    Fry::getInstance()->postRPC(  toJsonFormat( "id_delegate_set_network_min_connection_count", "delegate_set_network_min_connection_count", QStringList() << "0" ) );

    Fry::getInstance()->postRPC(  toJsonFormat( "id_wallet_delegate_set_block_production", "wallet_delegate_set_block_production", QStringList() << "ALL" << "true" ) );

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::scan()
{
    Fry::getInstance()->postRPC( toJsonFormat( "id_scan", "scan", QStringList() << "0"));
}

void Frame::newAccount(QString name)
{
    Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_get_account_public_address-" + name, "wallet_get_account_public_address", QStringList() << name));
}

void Frame::stop()
{
    Fry::getInstance()->write("stop\n");
    qApp->quit();
}

