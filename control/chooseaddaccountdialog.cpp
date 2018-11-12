#include "chooseaddaccountdialog.h"
#include "ui_chooseaddaccountdialog.h"

#include "Windows.h"

ChooseAddAccountDialog::ChooseAddAccountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseAddAccountDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

}

ChooseAddAccountDialog::~ChooseAddAccountDialog()
{
    delete ui;
}

bool ChooseAddAccountDialog::event(QEvent *event)
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


void ChooseAddAccountDialog::on_newBtn_clicked()
{
    close();
    emit newAccount();
}

void ChooseAddAccountDialog::on_importBtn_clicked()
{
    close();
    emit importAccount();
}
