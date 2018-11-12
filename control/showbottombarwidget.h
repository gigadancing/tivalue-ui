#ifndef SHOWBOTTOMBARWIDGET_H
#define SHOWBOTTOMBARWIDGET_H

#include <QWidget>

class ShowBottomBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowBottomBarWidget(QWidget *parent = 0);
    ~ShowBottomBarWidget();

protected:
    void enterEvent(QEvent* e);

signals:
    void showBottomBar();

};

#endif // SHOWBOTTOMBARWIDGET_H
