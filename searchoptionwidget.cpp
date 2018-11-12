#include "searchoptionwidget.h"
#include "ui_searchoptionwidget.h"

SearchOptionWidget::SearchOptionWidget(QWidget *parent) :
    QWidget(parent),
    currentChoice(0),
    ui(new Ui::SearchOptionWidget)
{
    ui->setupUi(this);


    setStyleSheet("border:1px solid #cccccc;");

    ui->radioButton1->setStyleSheet("QRadioButton{color:rgb(255,255,255);border:none;}"
                                    "QRadioButton:hover{color:rgb(152,200,255);}");
    ui->radioButton2->setStyleSheet("QRadioButton{color:rgb(255,255,255);border:none;}"
                                    "QRadioButton:hover{color:rgb(152,200,255);}");
    ui->radioButton3->setStyleSheet("QRadioButton{color:rgb(255,255,255);border:none;}"
                                    "QRadioButton:hover{color:rgb(152,200,255);}");
    ui->widget->setFocusProxy(this);
//    ui->radioButton1->setFocusProxy(this);
//    ui->radioButton2->setFocusProxy(this);
//    ui->radioButton3->setFocusProxy(this);

    ui->radioButton1->setChecked(true);
}

SearchOptionWidget::~SearchOptionWidget()
{
    delete ui;
}

void SearchOptionWidget::on_radioButton1_clicked()
{
    currentChoice = 0;
}

void SearchOptionWidget::on_radioButton2_clicked()
{
    currentChoice = 1;
}

void SearchOptionWidget::on_radioButton3_clicked()
{
    currentChoice = 2;
}


