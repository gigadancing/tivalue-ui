#include "ui_uploadparadialog.h"
#include "uploadparadialog.h"
#include "tic.h"
#include <QRect>
#include <QMouseEvent>

QRegExp rx("^[0-9]+(.[0-9]{4})?$");

uploadParaDialog::uploadParaDialog(QWidget *parent) :
    QDialog(parent),
    bCancel(false),
    m_MousePressed(false),
    ui(new Ui::uploadParaDialog)
{
    ui->setupUi(this);
    setParent(Fry::getInstance()->mainFrame);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    //setAutoFillBackground(true);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor("#16181c"));
    //setPalette(palette);
    setStyleSheet("background-color:black;color:#666666");

    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();
    for (int i = size - 1; i > -1; i--)
    {
        QString accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
        ui->accountNameBox->addItem(accountName);
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();

    /**********************/
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->comboBox->addItems(QStringList()<<"1"<<"2"<<"3"<<"4");
    ui->comboBox_2->addItem("1");
    ui->lineEdit->setText("1");
    ui->lineEdit_2->setText("1");
    ui->lineEdit_3->setText("2");
    ui->lineEdit_4->setText("2");
    ui->lineEdit_3->setValidator(validator);
    ui->lineEdit_4->setValidator(validator);

    ui->lineEdit->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);

}

QString uploadParaDialog::getAccountName()
{
    return ui->accountNameBox->currentText();
}

QString uploadParaDialog::getBackUpNumber()
{
    return ui->lineEdit->text();
}

QString uploadParaDialog::getPayCycle()
{
    return ui->lineEdit_2->text();
}

QString uploadParaDialog::getStoragePrice()
{
    return ui->lineEdit_4->text();
}

void uploadParaDialog::mousePressEvent(QMouseEvent *event)
{
    QRect *rc = new QRect(5,5,900,150);//创建一个矩形区域

    if(rc->contains(this->mapFromGlobal(QCursor::pos()))==true)//确定是这个矩形区域按下
    {
        if(event->button()==Qt::LeftButton)
        {
            m_WindowPos = this->pos();
            m_MousePos = event->globalPos();
            this->m_MousePressed = true;
        }
    }
}

void uploadParaDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(m_MousePressed)
    {
        this->move(m_WindowPos+(event->globalPos()-m_MousePos));
    }
}

void uploadParaDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        this->m_MousePressed = false;
    }
}

int uploadParaDialog::run()
{
    move(600, 220);
    exec();
    if(bCancel)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

uploadParaDialog::~uploadParaDialog()
{
    delete ui;
}

void uploadParaDialog::on_pushButton_3_clicked()
{
    hide();
}

void uploadParaDialog::on_pushButton_4_clicked()
{
    bCancel = true;
    close();
}

//subtraction
void uploadParaDialog::on_pushButton_clicked()
{
    int filecount = ui->lineEdit->text().toInt() - 1;
    if(filecount >= 1)
    {
        ui->lineEdit->setText(QString("%1").arg(filecount));
    }
}

//add
void uploadParaDialog::on_pushButton_2_clicked()
{
    int filecount = ui->lineEdit->text().toInt() + 1;
    if(filecount <= 5)
    {
        ui->lineEdit->setText(QString("%1").arg(filecount));
    }
}

void uploadParaDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->lineEdit_2->setText(arg1);
}
