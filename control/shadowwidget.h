#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class ShadowWidget;
}

class ShadowWidget: public QWidget
{
    Q_OBJECT
public:
    ShadowWidget(QWidget *parent = 0);
    ~ShadowWidget();

    void init(QSize size);

    void retranslator();

private:
    Ui::ShadowWidget *ui;
    QMovie* gif;
};

#endif // SHADOWWIDGET_H
