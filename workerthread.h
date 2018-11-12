#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QEvent>

#define LOGIN_USER "a"
#define LOGIN_PWD  "b"

class QTcpSocket;

class TaskEvent : public QEvent
{
public:
    TaskEvent(QString cmd)
        : QEvent(evType())
        , rpcCmd(cmd)
    {}

    static QEvent::Type evType()
    {
        if(s_evType == QEvent::None)
        {
            s_evType = (QEvent::Type)registerEventType();
        }
        return s_evType;
    }

    QString rpcCmd;

private:
    static QEvent::Type s_evType;
};


class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(int workerId, QObject *parent = 0);
    ~WorkerThread();

    bool isBusy();

    void setBusy(bool busyOrNot);

    void processRPC(QString cmd);

protected:
    void run();
    bool event(QEvent *event);

private:
    int m_id;
    QTcpSocket* socket;  // 在QThread中定义的所有东西都属于创建该QThread的线程。
                         // 如果不用指针 在构造时初始化socket， socket属于主线程
                         // 在run中 new 则属于该线程
    bool busy;

};

#endif // WORKERTHREAD_H
