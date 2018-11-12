#ifndef NEWSDIALOG_H
#define NEWSDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class NewsDialog;
}

class NewsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewsDialog(QWidget *parent = 0);
    ~NewsDialog();

    void pop();

private slots:
    void on_nextBtn_clicked();

    void on_closeBtn_clicked();

    void on_checkBtn_clicked();

signals:
    void showAccountPage(QString);

private:
    Ui::NewsDialog *ui;
    QMap<QString,int> recordIdTypeMap;
    int page;
    int totalNum;

    void showNews( QString id);
};

#endif // NEWSDIALOG_H
