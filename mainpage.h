#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class MainPage;
}

#define MODULE_MAIN_PAGE "MAIN_PAGE"

class AccountDetailWidget;

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = 0);
    ~MainPage();

    void updateAccountList();
    void retranslator(QString language);

public slots:
    void refresh();

signals:
    void openAccountPage(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showUpgradePage(QString);
    void showApplyDelegatePage(QString);
    void refreshAccountInfo();
    void showTransferPage(QString);
    void newAccount(QString);

private slots:
    void importAccount();

    void addAccount();

    void on_addAccountBtn_clicked();

    void on_accountTableWidget_cellClicked(int row, int column);

    void on_accountTableWidget_cellEntered(int row, int column);

    void jsonDataUpdated(QString id);

    void showExportDialog(QString name);

    void stopRefresh();

    void startRefresh();

    void withdrawSalary(QString name, QString salary);

    void renameAccount(QString name);

    void deleteAccount(QString name);

    void onTvRaBtnChecked();
    //acs
    void onAcsRaBtnChecked();
    //3dc
    void on3dcRaBtnChecked();

    // NBJ
    void onNbjRaBtnChecked();
    // NOT
    void onNotRaBtnChecked();

    //void showDetailWidget(QString name);
    //void hideDetailWidget();

private:
    Ui::MainPage *ui;
    int previousColorRow;
    bool hasDelegateOrNot;
    bool refreshOrNot;
    AccountDetailWidget* detailWidget;
    int currentAccountIndex;
    double _totalAcsBalance; // total ACS
    double _total3dcBalance; // total 3DC
    double _totalNbjBalance; // total NBJ
    double _totalNotBalance; // total NOT
    bool _first_login;
    //bool eventFilter(QObject *watched, QEvent *e);
    //void paintOnScrollarea(QWidget *w);
    void paintEvent(QPaintEvent*);
    void updateTotalBalance();
    void updatePending();
    bool eventFilter(QObject *watched, QEvent *e);

    //acs
    void updateAcsTotalBalance();
    void updateAcsAccountList();

    //3dc
    void update3dcTotalBalance();
    void update3dcAccountList();

    // NBJ
    void updateNbjTotalBalance();
    void updateNbjAccountList();

    // NOT
    void updateNotTotalBalance();
    void updateNotAccountList();
};

#endif // MAINPAGE_H
