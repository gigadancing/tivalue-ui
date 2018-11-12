#include "consoledialog.h"
#include "ui_consoledialog.h"
#include "tic.h"
#include "debug_log.h"
#include "rpcthread.h"

#include <QTextCodec>
#include <QPainter>
#include <QKeyEvent>

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    cmdIndex(0),
    ui(new Ui::ConsoleDialog)
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

    ui->containerWidget->installEventFilter(this);
    ui->consoleLineEdit->installEventFilter(this);

    connect( Fry::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/close2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->consoleLineEdit->setStyleSheet("background:rgb(37,39,43);color:rgb(190,190,190);border:none;");
    ui->consoleLineEdit->setTextMargins(8,0,0,0);
    ui->consoleBrowser->setStyleSheet("QTextBrowser{background:rgb(37,39,43);color:rgb(190,190,190);border:none;}");

    ui->consoleLineEdit->setFocus();

    ui->checkBox->setStyleSheet("QCheckBox{color:rgb(190,190,190);}"
                                "QCheckBox::indicator{ image:url(:/pic/ticpic/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic/ticpic/checkBox_checked.png); }");

    setStyleSheet("#ConsoleDialog{background-color: rgb(45,47,51);}");

//    mouse_press = false;

//    ui->checkBox->hide();  // rpc选项隐藏
    DLOG_QT_WALLET_FUNCTION_END;
}

ConsoleDialog::~ConsoleDialog()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    qDebug() << "ConsoleDialog delete";
//    Fry::getInstance()->currentDialog = NULL;
//    Fry::getInstance()->removeCurrentDialogVector(this);
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}

void ConsoleDialog::pop()
{
//    QEventLoop loop;
//    show();
//    ui->consoleLineEdit->grabKeyboard();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

//void ConsoleDialog::paintEvent(QPaintEvent *)
//{
//    QPainter painter(this);
//    painter.setPen(QColor(204,204,204));
//    painter.drawLine(QPoint(0,65), QPoint(629,65));
//}

bool ConsoleDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
            QPainter painter(ui->containerWidget);
            painter.setPen(QPen(QColor(54,56,60),Qt::SolidLine));
            painter.drawLine(QPoint(0,35),QPoint(511,35));


            return true;
        }
    }

    if(watched == ui->consoleLineEdit)
    {
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* event = static_cast<QKeyEvent*>(e);
            if( event->key() == Qt::Key_Up)
            {
                cmdIndex--;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex < 0)
                {
                    cmdIndex = 0;
                }

            }
            else if( event->key() == Qt::Key_Down)
            {
                cmdIndex++;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex > cmdVector.size() - 1)
                {
                    cmdIndex = cmdVector.size() - 1;
                }

            }
        }

    }

    return QWidget::eventFilter(watched,e);
}

void ConsoleDialog::on_closeBtn_clicked()
{
    close();
//    emit accepted();
}


//void ConsoleDialog::mousePressEvent(QMouseEvent *event)
//{

//    if(event->button() == Qt::LeftButton)
//     {
//          mouse_press = true;
//          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
//          move_point = event->pos();;
//     }
//}

//void ConsoleDialog::mouseMoveEvent(QMouseEvent *event)
//{
//    //若鼠标左键被按下
//    if(mouse_press)
//    {
//        //鼠标相对于屏幕的位置
//        QPoint move_pos = event->globalPos();

//        //移动主窗体位置
//        this->move(move_pos - move_point);
//    }
//}

//void ConsoleDialog::mouseReleaseEvent(QMouseEvent *)
//{
//    mouse_press = false;
//}

void ConsoleDialog::on_consoleLineEdit_returnPressed()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( !ui->consoleLineEdit->text().simplified().isEmpty())
    {
        cmdVector.removeAll(ui->consoleLineEdit->text());
        cmdVector.append(ui->consoleLineEdit->text());
        cmdIndex = cmdVector.size();
    }

    if( ui->checkBox->isChecked())
    {
        QString str = ui->consoleLineEdit->text();
        QStringList paramaters = str.split(' ');
        QString command = paramaters.at(0);
        paramaters.removeFirst();
        if( paramaters.isEmpty())  paramaters << "";

        Fry::getInstance()->postRPC( toJsonFormat( "console_" + str, command, paramaters ));

        ui->consoleLineEdit->clear();
        return;
    }

    QString str = ui->consoleLineEdit->text() + '\n';
    Fry::getInstance()->write(str);
    QString result = Fry::getInstance()->read();
    ui->consoleBrowser->append(">>>" + str);
    ui->consoleBrowser->append(result);
    ui->consoleLineEdit->clear();
    DLOG_QT_WALLET_FUNCTION_END;
}

//void ConsoleDialog::setVisible(bool visiable)
//{
//    QWidget::setVisible(visiable);
//}

void ConsoleDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith("console_"))
    {
        ui->consoleBrowser->append(">>>" + id.mid(8));
        ui->consoleBrowser->append( Fry::getInstance()->jsonDataValue(id));
        ui->consoleBrowser->append("\n");
        return;
    }
}
