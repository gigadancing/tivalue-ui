#include "showbottombarwidget.h"

ShowBottomBarWidget::ShowBottomBarWidget(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    setPalette(palette);
}

ShowBottomBarWidget::~ShowBottomBarWidget()
{

}

void ShowBottomBarWidget::enterEvent(QEvent *e)
{
    emit showBottomBar();
}

