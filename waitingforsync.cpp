#include "waitingforsync.h"
#include "ui_waitingforsync.h"
#include "tic.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "commondialog.h"
#include <QTimer>
#include <QDebug>
#include <QMovie>
#include <QDesktopServices>
#include <QMessageBox>

WaitingForSync::WaitingForSync(QWidget *parent) :
    QWidget(parent),
    updateOrNot(false),
    updateExe(NULL),
    synced(false),
    reply(NULL),
    ui(new Ui::WaitingForSync)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/ticpic/bg.png")));
    setPalette(palette);

    timer = new QTimer(this);    
    connect(timer,SIGNAL(timeout()),this,SLOT(updateInfo()));
    timer->start(5000);

    connect(Fry::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(infoUpdated(QString)));


    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/minimize2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                              "QToolButton:hover{background-image:url(:/pic/pic2/minimize_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic/pic2/close_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");



    getUpdateXml();
    DLOG_QT_WALLET_FUNCTION_END;
}

WaitingForSync::~WaitingForSync()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}

void WaitingForSync::updateInfo()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    RpcThread* rpcThread = new RpcThread;
    connect(rpcThread, SIGNAL(finished()), rpcThread, SLOT(deleteLater()));
    rpcThread->setWriteData( toJsonFormat( "id_info", "info", QStringList() << ""));
    rpcThread->start();

    updateBebuildInfo();

    DLOG_QT_WALLET_FUNCTION_END;
}

void WaitingForSync::on_minBtn_clicked()
{
    if( Fry::getInstance()->minimizeToTray)
    {
        emit tray();
    }
    else
    {
        emit minimum();
    }
}

void WaitingForSync::on_closeBtn_clicked()
{
    if( Fry::getInstance()->closeToMinimize)
    {
        emit tray();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr( "Sure to close Fry Wallet?"));
        bool choice = commonDialog.pop();

        if( choice)
        {
            emit closeGOP();
        }
        else
        {
            return;
        }

    }
}

void WaitingForSync::infoUpdated(QString id)
{
    if( id != "id_info")  return;

    QString result = Fry::getInstance()->jsonDataValue( id);

    int pos = result.indexOf( "\"blockchain_head_block_age\":") + 28;
    QString seconds = result.mid( pos, result.indexOf("\"blockchain_head_block_timestamp\":") - pos - 1);
    qDebug() << "info seconds : " << seconds;
    qDebug() << "updateOrNot : "  << updateOrNot;
    // 不管是否连上过节点 直接进入
    if( /* seconds.toInt() != 0 && */ !updateOrNot && !synced)
    {
        emit sync();
        synced = true;
        return;
    }

    return;
}

// 比较版本号 若 a > b返回 1, a = b返回 0, a < b 返回 -1
int compareVersion( QString a, QString b)
{
    if( a == b)  return 0;

    QStringList aList = a.split(".");
    QStringList bList = b.split(".");

    if( aList.at(0).toInt() > bList.at(0).toInt() )
    {
        return 1;
    }
    else if( aList.at(0) < bList.at(0))
    {
        return -1;
    }
    else
    {
        if( aList.at(1).toInt() > bList.at(1).toInt() )
        {
            return 1;
        }
        else if( aList.at(1) < bList.at(1))
        {
            return -1;
        }
        else
        {
            if( aList.at(2).toInt() > bList.at(2).toInt() )
            {
                return 1;
            }
            else if( aList.at(2) < bList.at(2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}

void WaitingForSync::getUpdateXml()
{
    QFile updateFile("update.xml");

    if (!updateFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "update.xml not exist.";
        updateFile.close();
        return;
    }
    else
    {
        if( !localXml.setContent(&updateFile))
        {
            qDebug() << "localxml setcontent false";
            updateFile.close();
            return;
        }

        updateFile.close();
    }

    QString serverPath = getXmlElementText( localXml, "UpdateServer");
qDebug() << "serverPath : " << serverPath;
    reply = qnam.get( QNetworkRequest(QUrl(serverPath + "/update.xml")));
    connect(reply, SIGNAL(finished()),this, SLOT(httpFinished()));
}

void WaitingForSync::httpFinished()
{
    if( !serverXml.setContent( reply->readAll()) )
    {
        qDebug() << "get serverXml error.";
        return;
    }

    if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "Version")) != -1)
    {
        qDebug() << "Already newest.";
        return;
    }
    else
    {
        // 由于3.0.4的uac问题 Update条目改为小写的u
        if( compareVersion( getXmlElementText( localXml, "update"), getXmlElementText( serverXml, "update")) == -1)
        {
            // 如果GOPWalletUpdate2.exe 需要更新  由于3.0.4的uac问题 改为GOPWalletUpdate2.exe
            updateExe = new QFile("GOPWalletUpdate2.exe.update");
            if (!updateExe->open(QIODevice::WriteOnly|QIODevice::Truncate))
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("New GOPWalletUpdate2.exe error: %1").arg(updateExe->errorString()) );
                commonDialog.pop();
                delete updateExe;
                updateExe = NULL;
                return;
            }

            QString serverPath = getXmlElementText( localXml, "UpdateServer");
            reply = qnam.get( QNetworkRequest(QUrl(serverPath + "/GOPWalletUpdate2.exe")));
            connect(reply, SIGNAL(finished()),
                    this, SLOT(getUpdateExeFinished()));
            connect(reply, SIGNAL(readyRead()),
                    this, SLOT(getUpdateExeReadyRead()));

            updateOrNot = true;  // 下完 GOPWalletUpdate2.exe 前不进入钱包

        }
        else
        {
            // 如果GOPWalletUpdate2.exe 不需要更新
            if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "LastAvailableVersion")) == -1)
            {
                // 如果版本低于最后可运行版本 强制更新
                qDebug() << "Force updating:  " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
                emit closeGOP();
            }
            else
            {
                updateOrNot = true;
                CommonDialog commonDialog(CommonDialog::OkAndCancel);
                commonDialog.setText( tr("New version detected,\nupdate or not?"));
                bool yesOrNo = commonDialog.pop();
                if(  yesOrNo)
                {
                    qDebug() << "Force updating:  " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
                    emit closeGOP();
                }
                else
                {
                    updateOrNot = false;
                }

            }


        }

    }

}

QString WaitingForSync::getXmlElementText(QDomDocument& doc, QString name)
{
    QDomElement docElem = doc.documentElement();  //返回根元素
    QDomNode n = docElem.firstChild();   //返回根节点的第一个子节点
    while(!n.isNull())   //如果节点不为空
    {
        if (n.isElement())  //如果节点是元素
        {
            QDomElement e = n.toElement();  //将其转换为元素

            if( e.tagName() == name)
            {
                return e.text();
            }
        }
        n = n.nextSibling();  //下一个兄弟节点
    }

    return "";
}

void WaitingForSync::updateBebuildInfo()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString str = gbkCodec->toUnicode(Fry::getInstance()->proc->readAll());
    qDebug() << "process running state: " << Fry::getInstance()->proc->state();
    qDebug() <<  "updateBebuildInfo " << str;
    if( str.contains("Replaying blockchain... Approximately ") && !str.contains("Successfully replayed"))
    {
        QStringList strList  = str.split("Replaying blockchain... Approximately ");
        QString percent = strList.last();
        percent = percent.mid(0, percent.indexOf("complete."));
        qDebug() << percent;
        ui->loadingLabel->setText( tr("Rebuilding... ") + percent );
    }

    if( Fry::getInstance()->proc->state() == QProcess::NotRunning)
    {
        QMessageBox::warning(NULL, "warning", QString::fromLocal8Bit("Ti_Value.exe 进程退出!"), QMessageBox::Ok);

        emit closeGOP();
        return;
    }

    QFile file( Fry::getInstance()->walletConfigPath + "/launchLog.txt");
    if( !file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << "open launchLog.txt failed: " << file.errorString();
        return;
    }

    QTextStream ts( &file);
    ts << "================== " + QTime::currentTime().toString() + " ==================\r\n" ;
    ts << str << "\r\n";

    file.close();

}

void WaitingForSync::getUpdateExeFinished()
{
    updateExe->flush();
    updateExe->close();

    if (reply->error())
    {
        // 如果 error
        updateExe->remove();
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText( tr("Get GOPWalletUpdate2.exe error: %1").arg(reply->errorString()) );
        commonDialog.pop();
    }
    else
    {
        // 下载完成后 删除原来的文件 重命名临时文件
        QString fileName = updateExe->fileName();
        QString oldName = fileName;
        oldName.remove(".update");

        qDebug() << "remove exe" << oldName <<  QFile::remove(oldName);
        qDebug() << "rename exe" << QFile::rename( fileName, oldName);

    }

    delete updateExe;
    updateExe = NULL;

    if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "LastAvailableVersion")) == -1)
    {
        // 如果版本低于最后可运行版本 强制更新
        qDebug() << "Force updating:  " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
        emit closeGOP();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr("New version detected,\nupdate or not?"));
        bool yesOrNo = commonDialog.pop();
        if( yesOrNo)
        {
            qDebug() << "start update " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));
            emit closeGOP();
        }
        else
        {
            updateOrNot = false;
        }
    }


}

void WaitingForSync::getUpdateExeReadyRead()
{
    if (updateExe)
        updateExe->write(reply->readAll());
}

//void WaitingForSync::hasSomethingToRead()
//{
//    qDebug() << Fry::getInstance()->proc->readAll();
//}
