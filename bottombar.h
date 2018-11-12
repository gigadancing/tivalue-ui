#ifndef BOTTOMBAR_H
#define BOTTOMBAR_H

#include <QWidget>

namespace Ui {
class BottomBar;
}

class CommonTip;

class BottomBar : public QWidget
{
    Q_OBJECT

public:
    explicit BottomBar(QWidget *parent = 0);
    ~BottomBar();
    void retranslator();
    void refresh();

private slots:

    void updateNumOfConnections();

    void jsonDataUpdated(QString id);

private:
    Ui::BottomBar *ui;
    int numOfConnections;
    QTimer* timer;
    CommonTip* connectionTip;


};

#endif // BOTTOMBAR_H
