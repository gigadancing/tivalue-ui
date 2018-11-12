#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameDialog(QWidget *parent = 0);
    ~RenameDialog();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void jsonDataUpdated(QString id);

private:
    Ui::RenameDialog *ui;
    bool yesOrNO;
    QMovie* gif;
};

#endif // RENAMEDIALOG_H
