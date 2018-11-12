#include "rightclickmenudialog.h"
#include "ui_rightclickmenudialog.h"
#include "../tic.h"

#include "Windows.h"
#include <QDebug>

RightClickMenuDialog::RightClickMenuDialog( QString name, QWidget *parent) :
    QDialog(parent),
    accountName(name),
    ui(new Ui::RightClickMenuDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

    ui->transferBtn->setStyleSheet("QPushButton{background-color:rgb(37,39,43);color:rgb(235,235,235);border:1px solid rgb(102,102,102);border-radius:0px;}"
                                  "QPushButton:hover{background-color:rgb(57,59,63);color:#ffffff;border-radius:0px;}"
                                  );
    ui->renameBtn->setStyleSheet("QPushButton{background-color:rgb(37,39,43);color:rgb(235,235,235);border:1px solid rgb(102,102,102);border-radius:0px;}"
                                 "QPushButton:hover{background-color:rgb(57,59,63);color:#ffffff;border-radius:0px;}"
                                 );
    ui->exportBtn->setStyleSheet("QPushButton{background-color:rgb(37,39,43);color:rgb(235,235,235);border:1px solid rgb(102,102,102);border-radius:0px;}"
                                 "QPushButton:hover{background-color:rgb(57,59,63);color:#ffffff;border-radius:0px;}"
                                 );

    if( Fry::getInstance()->registerMapValue(name) != "NO" )  // 如果是已注册账户 不显示修改账户名选项
    {
        ui->renameBtn->hide();
        ui->exportBtn->move(0,35);
        setGeometry(0,0,116,71);
    }

}

RightClickMenuDialog::~RightClickMenuDialog()
{
    delete ui;
}

void RightClickMenuDialog::on_transferBtn_clicked()
{
    close();
    emit transferFromAccount(accountName);
}

void RightClickMenuDialog::on_renameBtn_clicked()
{
    close();
    emit renameAccount(accountName);
}

void RightClickMenuDialog::on_exportBtn_clicked()
{
    close();
    emit exportAccount(accountName);
}

bool RightClickMenuDialog::event(QEvent *event)
{
    // class_ameneded 不能是custommenu的成员, 因为winidchange事件触发时, 类成员尚未初始化
    static bool class_amended = false;
    if (event->type() == QEvent::WinIdChange)
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (class_amended == false)
        {
            class_amended = true;
            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
            class_style &= ~CS_DROPSHADOW;
            ::SetClassLong(hwnd, GCL_STYLE, class_style); // windows系统函数
        }
    }
    return QWidget::event(event);
}
