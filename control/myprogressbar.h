#ifndef MYPROGRESSBAR_H
#define MYPROGRESSBAR_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

class MyProgressBar : public QWidget
{
    Q_OBJECT
public:
    MyProgressBar(QWidget* parent = 0);
    ~MyProgressBar();

    void setSize(int width,int height);

    void setValue(int value);

    int value();

signals:
    void valueChanged(int);

private:
    QProgressBar* progressBar;
    QLabel* signLabel;

};

#endif // MYPROGRESSBAR_H
