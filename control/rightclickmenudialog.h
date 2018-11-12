#ifndef RIGHTCLICKMENUDIALOG_H
#define RIGHTCLICKMENUDIALOG_H

#include <QDialog>

namespace Ui {
class RightClickMenuDialog;
}

class RightClickMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RightClickMenuDialog(QString name, QWidget *parent = 0);
    ~RightClickMenuDialog();

private slots:
    void on_transferBtn_clicked();

    void on_renameBtn_clicked();

    void on_exportBtn_clicked();

signals:
    void transferFromAccount(QString account);
    void renameAccount(QString account);
    void exportAccount(QString account);

private:
    Ui::RightClickMenuDialog *ui;
    QString accountName;

    bool event(QEvent *event);
};

#endif // RIGHTCLICKMENUDIALOG_H
