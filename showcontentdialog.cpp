#include "showcontentdialog.h"
#include "ui_showcontentdialog.h"
#include <QClipboard>

ShowContentDialog::ShowContentDialog(QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowContentDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);


    setStyleSheet("#ShowContentDialog{background-color:rgb(102,102,102);border:1px groove rgb(102,102,102);}");
    ui->textLabel->setStyleSheet("background-color:rgb102,102,102);color:rgb(190,190,190);");
    ui->textLabel->setText(text);
    ui->textLabel->adjustSize();

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/copyBtn.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));
    ui->copyBtn->move( 7, 3);
    ui->textLabel->move( 25, 1);

    setGeometry(0,0, ui->textLabel->width() + 30,20);
}

ShowContentDialog::~ShowContentDialog()
{
    delete ui;
}

void ShowContentDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText( ui->textLabel->text() );
    close();
}
