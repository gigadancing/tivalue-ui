#ifndef FUNCTIONBAR_H
#define FUNCTIONBAR_H

#include <QWidget>

namespace Ui {
class FunctionBar;
}

class FunctionBar : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionBar(QWidget *parent = 0);
    ~FunctionBar();
    void choosePage(int);
    void retranslator();

signals:
    void showMainPage();
    void showAccountPage(QString);
    void showTransferPage();
    void showContactPage();
    void showShadowWidget();
    void hideShadowWidget();
    void showUploadPage();
    void showStorageFilePage();
    void showDownloadFilePage();

private slots:
    void on_accountBtn_clicked();

    void on_transferBtn_clicked();

    void on_contactBtn_clicked();

    void on_StorageServiceBtn_2_clicked();

    void on_ControlPlatBtn_clicked();

    void on_UploadFileBtn_2_clicked();

    void on_StorageFileBtn_4_clicked();

    void on_DownloadFileBtn_clicked();

private:
    Ui::FunctionBar *ui;
    bool bStorage;
};

#endif // FUNCTIONBAR_H
