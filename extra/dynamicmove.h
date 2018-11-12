#ifndef DYNAMICMOVE_H
#define DYNAMICMOVE_H

#include <QWidget>
#include <QTimer>
#include <QTime>

class DynamicMove : public QObject
{
    Q_OBJECT
public:
    DynamicMove( QWidget* widget, QPoint destinationPosition, int intervalSecs, int numOfFrames, QWidget* parent = 0);
    ~DynamicMove();

    void start();

private slots:
    void step();

signals:
    void moveEnd();

private:
    QWidget* w;
    QTimer timer;
    int intervalTime;
    int number;  // 总帧数
    QPoint initialPosition;
    QPoint destination;
    int count;
};

#endif // DYNAMICMOVE_H
