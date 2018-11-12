#ifndef ACCOUNTDETAILWIDGET_H
#define ACCOUNTDETAILWIDGET_H

#include <QWidget>
#include "qrcodewidget.h"

namespace Ui {
class AccountDetailWidget;
}

class AccountDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountDetailWidget( QWidget *parent = 0);
    ~AccountDetailWidget();

    void setAccount(QString name);

    void showRecentTransactions();

    QString accountName;

public slots:
    void dynamicShow();
    void dynamicHide();

    void refresh();

private slots:
    void on_closeBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_moreTransactionBtn_clicked();

    void on_exportBtn_clicked();

    void on_transferBtn_clicked();

    void on_copyBtn_clicked();

    void on_upgradeBtn_clicked();

    void moveEnd();

signals:
    void back();

    void showAccountPage(QString name);

    void showTransferPage(QString name);

    void upgrade(QString);

    void applyDelegate(QString);

private:
    Ui::AccountDetailWidget *ui;
    double salary;
    QRCodeWidget* qrCodeWidget;
    bool produceOrNot;
    QString registeredLabelString;
};

#endif // ACCOUNTDETAILWIDGET_H
