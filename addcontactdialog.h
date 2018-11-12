#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = 0);
    ~AddContactDialog();

    void  pop();

private slots:
    void on_cancelBtn_clicked();

    void on_addressLineEdit_textChanged(const QString &arg1);

    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_remarkLineEdit_textChanged(const QString &arg1);

private:
    Ui::AddContactDialog *ui;
    QMovie* gif;

    bool eventFilter(QObject *watched, QEvent *e);
};

#endif // ADDCONTACTDIALOG_H
