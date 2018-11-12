#ifndef STORAGEFILEPAGE_H
#define STORAGEFILEPAGE_H

#include <QWidget>
#include <QNetworkReply>

class QStandardItemModel;
class QNetworkAccessManager;

namespace Ui {
class StorageFilePage;
}

class StorageFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit StorageFilePage(QWidget *parent = 0);
    ~StorageFilePage();
public:
    void applayStorage();

private:
    Ui::StorageFilePage *ui;
    QStandardItemModel* m_model;
    QNetworkAccessManager *_uploadManager;
    QNetworkReply *_reply;
private slots:
    void replyFinished(QNetworkReply *reply);
    void storageError(QNetworkReply::NetworkError errorCode);
    void OnstorageProgress(qint64 bytesSent, qint64 bytesTotal);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private :
    void listUploadRequests();
    QString getRpcResponse(QString id);
    int parseUploadRequests(QString strJson);
    void initWidget();
    void settingMOdelView();
};

#endif // STORAGEFILEPAGE_H
