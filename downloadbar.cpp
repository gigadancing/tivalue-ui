#include "downloadbar.h"
#include "ui_downloadbar.h"

#include "tic.h"
#include "rpcthread.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>

DownloadBar::DownloadBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadBar)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#191b1f"));
    setPalette(palette);

    ui->toolButton->setIconSize(QSize(94, 26));
    ui->toolButton->setStyleSheet("background:transparent;border:none;");
    ui->toolButton->setIcon(QIcon(":/pic/ticpic/downloadfileBtn.png"));
}

DownloadBar::~DownloadBar()
{
    delete ui;
}

void DownloadBar::on_toolButton_clicked()
{
    emit downloadFile();
}
