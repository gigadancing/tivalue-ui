#ifndef DOWNLOADFILEPAGE_H
#define DOWNLOADFILEPAGE_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class QStandardItemModel;

namespace Ui {
class DownloadFilePage;
}

class DownloadFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadFilePage(QWidget *parent = 0);
    ~DownloadFilePage();
public :
    void downloadFile();
private :
    void initWidget();
    void setttingModel();

private:
    Ui::DownloadFilePage *ui;
    QStandardItemModel* m_model;

    QNetworkAccessManager *_downloadManager;
    QNetworkReply *_reply;
    QFile *download_file;
private :
    void listSavedFiles();
    void parseUploadRequests(QString strJson);
    QString getRpcResponse(QString id);
private slots:
    void replyFinished(QNetworkReply *reply);
    void downloadError(QNetworkReply::NetworkError errorCode);
    void OndownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // DOWNLOADFILEPAGE_H
