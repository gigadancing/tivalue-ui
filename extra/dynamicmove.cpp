#include "dynamicmove.h"
#include <QDebug>

DynamicMove::DynamicMove( QWidget *widget, QPoint destinationPosition, int intervalSecs, int numOfFrames, QWidget *parent) :
    w(widget),
    destination(destinationPosition),
    intervalTime(intervalSecs),
    number(numOfFrames),
    QObject(parent)
{
    count = 1;
    initialPosition = w->pos();
}

DynamicMove::~DynamicMove()
{

}

void DynamicMove::start()
{
    timer.setInterval(intervalTime);
    connect( &timer, SIGNAL(timeout()), this, SLOT(step()));
    timer.start();
}

void DynamicMove::step()
{
    if( count > number)
    {
        timer.stop();
        emit moveEnd();
        return;
    }

    int x = initialPosition.x() + ( destination.x() - initialPosition.x() ) * 1.0 * count / number;
    int y = initialPosition.y() + ( destination.y() - initialPosition.y() ) * 1.0 * count / number;
    w->move(x,y);
    count++;
}



