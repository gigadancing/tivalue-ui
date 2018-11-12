#include "remarkdialog.h"
#include "ui_remarkdialog.h"
#include "tic.h"
#include <QTextStream>
#include <QDebug>
#include "debug_log.h"

RemarkDialog::RemarkDialog(QString address, QWidget *parent) :
    QDialog(parent),
    name(address),
    ui(new Ui::RemarkDialog)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

//    Fry::getInstance()->appendCurrentDialogVector(this);
    setParent(Fry::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(45,47,51);border:1px groove rgb(54,56,60);}");



    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->remarkLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);

    ui->remarkLineEdit->setFocus();
    DLOG_QT_WALLET_FUNCTION_END;
}

RemarkDialog::~RemarkDialog()
{
    delete ui;
//    Fry::getInstance()->removeCurrentDialogVector(this);
}

void RemarkDialog::pop()
{
//    QEventLoop loop;
//    show();
//    ui->remarkLineEdit->grabKeyboard();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void RemarkDialog::on_okBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    if( !Fry::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( Fry::getInstance()->contactsFile->readAll());
    ba += (name + '=' + ui->remarkLineEdit->text() + ";").toUtf8();
    ba = ba.toBase64();
    Fry::getInstance()->contactsFile->resize(0);
    QTextStream ts(Fry::getInstance()->contactsFile);
    ts << ba;

    Fry::getInstance()->contactsFile->close();

    close();
//    emit accepted();

    DLOG_QT_WALLET_FUNCTION_END;
}

void RemarkDialog::on_cancelBtn_clicked()
{
    close();
//    emit accepted();
}
