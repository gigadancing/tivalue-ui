#include "myprogressbar.h"
#include <QDebug>

MyProgressBar::MyProgressBar(QWidget* parent):
    QWidget(parent)
{
    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    progressBar->show();
    setSize(222,6);

    signLabel = new QLabel(this);
    signLabel->setPixmap( QPixmap("pic2/progressSign.png"));
    signLabel->show();

    progressBar->setStyleSheet("QProgressBar{background-color: rgb(229,229,229);border-radius: 3px; border:0px solid rgb(0,0,0);}"
                  "QProgressBar::chunk:horizontal {background-color: rgb(64,154,255);margin: 0px 0px 0px 0px;border-radius: 3px;}"
                  );
}

MyProgressBar::~MyProgressBar()
{

}


void MyProgressBar::setSize(int width, int height)
{
    setGeometry(0,0,width + 4, 12);
    progressBar->setGeometry(2,3,width,6);
}

void MyProgressBar::setValue(int value)
{
    int previousValue = progressBar->value();
    progressBar->setValue(value);
    signLabel->move(  214.0/100*value  ,0);
    if( previousValue != value)
    {
        emit valueChanged(value);
    }
}

int MyProgressBar::value()
{
    return progressBar->value();
}
