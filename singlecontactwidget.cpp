#include "singlecontactwidget.h"
#include "ui_singlecontactwidget.h"
#include <QDebug>
#include "tic.h"
#include "commondialog.h"
#include <QPainter>

SingleContactWidget::SingleContactWidget(int num,QString address,QString remarkName,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleContactWidget)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(22,24,28));
    setPalette(palette);

    ui->deleteBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close3.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
//    ui->deleteBtn->setToolTip(QString::fromLocal8Bit("删除联系人"));
    ui->deleteBtn->setToolTip(tr("Delete this contact"));

    ui->transferBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/transfer2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->editBtn->setStyleSheet("QToolButton{background-image:url(:/pic/ticpic/edit.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->addressLabel->setText(address);
    ui->remarkLabel->setText(remarkName);
    number = num;

    ui->remarkLabel->adjustSize();
    ui->editBtn->move( ui->remarkLabel->x() + ui->remarkLabel->width() + 10, 16);
    ui->addressLabel->adjustSize();
}

SingleContactWidget::~SingleContactWidget()
{
    delete ui;
}

void SingleContactWidget::on_deleteBtn_clicked()
{
    CommonDialog commonDialog(CommonDialog::OkAndCancel);
    commonDialog.setText(tr("Sure to delete this contact?"));

    bool yOrN = !commonDialog.pop();

    if( yOrN)
    {
        return;
    }

    if( !Fry::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
    }
    QByteArray rd = Fry::getInstance()->contactsFile->readAll();
    QString str =  QByteArray::fromBase64( rd);
    QStringList strList = str.split(";");
    strList.removeLast();
    strList.removeAt(number);
    int size = strList.size();

    QString str2;
    for( int i = 0; i < size; i++)
    {
        str2 += strList.at(i) + ';';
    }
    QByteArray ba = str2.toUtf8();
    ba = ba.toBase64();
    Fry::getInstance()->contactsFile->resize(0);
    QTextStream ts(Fry::getInstance()->contactsFile);
    ts << ba;

    Fry::getInstance()->contactsFile->close();

    emit deleteContact();
}


void SingleContactWidget::enterEvent(QEvent *)
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(37,39,43));
    setPalette(palette);
}


void SingleContactWidget::leaveEvent(QEvent *)
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(22,24,28));
    setPalette(palette);
}

void SingleContactWidget::on_transferBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = Fry::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();

    if( size == 0)   // 还没有账户
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
//        commonDialog.setText(QString::fromLocal8Bit("没有可以转账的账户"));
        commonDialog.setText(tr("No account for transfering."));
        commonDialog.pop();
        return;
    }
    emit  gotoTransferPage(ui->addressLabel->text());
}

void SingleContactWidget::on_editBtn_clicked()
{
    emit editContact( ui->addressLabel->text(), ui->remarkLabel->text());
}
