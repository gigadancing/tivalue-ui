#ifndef UPGRADEPAGE_H
#define UPGRADEPAGE_H

#include <QWidget>

namespace Ui {
class UpgradePage;
}

class UpgradePage : public QWidget
{
    Q_OBJECT

public:
    explicit UpgradePage(QString name,QWidget *parent = 0);
    ~UpgradePage();

    void refresh();

private slots:

    void on_addressNameLineEdit_textChanged(const QString &arg1);

    void on_upgradeBtn_clicked();

    void on_accountComboBox_currentTextChanged(const QString &arg1);

    void jsonDataUpdated(QString id);

signals:
    void back(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showApplyDelegatePage(QString);
    void accountRenamed();

private:
    Ui::UpgradePage *ui;

    void init();
    bool checkAddrNameFormat(QString);
    void paintEvent(QPaintEvent*);

    QString accountName;
    QString balance;
};

#endif // UPGRADEPAGE_H
