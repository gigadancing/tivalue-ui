#ifndef STORAGEBAR_H
#define STORAGEBAR_H

#include <QWidget>
#include <QNetworkReply>

class QNetworkAccessManager;


namespace Ui {
class StorageBar;
}

class UploadBar : public QWidget
{
    Q_OBJECT

public:
    explicit UploadBar(QWidget *parent = 0);
    ~UploadBar();
signals:
    void uploadSuccess(QString filename, QString filesize);

private slots:
    void on_toolButton_clicked();
    void replyFinished(QNetworkReply *reply);
    void upLoadError(QNetworkReply::NetworkError errorCode);
    void OnUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private :
    void initWidget(QWidget* parent);
    QString  queryJsonValue(QString strJson, QString key);
    QString getRpcResponse(QString id);
private:
    Ui::StorageBar *ui;
    QNetworkAccessManager *_uploadManager;
    QNetworkReply *_reply;
};

#endif // STORAGEBAR_H
