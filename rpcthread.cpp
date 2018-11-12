#include "rpcthread.h"
#include <QTcpSocket>
#include "tic.h"
#include <QHostAddress>
#include <QJsonDocument>

RpcThread::RpcThread()
{
  writeData.clear();
  loginOrNot = false;
  loginUser = "";
  loginPwd  = "";
}

RpcThread::~RpcThread()
{
}

void RpcThread::run()
{
  //qDebug() << "rpcrun +++++++++ ||" << QThread::currentThreadId() << "||" <<  writeData;
  if( !writeData.isEmpty())
  {
    QTcpSocket socket;
    socket.connectToHost(QHostAddress("127.0.0.1"), RPC_PORT);
    if (!socket.waitForConnected())
    {
      qDebug() << "could not connect to server: " << socket.errorString();
      return;
    }

    if (loginOrNot)
    {
      QString loginRpc = "{\"jsonrpc\":\"2.0\",\"id\":\"id_login\",\"method\":\"login\",\"params\":[\"" + loginUser + "\",\"" + loginPwd + "\"]}";
      socket.write(loginRpc.toUtf8());
      socket.waitForReadyRead(120000);
      QString loginReceive = socket.readAll();
      QString idLogin = loginReceive.mid(loginReceive.indexOf("\"id\":") + 6, loginReceive.indexOf(",") - loginReceive.indexOf("\"id\":") - 7);
      QString resultLogin = loginReceive.mid(loginReceive.indexOf(",") + 1);
      resultLogin = resultLogin.left(resultLogin.size() - 2);
      Fry::getInstance()->updateJsonDataMap(idLogin, resultLogin);
    }

    foreach (QString data, writeData)
    {
      socket.write(data.toUtf8());
      QString receive;
      // 如果数据太长， 循环读取 直到读到的 "{" 和 "}" 一样多
      do {
        socket.waitForReadyRead(120000);
        receive += socket.readAll();
      } while (receive.count("{") > receive.count("}"));

      QString id = receive.mid(receive.indexOf("\"id\":") + 6, receive.indexOf(",") - receive.indexOf("\"id\":") - 7);
      QString result = receive.mid(receive.indexOf(",") + 1);
      result = result.left(result.size() - 2);
      Fry::getInstance()->updateJsonDataMap(id, result);
    }
  }
}

void RpcThread::setWriteData(QString data)
{
  writeData << data;
}

QString toJsonFormat(QString id, QString instruction, QStringList parameters)
{
  QString str = "{\"jsonrpc\":\"2.0\",\"id\":\"";
  str += id;
  str += "\",\"method\":\"" + instruction;
  str += "\",\"params\":[" ;

  foreach (QString para, parameters)
  {
    str += "\"" + para + "\"" + ",";
  }
  str = str.left(str.size() - 1);
  str += "]}";

  // 判断是否组成一个完整的json  不完整返回空字符串
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(str.toLatin1(), &json_error);
  if (json_error.error == QJsonParseError::NoError)
  {
    return str;
  }
  else
  {
    qDebug() << "json error: " << json_error.errorString();
    qDebug() << "wrong json string: " << str;
    return "";
  }
}

void RpcThread::setLogin(QString user, QString pwd)
{
  loginOrNot = true;
  loginUser = user;
  loginPwd  = pwd;
}
