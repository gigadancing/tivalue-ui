#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "rpcthread.h"
#include "tic.h"
#include "commondialog.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QFile>


ExportDialog::ExportDialog( QString name, QWidget *parent) :
    QDialog(parent),
    accoutName(name),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

//    Fry::getInstance()->appendCurrentDialogVector(this);
    setParent(Fry::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(45,47,51);border:1px groove rgb(54,56,60);}");


    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->nameLabel->setText( tr("Export ") + accoutName + tr(" \'s private key to:"));

    QDir dir;
    QString path = dir.currentPath().replace("/", "\\");
    ui->pathLineEdit->setText( path + "\\" + accoutName + ".key");

    ui->pathLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->pathLineEdit->setTextMargins(8,0,0,0);
}

ExportDialog::~ExportDialog()
{
    delete ui;
//    Fry::getInstance()->removeCurrentDialogVector(this);
}

void ExportDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();

}

void ExportDialog::on_pathBtn_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this,tr( "Select the path"));
    if( !file.isEmpty())
    {
        file.replace("/","\\");
        ui->pathLineEdit->setText( file + "\\" + accoutName + ".key");
    }
}

void ExportDialog::on_cancelBtn_clicked()
{
    close();
//    emit accepted();
}

void ExportDialog::getPrivateKey()
{
//    RpcThread* rpcThread = new RpcThread;
//    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//    rpcThread->setLogin("a","b");
//    rpcThread->setWriteData( toJsonFormat( "id_wallet_dump_account_private_key_" + accoutName, "wallet_dump_account_private_key", QStringList() << accoutName << "0" ));
//    rpcThread->start();

    Fry::getInstance()->postRPC( toJsonFormat( "id_wallet_dump_account_private_key_" + accoutName, "wallet_dump_account_private_key", QStringList() << accoutName << "0" ));
}

void ExportDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_dump_account_private_key_" + accoutName)
    {
        QString result = Fry::getInstance()->jsonDataValue(id);

        if( result.mid(0,8) == "\"error\":")
        {
            qDebug() << "wallet_dump_account_private_key " + accoutName + " ERROR: " + result;
        }
        else
        {
            privateKey = result.mid(10,51);
            QByteArray ba = privateKey.toUtf8();

            QFile file( ui->pathLineEdit->text());

            QString fName = file.fileName();
            fName.replace("/","\\");
            fName = fName.mid( fName.lastIndexOf("\\") + 1);

            if( file.exists())
            {
                CommonDialog tipDialog(CommonDialog::OkAndCancel);
                tipDialog.setText( fName + tr( " already exists!\nCover it or not?") );
                if ( !tipDialog.pop())  return;
            }

            if( !file.open(QIODevice::WriteOnly))
            {
                qDebug() << "privatekey file open " + fName + " ERROR";

                CommonDialog tipDialog(CommonDialog::OkOnly);
                tipDialog.setText( tr( "Wrong path!") );
                tipDialog.pop();
                return;
            }

            file.resize(0);
            QTextStream ts( &file);
            ts << ba.toBase64();
            file.close();

            close();
//            emit  accepted();

            CommonDialog tipDialog(CommonDialog::OkOnly);
            tipDialog.setText( tr( "Export to ") + fName + tr(" succeeded!") );
            tipDialog.pop();

            QString path = ui->pathLineEdit->text();
            path.replace( "/", "\\");
            path = path.left( path.lastIndexOf("\\") );

            QProcess::startDetached("explorer \"" + path + "\"");
        }


        return;
    }
}

void ExportDialog::on_exportBtn_clicked()
{
    if( !ui->pathLineEdit->text().endsWith(".key"))
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("Wrong file format!"));
        commonDialog.pop();
        return;
    }

    getPrivateKey();
}
