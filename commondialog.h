#ifndef COMMONDIALOG_H
#define COMMONDIALOG_H

#include <QDialog>

namespace Ui {
class CommonDialog;
}

class CommonDialog : public QDialog
{
    Q_OBJECT

public:
    enum commonDialogType{OkAndCancel, OkOnly, YesOrNo};
    explicit CommonDialog( commonDialogType type,QWidget *parent = 0);
    ~CommonDialog();

    bool pop();
    void setText(QString text);

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::CommonDialog *ui;
    bool yesOrNO;
};

#endif // COMMONDIALOG_H
