#ifndef ADDNODEDIALOG_H
#define ADDNODEDIALOG_H

#include <QDialog>

namespace Ui {
class AddNodeDialog;
}

class AddNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNodeDialog(QWidget *parent = 0);
    ~AddNodeDialog();

private slots:
    void on_cancelBtn_clicked();

    void on_addBtn_clicked();

    void on_portLineEdit_returnPressed();

    void on_ipLineEdit_textChanged(const QString &arg1);

    void on_portLineEdit_textChanged(const QString &arg1);

private:
    Ui::AddNodeDialog *ui;
};

#endif // ADDNODEDIALOG_H
