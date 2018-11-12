#include "functionbar.h"
#include "ui_functionbar.h"
#include "tic.h"
#include "commondialog.h"
#include "debug_log.h"
#include "consoledialog.h"

#include <QPainter>
#include <QDebug>


FunctionBar::FunctionBar(QWidget *parent) :
    QWidget(parent),
    bStorage(false),
    ui(new Ui::FunctionBar)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(25,27,31));
    setPalette(palette);

    ui->accountBtn->setIconSize(QSize(167,40));
    ui->transferBtn->setIconSize(QSize(167,40));
    ui->contactBtn->setIconSize(QSize(167,40));

    ui->ControlPlatBtn->setIconSize(QSize(167,40));
    ui->StorageServiceBtn_2->setIconSize(QSize(167,40));
    ui->UploadFileBtn_2->setIconSize(QSize(167,40));
    ui->StorageFileBtn_4->setIconSize(QSize(167,40));
    ui->DownloadFileBtn->setIconSize(QSize(167,40));

    ui->accountBtn->setStyleSheet("background:transparent;border:none;");
    ui->transferBtn->setStyleSheet("background:transparent;border:none;");
    ui->contactBtn->setStyleSheet("background:transparent;border:none;");

    ui->ControlPlatBtn->setStyleSheet("background:transparent;border:none;");
    ui->StorageServiceBtn_2->setStyleSheet("background:transparent;border:none;");
    ui->UploadFileBtn_2->setStyleSheet("background:transparent;border:none;");
    ui->StorageFileBtn_4->setStyleSheet("background:transparent;border:none;");
    ui->DownloadFileBtn->setStyleSheet("background:transparent;border:none;");

    ui->UploadFileBtn_2->hide();
    ui->StorageFileBtn_4->hide();
    ui->DownloadFileBtn->hide();

    choosePage(1);

    DLOG_QT_WALLET_FUNCTION_END;
}

FunctionBar::~FunctionBar()
{
    delete ui;
}

void FunctionBar::on_accountBtn_clicked()
{
    choosePage(1);
    emit showMainPage();
}

void FunctionBar::on_transferBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = Fry::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size != 0)   // 有至少一个账户
    {
        choosePage(2);
        emit showTransferPage();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No account for transfering,\nadd an account first"));
        commonDialog.move( this->mapToGlobal(QPoint(318,150)));
        commonDialog.pop();
        on_accountBtn_clicked();
    }
}

void FunctionBar::on_contactBtn_clicked()
{
    choosePage(3);
    emit showContactPage();
}

void FunctionBar::on_StorageServiceBtn_2_clicked()
{
    choosePage(7);
    if(bStorage == false)
    {
        ui->UploadFileBtn_2->show();
        ui->StorageFileBtn_4->show();
        ui->DownloadFileBtn->show();
        bStorage = true;
    }
    else
    {
        ui->UploadFileBtn_2->hide();
        ui->StorageFileBtn_4->hide();
        ui->DownloadFileBtn->hide();
        ui->UploadFileBtn_2->setIcon(QIcon(":/pic/ticpic/uploadfile.png"));
        ui->StorageFileBtn_4->setIcon(QIcon(":/pic/ticpic/storage.png"));
        ui->DownloadFileBtn->setIcon(QIcon(":/pic/ticpic/downloadfile.png"));
        bStorage = false;
    }

}

void FunctionBar::on_ControlPlatBtn_clicked()
{
    choosePage(8);
    ConsoleDialog consoleDialog;
    consoleDialog.pop();
}

void FunctionBar::on_UploadFileBtn_2_clicked()
{
    choosePage(4);
    emit showUploadPage();
}

void FunctionBar::on_StorageFileBtn_4_clicked()
{
    choosePage(5);
    emit showStorageFilePage();
}

void FunctionBar::on_DownloadFileBtn_clicked()
{
    choosePage(6);
    emit showDownloadFilePage();
}

void FunctionBar::choosePage(int pageIndex)
{

    switch (pageIndex) {
    case 1:
        ui->accountBtn->setIcon(QIcon(":/pic/ticpic/accountBtn.png"));

        ui->transferBtn->setIcon(QIcon(":/pic/ticpic/transferBtn_unselected.png"));

        ui->contactBtn->setIcon(QIcon(":/pic/ticpic/contactBtn_unselected.png"));

        ui->ControlPlatBtn->setIcon(QIcon(":/pic/ticpic/control.png"));
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        ui->UploadFileBtn_2->setIcon(QIcon(":/pic/ticpic/uploadfile.png"));
        ui->StorageFileBtn_4->setIcon(QIcon(":/pic/ticpic/storage.png"));
        ui->DownloadFileBtn->setIcon(QIcon(":/pic/ticpic/downloadfile.png"));
        break;
    case 2:
        ui->accountBtn->setIcon(QIcon(":/pic/ticpic/accountBtn_unselected.png"));
        ui->transferBtn->setIcon(QIcon(":/pic/ticpic/transferBtn.png"));
        ui->contactBtn->setIcon(QIcon(":/pic/ticpic/contactBtn_unselected.png"));

        ui->ControlPlatBtn->setIcon(QIcon(":/pic/ticpic/control.png"));
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        break;
    case 3:
        ui->accountBtn->setIcon(QIcon(":/pic/ticpic/accountBtn_unselected.png"));
        ui->transferBtn->setIcon(QIcon(":/pic/ticpic/transferBtn_unselected.png"));
        ui->contactBtn->setIcon(QIcon(":/pic/ticpic/contactBtn.png"));
        ui->ControlPlatBtn->setIcon(QIcon(":/pic/ticpic/control.png"));
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        break;
    case 4:
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        ui->UploadFileBtn_2->setIcon(QIcon(":/pic/ticpic/uploadfile_clicked.png"));
        ui->StorageFileBtn_4->setIcon(QIcon(":/pic/ticpic/storage.png"));
        ui->DownloadFileBtn->setIcon(QIcon(":/pic/ticpic/downloadfile.png"));
        //todo
        break;
    case 5:
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        ui->UploadFileBtn_2->setIcon(QIcon(":/pic/ticpic/uploadfile.png"));
        ui->StorageFileBtn_4->setIcon(QIcon(":/pic/ticpic/storage_clicked.png"));
        ui->DownloadFileBtn->setIcon(QIcon(":/pic/ticpic/downloadfile.png"));
        //todo
        break;
    case 6:
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        ui->UploadFileBtn_2->setIcon(QIcon(":/pic/ticpic/uploadfile.png"));
        ui->StorageFileBtn_4->setIcon(QIcon(":/pic/ticpic/storage.png"));
        ui->DownloadFileBtn->setIcon(QIcon(":/pic/ticpic/downloadfile_clicked.png"));
        //todo
        break;
    case 7:
        ui->ControlPlatBtn->setIcon(QIcon(":/pic/ticpic/control.png"));
        ui->accountBtn->setIcon(QIcon(":/pic/ticpic/accountBtn_unselected.png"));
        ui->transferBtn->setIcon(QIcon(":/pic/ticpic/transferBtn_unselected.png"));
        ui->contactBtn->setIcon(QIcon(":/pic/ticpic/contactBtn_unselected.png"));
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service_clicked.png"));
        break;
    case 8:
        ui->ControlPlatBtn->setIcon(QIcon(":/pic/ticpic/control_clicked.png"));
        ui->accountBtn->setIcon(QIcon(":/pic/ticpic/accountBtn_unselected.png"));
        ui->transferBtn->setIcon(QIcon(":/pic/ticpic/transferBtn_unselected.png"));
        ui->contactBtn->setIcon(QIcon(":/pic/ticpic/contactBtn_unselected.png"));
        ui->StorageServiceBtn_2->setIcon(QIcon(":/pic/ticpic/storage_service.png"));
        break;
    default:
        break;
    }
}

void FunctionBar::retranslator()
{
    ui->retranslateUi(this);
}

