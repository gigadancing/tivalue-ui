#include "incomecellwidget.h"
#include "ui_incomecellwidget.h"

#include <QMouseEvent>
#include <QDebug>

IncomeCellWidget::IncomeCellWidget(QString name, QWidget *parent) :
    QWidget(parent),
    accountName(name),
    ui(new Ui::IncomeCellWidget)
{
    ui->setupUi(this);

    ui->label->setToolTip(tr("Click to withdraw salary."));
}


IncomeCellWidget::~IncomeCellWidget()
{
    delete ui;
}

QString IncomeCellWidget::text()
{
    return ui->label->text();
}

void IncomeCellWidget::setText(QString text)
{
    salary = text;
    if( text != "--")
    {
        ui->label->setText( "<body><font color=#409aff>" + text + "</font></body>");
    }
    else
    {
        ui->label->setText(text);
    }
}

void IncomeCellWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
     {
        if( salary != "--")
        {
            emit withdrawSalary(accountName,salary);
        }
     }
}
