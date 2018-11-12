//#include "dialogwithshadow.h"
//#include "../goopal.h"

//DialogWithShadow::DialogWithShadow(QWidget *parent):
//    QDialog(parent),
//    choice(2)
//{
//    Goopal::getInstance()->appendCurrentDialogVector(this);

//    resize(960,580);
//    setAttribute(Qt::WA_TranslucentBackground, true);
//    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

//    widget = new QWidget(this);
//    widget->setGeometry(0,0,960,580);
//    widget->setObjectName("widget");
//    widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");

//}

//DialogWithShadow::~DialogWithShadow()
//{
//    Goopal::getInstance()->removeCurrentDialogVector(this);
//}

//int DialogWithShadow::pop()
//{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec(QEventLoop::DialogExec);  //进入事件 循环处理，阻塞
//    return choice;
//}

