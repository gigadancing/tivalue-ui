#ifndef RPCTHREAD_H
#define RPCTHREAD_H


#include <QThread>
#include <QStringList>

QString toJsonFormat(QString id, QString instruction, QStringList parameters);

class RpcThread : public QThread
{
    Q_OBJECT
public:
    RpcThread();
    void run();
    void setWriteData(QString);
    void setLogin(QString user, QString pwd);
    ~RpcThread();

private:
    QStringList writeData;

    bool loginOrNot;
    QString loginUser;
    QString loginPwd;
};

#endif // RPCTHREAD_H
