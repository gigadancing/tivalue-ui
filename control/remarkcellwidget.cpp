#include "remarkcellwidget.h"
#include "ui_remarkcellwidget.h"

#include <QDebug>

RemarkCellWidget::RemarkCellWidget(QString str, QWidget *parent) :
    QWidget(parent),
    ss(str),
    ui(new Ui::RemarkCellWidget)
{
    ui->setupUi(this);

    ui->label->setText( ss);
}

RemarkCellWidget::~RemarkCellWidget()
{
    delete ui;
}

QString RemarkCellWidget::text()
{
    return ss;
}
