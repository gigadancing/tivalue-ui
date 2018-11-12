/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
         佛祖保佑       永无BUG
*/

#ifndef TIC_H
#define TIC_H
#include <QObject>
#include <QMap>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QMutex>
#include <QDialog>

#define ASSET_NAME "TV"
#define ASSET_NAME_ACS "ACS"
#define TICWALLET_VERSION "1.2.0"           // 版本号
#define AUTO_REFRESH_TIME 5000           // 自动刷新时间(ms)

//  密码输入错误5次后 锁定一段时间 (秒)
#define PWD_LOCK_TIME  7200
#define RPC_PORT 53060


class QTimer;
class QFrame;
class WorkerThreadManager;

static QMutex mutexForJsonData;
static QMutex mutexForPending;
static QMutex mutexForConfigFile;
static QMutex mutexForMainpage;
static QMutex mutexForPendingFile;
static QMutex mutexForDelegateList;
static QMutex mutexForRegisterMap;
static QMutex mutexForBalanceMap;
static QMutex mutexForAddressMap;
static QMutex mutexForRpcReceiveOrNot;


class Fry : public QObject
{
    Q_OBJECT
public:
    ~Fry();
    static Fry*   getInstance();
    qint64 write(QString);
    void quit();
    QString read();
    QProcess* proc;
    int lockMinutes;   // 自动锁定时间
    bool notProduce;   // 是否产块/记账
    bool minimizeToTray;  // 是否最小化到托盘
    bool closeToMinimize; // 是否点击关闭最小化
    QString language;   // 语言
    bool needToScan;   // 在当前scan完成后是否还需要scan

    QMap<QString,QString> balanceMap;
    QMap<QString,QString> addressMap;
    QMap<QString,QString> registerMap;
    QStringList delegateAccountList;
    QStringList delegateList;
    bool hasDelegateSalary;
    QMap<QString,double> delegateSalaryMap;
    QMap<QString,bool> rpcReceivedOrNotMap;  // 标识rpc指令是否已经返回了，如果还未返回则忽略

    QString getAddress(QString);
    QString getBalance(QString);
    QString getRegisterTime(QString);
    void deleteAccountInConfigFile(QString);
    void updateJsonDataMap(QString id, QString data);
    QString jsonDataValue(QString id);
    double getPendingAmount(QString name);
    QString getPendingInfo(QString id);
    int getDelegateIndex(QString delegateName);

    QString registerMapValue(QString key);
    void registerMapInsert(QString key, QString value);
    int registerMapRemove(QString key);
    QString balanceMapValue(QString key);
    void balanceMapInsert(QString key, QString value);
    int balanceMapRemove(QString key);
    QString addressMapValue(QString key);
    void addressMapInsert(QString key, QString value);
    int addressMapRemove(QString key);
    bool rpcReceivedOrNotMapValue(QString key);
    void rpcReceivedOrNotMapSetValue(QString key, bool received);

    void appendCurrentDialogVector(QWidget*);
    void removeCurrentDialogVector(QWidget *);
    void hideCurrentDialog();
    void showCurrentDialog();
    void resetPosOfCurrentDialog();

    void initWorkerThreadManager();
    void destroyWorkerThreadManager();
    void postRPC(QString cmd);

    void getContactsFile();  // contacts.dat 改放到数据路径

    QVector<QWidget*> currentDialogVector;  // 保存不属于frame的dialog
                                            // 为的是自动锁定的时候hide这些dialog

    QSettings *configFile;
//    void loadAccountInfo();

    QString appDataPath;
    QString walletConfigPath;

    QFile* contactsFile;
    QFile* pendingFile;

    QThread* threadForWorkerManager;

    QDialog* currentDialog;  // 如果不为空 则指向当前最前面的不属于frame的dialog
                             // 为的是自动锁定的时候hide该dialog

    QFrame* mainFrame = NULL; // 指向主窗口的指针

    int currentPort;          // 当前rpc 端口

    QString localIP;   // 保存 peerinfo 获得的本机IP和端口



signals:
    void started();

    void jsonDataUpdated(QString);

    void rpcPosted(QString cmd);

private:

    Fry();
    static Fry* goo;
    QMap<QString,QString> jsonDataMap;   //  各指令的id,各指令的返回
    WorkerThreadManager* workerManager;  // 处理rpc worker thread

    void getSystemEnvironmentPath();
    void changeToWalletConfigPath();     // 4.2.2后config pending log 等文件移动到 APPDATA路径

    class CGarbo // 它的唯一工作就是在析构函数中删除CSingleton的实例
    {
    public:
        ~CGarbo()
        {
            if (Fry::goo)
                delete Fry::goo;
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
};
#endif // TIC_H

QString doubleTo2Decimals(double number, bool rounding = false);
