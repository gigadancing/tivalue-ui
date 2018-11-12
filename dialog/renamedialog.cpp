#include "renamedialog.h"
#include "ui_renamedialog.h"
#include "../tic.h"
#include "../tic_common_define.h"
#include "../rpcthread.h"

#include <QDebug>
#include <QMovie>

RenameDialog::RenameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameDialog)
{
    ui->setupUi(this);

//    Fry::getInstance()->appendCurrentDialogVector(this);
    setParent(Fry::getInstance()->mainFrame);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(45,47,51);border:1px groove rgb(54,56,60);}");

    ui->nameLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->nameLineEdit->setPlaceholderText( tr("Beginning with letter,letters or numbers"));
    ui->nameLineEdit->setTextMargins(8,0,0,0);
    ui->nameLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);



    ui->okBtn->setText(tr("Ok"));
    ui->okBtn->setEnabled(false);

    ui->cancelBtn->setText(tr("Cancel"));

    QRegExp regx("[a-z][a-z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->nameLineEdit->setValidator( validator );

    ui->nameLineEdit->setFocus();

    gif = new QMovie(":/pic/pic2/loading2.gif");
    gif->setScaledSize( QSize(18,18));
    ui->gifLabel->setMovie(gif);
    gif->start();
    ui->gifLabel->hide();
}

RenameDialog::~RenameDialog()
{
    delete ui;
//    Fry::getInstance()->removeCurrentDialogVector(this);
}

void RenameDialog::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
//    emit accepted();
}

void RenameDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
//    emit accepted();
}

QString RenameDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();

    if( yesOrNO == true)
    {
        return ui->nameLineEdit->text();
    }
    else
    {
        return "";
    }
}

bool isExistInWallet(QString);

void RenameDialog::on_nameLineEdit_textChanged(const QString &arg1)
{
    if( arg1.isEmpty())
    {
        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel2->setText("" );
        return;
    }

    QString addrName = arg1;

    if( ADDRNAME_MAX_LENGTH < addrName.size() )
    {

        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("More than 63 characters!") + "</font></body>" );

        return;
    }


    //检查地址名是否在钱包内已经存在
    if( isExistInWallet(addrName) )
    {
        ui->okBtn->setEnabled(false);
        ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr( "This name has been used") + "</font></body>" );
        return;
    }


    Fry::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_account_" + addrName, "blockchain_get_account", QStringList() << addrName ));
    ui->gifLabel->show();
}

void RenameDialog::on_nameLineEdit_returnPressed()
{
    if( !ui->okBtn->isEnabled())  return;

    on_okBtn_clicked();
}

void RenameDialog::jsonDataUpdated(QString id)
{
    if( id.mid(0,26) == "id_blockchain_get_account_")
    {
        // 如果跟当前输入框中的内容不一样，则是过时的rpc返回，不用处理
        if( id.mid(26) != ui->nameLineEdit->text())  return;
        QString result = Fry::getInstance()->jsonDataValue(id);
        ui->gifLabel->hide();

        if( result == "\"result\":null")
        {
            ui->okBtn->setEnabled(true);
            ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#67B667>" + tr( "You can use this name") + "</font></body>" );
        }
        else
        {
            ui->okBtn->setEnabled(false);
            ui->addressNameTipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr( "This name has been used") + "</font></body>" );
        }

        return;
    }

}
