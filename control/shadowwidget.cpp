#include "shadowwidget.h"
#include "ui_shadowwidget.h"

#include <QDebug>
#include <QMovie>

ShadowWidget::ShadowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShadowWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(10,10,10,100));
    setPalette(palette);

    ui->gifLabel->setGeometry(0,0,34,34);
    ui->gifLabel->show();

    gif = new QMovie(":/pic/pic2/loading2.gif");
    gif->setScaledSize( QSize(34,34));
    ui->gifLabel->setMovie(gif);
    gif->start();

    retranslator();

}

ShadowWidget::~ShadowWidget()
{

}

void ShadowWidget::init(QSize size)
{
    resize(size);
    ui->gifLabel->move( size.width() / 2 - 17, size.height() / 2 - 17);
    ui->tipLabel->move( ( this->width() - ui->tipLabel->width() ) / 2, 400);
}

void ShadowWidget::retranslator()
{
    ui->retranslateUi(this);
    ui->tipLabel->adjustSize();
    ui->tipLabel->move( ( this->width() - ui->tipLabel->width() ) / 2, 400);
}

