#ifndef TRANSFERPAGE_H
#define TRANSFERPAGE_H

#include <QWidget>
#include <QComboBox>
namespace Ui {
class TransferPage;
}

#define MODULE_TRAHSFER_PAGE "TRAHSFER_PAGE"

class TransferPage : public QWidget
{
    Q_OBJECT

public:
    explicit TransferPage(QString name,QWidget *parent = 0);
    ~TransferPage();

    QString getCurrentAccount();
    void setAddress(QString);
    QComboBox* getAssetCombo();
signals:
    void accountChanged(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showAccountPage(QString);
    void back();

public slots:
    void refresh();

private slots:

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_sendBtn_clicked();

    void on_amountLineEdit_textChanged(const QString &arg1);

    void on_sendtoLineEdit_textChanged(const QString &arg1);

    void on_addContactBtn_clicked();

    void contactSelected(QString);

    void jsonDataUpdated(QString id);

    void on_feeLineEdit_textChanged(const QString &arg1);

    void on_messageLineEdit_textChanged(const QString &arg1);

    void onAssetComboChanged(int index);

private:
    Ui::TransferPage *ui;
    QString accountName;
    QStringList contactsList;
    bool inited;
    QString delegateLabelString;
    QString registeredLabelString;

    void paintEvent(QPaintEvent*);
//    void addContact(QString,QString);
    void getContactsList();
};

#endif // TRANSFERPAGE_H
