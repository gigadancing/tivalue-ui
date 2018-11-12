#ifndef FRAME_H
#define FRAME_H
#include <QFrame>
#include <QWidget>
#include <QMap>
#include <QTranslator>
#include <QSystemTrayIcon>

#include "windows.h"

namespace Ui {
   class Frame;
}

class FirstLogin;
class NormalLogin;
class MainPage;
class AccountPage;
class TransferPage;
class BottomBar;
class LockPage;
class TitleBar;
class QMenu;
class WaitingForSync;
class FunctionBar;
class ContactPage;
class UpgradePage;
class ApplyDelegatePage;
class SelectGopPathWidget;
class ShowBottomBarWidget;
class ShadowWidget;
class UploadFilePage;
class UploadBar;
class StorageFilePage;
class StorageFileBar;
class DownloadFilePage;
class DownloadBar;
class QProcess;


class Frame:public QFrame
{
    Q_OBJECT
public:
    Frame();
    ~Frame();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

public slots:
    void refreshAccountInfo();
    void refresh();
    void autoRefresh();
    void shadowWidgetShow();
    void shadowWidgetHide();
    void setLanguage(QString);
    void syncFinished();
signals:
    void started();
    void delegateListUpdated();

private slots:
    void alreadyLogin();
    void showAccountPage(QString);
    void showTransferPage(QString);
    void showLockPage();
    void autoLock();
    void unlock();
    void updateTimer();
    void settingSaved();
    void privateKeyImported();
    void showUploadPage();
    void showStoragePage();
    void showDownloadPage();

    void uploadSuccess(QString filename, QString filesize);
    void applayStorage();
    void downloadFile();

    void jsonDataUpdated(QString id);

    void showMainPage();
    void showTransferPage();
    void showContactPage();
    void showUpgradePage(QString);
    void showTransferPageWithAddress(QString);
    void showWaittingForSyncWidget();

    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
    void showNormalAndActive();

    void scan();

    void newAccount(QString name);

    void stop();

private:
    bool mouse_press;
    QPoint move_point;
    SelectGopPathWidget*   selectGopPathWidget;
    FirstLogin* firstLogin;
    NormalLogin* normalLogin;
    MainPage*   mainPage;
    AccountPage* accountPage;
    TransferPage* transferPage;
    UploadFilePage* uploadFilePage;
    UploadBar* uploadPageBar;
    StorageFilePage* storagePage;
    StorageFileBar* storageBar;
    DownloadFilePage* downloadFilePage;
    DownloadBar* downloadBar;

    BottomBar* bottomBar;
    QWidget* centralWidget;
    LockPage*  lockPage;
    QTimer* timer;
    TitleBar* titleBar;
    QString lastPage;
    QString currentAccount;
    WaitingForSync* waitingForSync;
    int currentPageNum;  //  0:mainPage   1:accountPage  2:delegatePgae  3:transferPage    4:contactPage
                         //   6:   7:upgradePage 8:storagePage
    ShadowWidget* shadowWidget;
    QSystemTrayIcon* trayIcon;
    void createTrayIconActions();
    void createTrayIcon();
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMenu *trayIconMenu;
//    ShowBottomBarWidget* showBottomBarWidget;

    RECT rtConfined;   // 由于定义了 framelesswindowhint 为了不让鼠标拖动时能移到任务栏下
    RECT rtDefault;

    void  getAccountInfo();
    void startTimerForAutoRefresh();      // 自动刷新
    QTimer* timerForAutoRefresh;
    FunctionBar* functionBar;
    void closeCurrentPage();
    bool eventFilter(QObject *watched, QEvent *e);
    void closeEvent(QCloseEvent* e);
    void init();

    ContactPage* contactPage;
    UpgradePage* upgradePage;

    QTranslator translator;         //  选择语言
    QTranslator menuTranslator;     //  右键菜单语言
    QTranslator translatorForTextBrowser;   // QTextBrowser的右键菜单翻译

    bool needToRefresh;

    QProcess* ipfsprocess;

};


#endif // FRAME_H
