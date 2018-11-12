#ifndef WAITINGFORSYNC_H
#define WAITINGFORSYNC_H

#include <QWidget>
#include <QtNetwork>
#include <QDomDocument>

namespace Ui {
class WaitingForSync;
}

class QNetworkReply;

class WaitingForSync : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingForSync(QWidget *parent = 0);
    ~WaitingForSync();

    void getUpdateXml();

    QTimer* timer;

signals:
    void sync();
    void minimum();
    void tray();
    void closeGOP();
    void showShadowWidget();
    void hideShadowWidget();

public slots:


private slots:
    void updateInfo();

    void on_minBtn_clicked();

    void on_closeBtn_clicked();

//    void on_progressBar_valueChanged(int value);

    void infoUpdated(QString id);

    void httpFinished();

    void getUpdateExeFinished();

    void getUpdateExeReadyRead();

//    void hasSomethingToRead();

private:
    Ui::WaitingForSync *ui;
    QNetworkReply *reply;
    QDomDocument localXml;
    QDomDocument serverXml;
    QNetworkAccessManager qnam;
    bool updateOrNot;
    QFile* updateExe;
    bool synced;  // 防止发出两次sync()信号

    QString getXmlElementText(QDomDocument& doc, QString name);
    void updateBebuildInfo();

};

#endif // WAITINGFORSYNC_H
