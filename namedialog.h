#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include <QDialog>

namespace Ui {
class NameDialog;
}

class NameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameDialog(QWidget *parent = 0);
    ~NameDialog();

    QString pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_returnPressed();

    void jsonDataUpdated(QString id);

private:
    Ui::NameDialog *ui;
    bool yesOrNO;
    QMovie* gif;
};

#endif // NAMEDIALOG_H
