#include "addcontactdialog.h"
#include "ui_addcontactdialog.h"
#include "tic.h"
#include "rpcthread.h"
#include <QDebug>
#include <QMovie>
#include <QPainter>

AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
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

    ui->containerWidget->installEventFilter(this);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->addressLineEdit->setValidator( validator );

    ui->addressLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->addressLineEdit->setTextMargins(8,0,0,0);
    ui->addressLineEdit->setPlaceholderText( tr("Please enter an account address."));
    ui->addressLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->remarkLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);


    ui->okBtn->setEnabled(false);

    ui->addressLineEdit->setFocus();

    gif = new QMovie(":/pic/pic2/loading2.gif");
    gif->setScaledSize( QSize(18,18));
    ui->gifLabel->setMovie(gif);
    gif->start();
    ui->gifLabel->hide();
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
//    Fry::getInstance()->removeCurrentDialogVector(this);
}

void AddContactDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void AddContactDialog::on_cancelBtn_clicked()
{
    close();
//    emit accepted();
}

void AddContactDialog::on_addressLineEdit_textChanged(const QString &arg1)
{
    ui->addressLineEdit->setText( ui->addressLineEdit->text().remove(" "));
    ui->addressLineEdit->setText( ui->addressLineEdit->text().remove("\n"));
    if( ui->addressLineEdit->text().isEmpty())
    {
        ui->tipLabel->setText("");
        return;
    }

    if( ui->addressLineEdit->text().mid(0,2) == ASSET_NAME)
    {
        ui->tipLabel->setText("");
        ui->okBtn->setEnabled(true);
        return;
    }

    QString str;
    if( ui->addressLineEdit->text().toInt() == 0)   // 防止输入数字
    {
        str = ui->addressLineEdit->text();
    }
    else
    {
        str = "WRONG";
    }

//    RpcThread* rpcThread = new RpcThread;
//    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
////    rpcThread->setLogin("a","b");
//    rpcThread->setWriteData( toJsonFormat( "id_blockchain_get_account", "blockchain_get_account", QStringList() << str ));
//    rpcThread->start();

//    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_account", "blockchain_get_account", QStringList() << str ));
//    ui->gifLabel->show();
}

void AddContactDialog::jsonDataUpdated(QString id)
{
//    if( id != "id_blockchain_get_account")  return;

//    QString result = Fry::getInstance()->jsonDataValue( id);
//    ui->gifLabel->hide();

//    if( result == "\"result\":null")
//    {
//        ui->tipLabel->setText(tr("Invalid address"));
//        ui->tipLabel2->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
//        ui->okBtn->setEnabled(false);
//    }
//    else
//    {
//        ui->tipLabel->setText(tr("<body><font color=green>Valid address</font></body>"));
//        ui->tipLabel2->setPixmap(QPixmap(":/pic/pic2/correct.png"));
//        ui->okBtn->setEnabled(true);
//    }

}

void AddContactDialog::on_okBtn_clicked()
{
    if( ui->addressLineEdit->text().simplified().isEmpty())
    {
        ui->tipLabel->setText(tr("can not be empty"));
        return;
    }

    if( !Fry::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }


    QByteArray ba = QByteArray::fromBase64( Fry::getInstance()->contactsFile->readAll());
    QString str(ba);

    QStringList strList = str.split(";");
    strList.removeLast();
    foreach (QString ss, strList)
    {
        if( (ss.mid(0, ss.indexOf('=')) == ui->addressLineEdit->text())
//             && ss.mid( ss.indexOf('=') + 1) ==  ui->remarkLineEdit->text()
          )
        {
            ui->tipLabel->setText(tr("Already existed"));
            Fry::getInstance()->contactsFile->close();
            return;
        }
    }


    ba += ui->addressLineEdit->text().toUtf8() + '=' + ui->remarkLineEdit->text().toUtf8() + QString(";").toUtf8();
    ba = ba.toBase64();
    Fry::getInstance()->contactsFile->resize(0);
    QTextStream ts(Fry::getInstance()->contactsFile);
    ts << ba;

    Fry::getInstance()->contactsFile->close();

    close();
//    emit accepted();
}

void AddContactDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QString remark = arg1;
    if( remark.contains("=") || remark.contains(";"))
    {
        remark.remove("=");
        remark.remove(";");
        ui->remarkLineEdit->setText( remark);
    }
}


bool AddContactDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
//            QPainter painter(ui->containerWidget);
//            painter.setPen(QPen(QColor(122,112,110),Qt::SolidLine));
//            painter.setBrush(QBrush(QColor(122,112,110),Qt::SolidPattern));
//            painter.drawRect(0,0,756,50);

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}
