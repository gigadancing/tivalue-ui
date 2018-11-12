#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <windows.h>

#include "firstlogin.h"
#include "ui_firstlogin.h"
#include "tic.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "commondialog.h"

FirstLogin::FirstLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirstLogin)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    connect(Fry::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/ticpic/bg.png")));
    setPalette(palette);

    ui->pwdLineEdit->setStyleSheet("color:white;background:transparent;border-width:0;border-style:outset;");
    ui->pwdLineEdit2->setStyleSheet("color:white;background:transparent;border-width:0;border-style:outset;");

    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->installEventFilter(this);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit2->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);


    QFont font = ui->pwdLabel->font();
    font.setPixelSize(12);
    ui->pwdLabel->setFont(font);
    ui->pwdLabel2->setFont(font);

    font = ui->tipLabel->font();
    font.setPixelSize(12);
    ui->tipLabel->setFont(font);

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/minimize2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                              "QToolButton:hover{background-image:url(:/pic/pic2/minimize_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic/pic2/close_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->correctLabel->setPixmap( QPixmap(":/pic/pic2/correct2.png"));
    ui->correctLabel2->setPixmap( QPixmap(":/pic/pic2/correct2.png"));
    ui->correctLabel->hide();
    ui->correctLabel2->hide();



    if( GetKeyState(VK_CAPITAL) )
    {
        ui->tipLabel->setText( tr("Caps lock opened!") );
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

FirstLogin::~FirstLogin()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;

    DLOG_QT_WALLET_FUNCTION_END;
}

void FirstLogin::on_createBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit2->text().isEmpty() )
    {
        ui->tipLabel->setText(tr("Empty!"));
        return;
    }

    if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text())
    {
        if( ui->pwdLineEdit->text().size() < 8)
        {
            ui->tipLabel->setText(tr("Too short!"));
            return;
        }

        emit showShadowWidget();
        ui->pwdLineEdit->setEnabled(false);
        ui->pwdLineEdit2->setEnabled(false);

//        RpcThread* rpcThread = new RpcThread;
//        connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
//        rpcThread->setLogin("a","b");
//        rpcThread->setWriteData( toJsonFormat( "id_create", "create", QStringList() << "wallet" << ui->pwdLineEdit->text() ));
//        rpcThread->start();

        Fry::getInstance()->postRPC( toJsonFormat( "id_create", "create", QStringList() << "wallet" << ui->pwdLineEdit->text() ));
    }
    else
    {
        ui->tipLabel->setText(tr("not consistent!"));
        return;
    }

    DLOG_QT_WALLET_FUNCTION_END;
}


void FirstLogin::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor(137,176,226));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(QPoint(367,217),QPoint(595,217));
    painter.drawLine(QPoint(367,289),QPoint(595,289));

}



void FirstLogin::on_pwdLineEdit2_returnPressed()
{
    on_createBtn_clicked();
}

void FirstLogin::on_minBtn_clicked()
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

void FirstLogin::on_closeBtn_clicked()
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

bool FirstLogin::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->pwdLineEdit)
    {
        if(e->type() == QEvent::FocusIn)
        {
        }
        else if( e->type() == QEvent::FocusOut)
        {
        }
    }

    return QWidget::eventFilter(watched,e);

}

void FirstLogin::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

    if( arg1.size() > 7 )
    {
        ui->correctLabel->show();

        if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() )
        {
            ui->correctLabel2->show();
        }
        else
        {
            ui->correctLabel2->hide();
        }
    }
    else
    {
        ui->correctLabel->hide();
        ui->correctLabel2->hide();
    }

}

void FirstLogin::on_pwdLineEdit2_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit2->setText( ui->pwdLineEdit2->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

    if( arg1.size() > 7 && ( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() ) )
    {
        ui->correctLabel2->show();
    }
    else
    {
        ui->correctLabel2->hide();
    }
}

void FirstLogin::jsonDataUpdated(QString id)
{
    if( id == "id_create")
    {
        QString result = Fry::getInstance()->jsonDataValue(id);
        qDebug() << "id_create" << result;
        if( result == "\"result\":null" )
        {
            Fry::getInstance()->postRPC( toJsonFormat( "id_unlock_firstlogin", "unlock", QStringList() << "99999999" << ui->pwdLineEdit->text() ));
qDebug() << "id_unlock_firstlogin";
            return;
        }
        else
        {
            emit hideShadowWidget();
            return;
        }

    }

    if( id == "id_unlock_firstlogin")
    {
        emit hideShadowWidget();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit2->setEnabled(true);

        qDebug() << "id_unlock_firstlogin" << Fry::getInstance()->jsonDataValue(id);
        if( Fry::getInstance()->jsonDataValue(id) == "\"result\":null")
        {
            emit login();
            this->close();
        }
        return;
    }

}

void FirstLogin::keyPressEvent(QKeyEvent *e)
{
    if( e->key() == Qt::Key_CapsLock)
    {
        if( GetKeyState(VK_CAPITAL) == -127 )  // 不知道为什么跟构造函数中同样的调用返回的short不一样
        {
            ui->tipLabel->setText( tr("Caps lock opened!") );
        }
        else
        {

            ui->tipLabel->setText( tr("Caps lock closed!") );
        }

    }

    QWidget::keyPressEvent(e);
}
