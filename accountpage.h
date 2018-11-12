#ifndef ACCOUNTPAGE_H
#define ACCOUNTPAGE_H

#include <QWidget>

namespace Ui {
class AccountPage;
}

#define MODULE_ACCOUNT_PAGE "ACCOUNT_PAGE"

class AccountPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountPage(QString name,QWidget *parent = 0);
    ~AccountPage();

    void updateTransactionsList();

public slots:
    void refresh();
    void retranslator();

private slots:

    void on_copyBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void jsonDataUpdated(QString id);

    void on_transactionTypeComboBox_currentIndexChanged(int index);

    void on_accountTransactionsTableWidget_cellClicked(int row, int column);

    void on_prePageBtn_clicked();

    void on_nextPageBtn_clicked();

    void on_pageLineEdit_editingFinished();

    void on_pageLineEdit_textEdited(const QString &arg1);

    void on_preExportBtn_clicked();

    void on_queryBtn_clicked(bool clicked);




signals:
    void back();
    void showUpgradePage(QString);
    void accountChanged(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showApplyDelegatePage(QString);

private:
    Ui::AccountPage *ui;
    QString accountName;
    QString address;
    int transactionType;
    bool inited;
    int currentPageIndex;
    QString registeredLabelString;

    void paintEvent(QPaintEvent*);
    void init();
    void showTransactions(int type);

};

#endif // ACCOUNTPAGE_H
