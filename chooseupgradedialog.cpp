#include "chooseupgradedialog.h"
#include "ui_chooseupgradedialog.h"
#include "fry.h"

#include "Windows.h"

ChooseUpgradeDialog::ChooseUpgradeDialog(QString name, QWidget *parent) :
    QDialog(parent),
    accountName(name),
    ui(new Ui::ChooseUpgradeDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);

    ui->upgradeBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:#62a9f8;border:1px solid rgb(239,239,239);border-radius:0px;}"
                                  "QPushButton:hover{background-color:#62a9f8;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:0px;}"
                                  );
    ui->applyDelegateBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:#62a9f8;border:1px solid rgb(239,239,239);border-radius:0px;}"
                                  "QPushButton:hover{background-color:#62a9f8;color:#ffffff;border:0px solid rgb(64,153,255);border-radius:0px;}"
                                  );

}

ChooseUpgradeDialog::~ChooseUpgradeDialog()
{
    delete ui;
}

bool ChooseUpgradeDialog::event(QEvent *event)
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

void ChooseUpgradeDialog::on_upgradeBtn_clicked()
{
    close();
    emit upgrade(accountName);
}

void ChooseUpgradeDialog::on_applyDelegateBtn_clicked()
{
    close();
    emit applyDelegate(accountName);
}

