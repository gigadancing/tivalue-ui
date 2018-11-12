#include "storagefilebar.h"
#include "ui_storagefilebar.h"
#include "tic.h"
#include "rpcthread.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>

StorageFileBar::StorageFileBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageFileBar)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#191b1f"));
    setPalette(palette);

    ui->toolButton->setIconSize(QSize(94, 26));
    ui->toolButton->setStyleSheet("background:transparent;border:none;");
    ui->toolButton->setIcon(QIcon(":/pic/ticpic/applayUploadFile.png"));
}

StorageFileBar::~StorageFileBar()
{
    delete ui;
}


void StorageFileBar::on_toolButton_clicked()
{
    emit applayStorage();
}
