#include "tic.h"
#include "debug_log.h"
#include "workerthreadmanager.h"
#include <QTextCodec>
#include <QDebug>
#include <QObject>
#include <Windows.h>
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QFrame>
#include <QDir>

Fry* Fry::goo = 0;
//QMutex mutexForJsonData;
//QMutex mutexForPending;
//QMutex mutexForConfigFile;
//QMutex mutexForMainpage;
//QMutex mutexForPendingFile;
//QMutex mutexForDelegateList;
//QMutex mutexForRegisterMap;
//QMutex mutexForBalanceMap;
//QMutex mutexForAddressMap;
//QMutex mutexForRpcReceiveOrNot;

Fry::Fry()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    proc = new QProcess;

    workerManager = NULL;

    getSystemEnvironmentPath();
    changeToWalletConfigPath();

    threadForWorkerManager = NULL;
    currentDialog = NULL;
    hasDelegateSalary = false;
    needToScan = false;
    currentPort = RPC_PORT;

    configFile = new QSettings( walletConfigPath + "/config.ini", QSettings::IniFormat);
    if( configFile->value("/settings/lockMinutes").toInt() == 0)   // 如果第一次，没有config.ini
    {
        configFile->setValue("/settings/lockMinutes",5);
        lockMinutes     = 5;
        configFile->setValue("/settings/notAutoLock",false);
        notProduce      =  true;
        configFile->setValue("/settings/language","Simplified Chinese");
        language = "Simplified Chinese";
        minimizeToTray  = false;
        configFile->setValue("/settings/minimizeToTray",false);
        closeToMinimize = false;
        configFile->setValue("/settings/closeToMinimize",false);


    }
    else
    {
        lockMinutes     = configFile->value("/settings/lockMinutes").toInt();
        if( lockMinutes < 1)    lockMinutes = 5;

        notProduce      = !configFile->value("/settings/notAutoLock").toBool();
        minimizeToTray  = configFile->value("/settings/minimizeToTray").toBool();
        closeToMinimize = configFile->value("/settings/closeToMinimize").toBool();
        language        = configFile->value("/settings/language").toString();

    }

    QFile file( walletConfigPath + "/log.txt");       // 每次启动清空 log.txt文件
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

//    contactsFile = new QFile( "contacts.dat");
    contactsFile = NULL;

    pendingFile  = new QFile( walletConfigPath + "/pending.dat");

    DLOG_QT_WALLET_FUNCTION_END;
}

Fry::~Fry()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

	if (configFile)
	{
		delete configFile;
		configFile = NULL;
	}

    if( contactsFile)
    {
        delete contactsFile;
        contactsFile = NULL;
    }

    if( threadForWorkerManager)
    {
        delete threadForWorkerManager;
        threadForWorkerManager = NULL;
    }

    if( workerManager)
    {
        delete workerManager;
        workerManager = NULL;
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

Fry*   Fry::getInstance()
{
    if( goo == NULL)
    {
        goo = new Fry;
    }
    return goo;
}


qint64 Fry::write(QString cmd)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray cmdBa = gbkCodec->fromUnicode(cmd);  // 转为gbk的bytearray
    proc->readAll();
    return proc->write(cmdBa.data());
}

QString Fry::read()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result;
    QString str;
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    while ( !result.contains(">>>"))        // 确保读取全部输出
    {
        proc->waitForReadyRead(50);
        str = gbkCodec->toUnicode(proc->readAll());
        result += str;
        if( str.right(2) == ": " )  break;

        //  手动调用处理未处理的事件，防止界面阻塞
//        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    QApplication::restoreOverrideCursor();
    return result;

}

void Fry::deleteAccountInConfigFile(QString accountName)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    mutexForConfigFile.lock();
    configFile->beginGroup("/accountInfo");
    QStringList keys = configFile->childKeys();

    int i = 0;
    for( ; i < keys.size(); i++)
    {
        if( configFile->value( keys.at(i)) == accountName)
        {
            break;
        }
    }

    for( ; i < keys.size() - 1; i++)
    {
        configFile->setValue( keys.at(i) , configFile->value( keys.at(i + 1)));
    }

    configFile->remove( keys.at(i));

    configFile->endGroup();
    mutexForConfigFile.unlock();

    DLOG_QT_WALLET_FUNCTION_END;
}


QString Fry::getAddress(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }

    QString result = jsonDataValue("id_wallet_list_my_addresses");

    int pos = result.indexOf( "\"name\":\"" + name + "\",") ;
    if( pos != -1)  // 如果 wallet_list_my_addresses 中存在
    {
        int pos2 = result.indexOf( "\"owner_address\":", pos) + 17;
        QString address = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);
//        address += "ffffffffffffffffffffffffffffffff";
        return address;
    }

    return NULL;
}

QString Fry::getBalance(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }

    QString result = jsonDataValue("id_balance");
    int p = result.indexOf( "[\"" + name + "\",");
    if( p != -1)  // 如果balance中存在
    {
        int pos = p + 8 + name.size();
        QString str = result.mid( pos, result.indexOf( "]", pos) - pos);
        str.remove("\"");

        double amount = str.toDouble() / 100000;

        return doubleTo2Decimals( amount) + " TV";
    }

    // balance中不存在
    return "0.00 TV";
}

QString Fry::getRegisterTime(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }

    QString result = jsonDataValue("id_wallet_list_my_addresses");
    int pos = result.indexOf( "\"name\":\"" + name + "\",") ;
    if( pos != -1)  // 如果 wallet_list_my_addresses 中存在
    {
        int pos2 = result.indexOf( "\"registration_date\":", pos) + 21;
        QString registerTime = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);

        if( registerTime != "1970-01-01T00:00:00")
        {
            return registerTime;
        }
        else
        {
            return "NO";
        }
    }

    return "WRONGACCOUNTNAME";
}

void Fry::getSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {
            walletConfigPath = str.mid(8) + "\\TVWallet";
            appDataPath = walletConfigPath + "\\chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#elif defined(TARGET_OS_MAC)
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            appDataPath = str.mid(5) + "/Library/Application Support/TV";
            walletConfigPath = str.mid(5) + "/TVWallet";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#else
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            appDataPath = str.mid(5) + "/TV";
            walletConfigPath = str.mid(5) + "/TVWallet";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#endif
}

void Fry::changeToWalletConfigPath()
{
    QFile file("config.ini");
    if( !file.exists())     return;
    QFile file2(walletConfigPath + "/config.ini");
    qDebug() << file2.exists() << walletConfigPath + "/config.ini";
    if( file2.exists())
    {
        qDebug() << "remove config.ini : " << file.remove();
        return;
    }

    qDebug() << "copy config.ini : " << file.copy(walletConfigPath + "/config.ini");
    qDebug() << "remove old config.ini : " << file.remove();
}

void Fry::getContactsFile()
{
    QString path;
    if( configFile->contains("/settings/fryPath"))
    {
        path = configFile->value("/settings/fryPath").toString();
    }
    else
    {
        path = appDataPath;
    }

    contactsFile = new QFile(path + "\\contacts.dat");
    if( contactsFile->exists())
    {
        // 如果数据路径下 有contacts.dat 则使用数据路径下的
        return;
    }
    else
    {
        QFile file2("contacts.dat");
        if( file2.exists())
        {
            // 如果数据路径下没有 钱包路径下有 将钱包路径下的剪切到数据路径下
       qDebug() << "contacts.dat copy" << file2.copy(path + "\\contacts.dat");
       qDebug() << "contacts.dat copy" << file2.remove();
            return;
        }
        else
        {
            // 如果都没有 则使用钱包路径下的
        }
    }
}



void Fry::quit()
{
    if (proc)
    {
        proc->close();
        qDebug() << "proc: close";
        delete proc;
        proc = NULL;
    }
}

void Fry::updateJsonDataMap(QString id, QString data)
{
    mutexForJsonData.lock();
    jsonDataMap.insert( id, data);
    emit jsonDataUpdated(id);
    mutexForJsonData.unlock();
}

QString Fry::jsonDataValue(QString id)
{

    mutexForJsonData.lock();

    QString value = jsonDataMap.value(id);

    mutexForJsonData.unlock();

    return value;
}

double Fry::getPendingAmount(QString name)
{
    mutexForConfigFile.lock();
    if( !pendingFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "pending.dat not exist";
        return 0;
    }
    QString str = QByteArray::fromBase64( pendingFile->readAll());
    pendingFile->close();
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForConfigFile.unlock();

    double amount = 0;
    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");
        if( sList.at(1) == name)
        {
            amount += sList.at(2).toDouble() + sList.at(3).toDouble();
        }
    }

    return amount;
}

QString Fry::getPendingInfo(QString id)
{
    mutexForConfigFile.lock();
    if( !pendingFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "pending.dat not exist";
        return 0;
    }
    QString str = QByteArray::fromBase64( pendingFile->readAll());
    pendingFile->close();
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForConfigFile.unlock();

    QString info;
    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");
        if( sList.at(0) == id)
        {
            info = ss;
            break;
        }
    }

    return info;
}

int Fry::getDelegateIndex(QString delegateName)
{
    mutexForDelegateList.lock();
    QStringList singleList = Fry::getInstance()->delegateList.filter( "\"name\":\"" + delegateName + "\"");
    mutexForDelegateList.unlock();
    if( singleList.size() == 0) return -1;  // 代理list中没有时 返回 -1
    int pos = singleList.at(0).indexOf("\"index\":") + 8;
    int rank = singleList.at(0).mid(pos).toInt();
    return rank;
}

QString doubleTo2Decimals(double number, bool rounding)  // 5位小数去掉后三位小数, rounding为false舍尾，为true进1
{
    if( !rounding)
    {
        QString num = QString::number(number,'f', 5);
        int pos = num.indexOf('.') + 3;
        return num.mid(0,pos);
    }
    else
    {
        QString num = QString::number(number,'f', 5);
        int pos = num.indexOf('.') + 3;
        QString round = num.mid(0,pos);
        double sub = num.toDouble() - round.toDouble();
        if( sub > 0.000001 )
        {
            return QString::number(round.toDouble() + 0.01,'f', 2);
        }
        else
        {
            return round;
        }

    }

}

QString Fry::registerMapValue(QString key)
{
    mutexForRegisterMap.lock();
    QString value = registerMap.value(key);
    mutexForRegisterMap.unlock();

    return value;
}

void Fry::registerMapInsert(QString key, QString value)
{
    mutexForRegisterMap.lock();
    registerMap.insert(key,value);
    mutexForRegisterMap.unlock();
}

int Fry::registerMapRemove(QString key)
{
    mutexForRegisterMap.lock();
    int number = registerMap.remove(key);
    mutexForRegisterMap.unlock();
    return number;
}

QString Fry::balanceMapValue(QString key)
{
    mutexForBalanceMap.lock();
    QString value = balanceMap.value(key);
    mutexForBalanceMap.unlock();

    return value;
}

void Fry::balanceMapInsert(QString key, QString value)
{
    mutexForBalanceMap.lock();
    balanceMap.insert(key,value);
    mutexForBalanceMap.unlock();
}

int Fry::balanceMapRemove(QString key)
{
    mutexForBalanceMap.lock();
    int number = balanceMap.remove(key);
    mutexForBalanceMap.unlock();
    return number;
}

QString Fry::addressMapValue(QString key)
{
    mutexForAddressMap.lock();
    QString value = addressMap.value(key);
    mutexForAddressMap.unlock();

    return value;
}

void Fry::addressMapInsert(QString key, QString value)
{
    mutexForAddressMap.lock();
    addressMap.insert(key,value);
    mutexForAddressMap.unlock();
}

int Fry::addressMapRemove(QString key)
{
    mutexForAddressMap.lock();
    int number = addressMap.remove(key);
    mutexForAddressMap.unlock();
    return number;
}

bool Fry::rpcReceivedOrNotMapValue(QString key)
{
    mutexForRpcReceiveOrNot.lock();
    bool received = rpcReceivedOrNotMap.value(key);
    mutexForRpcReceiveOrNot.unlock();
    return received;
}

void Fry::rpcReceivedOrNotMapSetValue(QString key, bool received)
{
    mutexForRpcReceiveOrNot.lock();
    rpcReceivedOrNotMap.insert(key, received);
    mutexForRpcReceiveOrNot.unlock();
}


void Fry::appendCurrentDialogVector( QWidget * w)
{
    currentDialogVector.append(w);
}

void Fry::removeCurrentDialogVector( QWidget * w)
{
    currentDialogVector.removeAll(w);
}

void Fry::hideCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->hide();
    }
}

void Fry::showCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        qDebug() << "showCurrentDialog :" << w;
        w->show();
        w->move( mainFrame->pos());  // lock界面可能会移动，重新显示的时候要随之移动
    }
}

void Fry::resetPosOfCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->move( mainFrame->pos());
    }
}

void Fry::initWorkerThreadManager()
{
    qDebug() << "initWorkerThreadManager " << QThread::currentThreadId();
    if( workerManager)
    {
        delete workerManager;
    }
    if( threadForWorkerManager)
    {
        delete threadForWorkerManager;
    }

    threadForWorkerManager = new QThread;
    threadForWorkerManager->start();
    workerManager = new WorkerThreadManager;
    workerManager->moveToThread(threadForWorkerManager);
    connect(this, SIGNAL(rpcPosted(QString)), workerManager, SLOT(processRPCs(QString)));

}

void Fry::destroyWorkerThreadManager()
{
    qDebug() << "destroyWorkerThreadManager " << QThread::currentThreadId();
    if( workerManager)
    {
        workerManager->deleteLater();
        workerManager = NULL;

        if( threadForWorkerManager)
        {
            threadForWorkerManager->deleteLater();
            threadForWorkerManager = NULL;
        }
    }

}

void Fry::postRPC(QString cmd)
{
    if( rpcReceivedOrNotMap.contains( cmd))
    {
        if( rpcReceivedOrNotMap.value( cmd) == true)
        {
            rpcReceivedOrNotMapSetValue( cmd, false);
            emit rpcPosted(cmd);
        }
        else
        {
            // 如果标识为未返回 则不重复排入事件队列
            return;
        }
    }
    else
    {
        rpcReceivedOrNotMapSetValue( cmd, false);
        emit rpcPosted(cmd);
    }

}
