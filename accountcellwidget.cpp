#include "accountcellwidget.h"
#include "ui_accountcellwidget.h"
#include "tic.h"

#include <QDebug>
#include <QTableWidget>

AccountCellWidget::AccountCellWidget( QString name, int r, int c, QWidget *parent) :
    QWidget(parent),
    accountName(name),
    row(r),
    column(c),
    ui(new Ui::AccountCellWidget)
{
    ui->setupUi(this);

    if( name.size() > 10) {//如果name太长 显示省略号
        ui->nameLabel->setText(name.mid(0, 10) + "...");
    } else {
        ui->nameLabel->setText(name);
    }

    ui->nameLabel->adjustSize();
    ui->identityLabel->move(ui->nameLabel->x() + ui->nameLabel->width() + 8, ui->nameLabel->y() - 2);
    QString language = Fry::getInstance()->language;
    if (language.isEmpty() || language == "Simplified Chinese") {
        registeredLabelString = "pic2/registeredLabel.png";
    } else {
        registeredLabelString = "pic2/registeredLabel_En.png";
    }

    // 如果是已注册账户
    if (Fry::getInstance()->registerMapValue(accountName) != "NO" && !Fry::getInstance()->registerMapValue(accountName).isEmpty()) {
        ui->identityLabel->setPixmap(QPixmap(registeredLabelString));
    } else {
        ui->identityLabel->setPixmap(QPixmap(""));
    }
}

AccountCellWidget::~AccountCellWidget()
{
    delete ui;
}

void AccountCellWidget::setBackgroundColor(int r, int g, int b, int a)
{
    QString colorString = "rgb(" + QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(a) + ")";
    ui->widget->setStyleSheet( "background-color:" + colorString + ";");
}

void AccountCellWidget::enterEvent(QEvent *e)
{
    emit cellEnter(row,column);
}

