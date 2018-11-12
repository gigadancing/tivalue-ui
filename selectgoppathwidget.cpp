#include "selectgoppathwidget.h"
#include "ui_selectgoppathwidget.h"
#include "tic.h"
#include "rpcthread.h"
#include <QPainter>
#include "commondialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>

SelectGopPathWidget::SelectGopPathWidget(QWidget *parent) :
    QWidget(parent),
    updateExe(NULL),
    reply(NULL),
    ui(new Ui::SelectGopPathWidget)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/ticpic/bg.png")));
    setPalette(palette);

    ui->pathLineEdit->setText( Fry::getInstance()->appDataPath);
    ui->pathLineEdit->setStyleSheet("color:white;background:transparent;border-width:0;border-style:outset;");

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/minimize2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                              "QToolButton:hover{background-image:url(:/pic/pic2/minimize_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic/pic2/close_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");


    getUpdateXml();
}

SelectGopPathWidget::~SelectGopPathWidget()
{
    delete ui;
}

void SelectGopPathWidget::on_selectPathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path to store the blockchain"));
    if( !file.isEmpty())
    {
        file.replace("/","\\");
        ui->pathLineEdit->setText( file);
    }
}

void SelectGopPathWidget::on_okBtn_clicked()
{
    qDebug() << "fry path " << ui->pathLineEdit->text();
    Fry::getInstance()->proc->start("Ti_Value.exe",QStringList() << "--data-dir" << ui->pathLineEdit->text()
                                       << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number( RPC_PORT) << "--server");
    if( Fry::getInstance()->proc->waitForStarted())
    {
        mutexForConfigFile.lock();
        Fry::getInstance()->configFile->setValue("/settings/fryPath", ui->pathLineEdit->text());
        mutexForConfigFile.unlock();
        Fry::getInstance()->appDataPath = ui->pathLineEdit->text();
        emit enter();
    }
    else
    {
        qDebug() << "laungh Ti_Value.exe failed";
    }
}

void SelectGopPathWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::white);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(QPoint(348,290),QPoint(575,290));
}

void SelectGopPathWidget::on_minBtn_clicked()
{
    emit minimum();
}

void SelectGopPathWidget::on_closeBtn_clicked()
{
    emit closeGOP();
}

void SelectGopPathWidget::getUpdateXml()
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

void SelectGopPathWidget::httpFinished()
{
    if( !serverXml.setContent( reply->readAll()) )
    {
        qDebug() << "get serverXml error.";
        return;
    }

    if( compareVersion( getXmlElementText( localXml, "Version"), getXmlElementText( serverXml, "Version")) != -1)
    {
        qDebug() << "Update.xml already newest.";
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
//                qDebug() << "Force updating: " << QProcess::startDetached("GOPWalletUpdate2.exe");
                emit closeGOP();
            }
            else
            {
                // 否则选择是否更新
                updateOrNot = true;
                CommonDialog commonDialog(CommonDialog::OkAndCancel);
                commonDialog.setText( tr("New version detected,\nupdate or not?"));
                bool yesOrNo = commonDialog.pop();
                if( yesOrNo)
                {
                    qDebug() << "start update " << QDesktopServices::openUrl(QUrl::fromLocalFile("GOPWalletUpdate2.exe"));

//                    qDebug() << "start update " << QProcess::startDetached("GOPWalletUpdate2.exe");
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

QString SelectGopPathWidget::getXmlElementText(QDomDocument& doc, QString name)
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

void SelectGopPathWidget::getUpdateExeFinished()
{
    updateExe->flush();
    updateExe->close();

    if (reply->error() != QNetworkReply::NoError)
    {
        // 如果 error
        qDebug() << "getUpdateExe error: " << reply->errorString();
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

void SelectGopPathWidget::getUpdateExeReadyRead()
{
    if (updateExe)
        updateExe->write(reply->readAll());
}
