#ifndef EDITREMARKDIALOG_H
#define EDITREMARKDIALOG_H

#include <QDialog>

namespace Ui {
class EditRemarkDialog;
}

class EditRemarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditRemarkDialog( QString remark, QWidget *parent = 0);
    ~EditRemarkDialog();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_remarkLineEdit_textChanged(const QString &arg1);

private:
    Ui::EditRemarkDialog *ui;
};

#endif // EDITREMARKDIALOG_H
