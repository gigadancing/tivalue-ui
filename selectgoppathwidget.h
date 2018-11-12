#ifndef SELECTGOPPATHWIDGET_H
#define SELECTGOPPATHWIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QDomDocument>

namespace Ui {
class SelectGopPathWidget;
}

class SelectGopPathWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectGopPathWidget(QWidget *parent = 0);
    ~SelectGopPathWidget();

private slots:
    void on_selectPathBtn_clicked();

    void on_okBtn_clicked();

    void on_minBtn_clicked();

    void on_closeBtn_clicked();

    void httpFinished();

    void getUpdateExeFinished();

    void getUpdateExeReadyRead();

signals:
    void enter();
    void minimum();
    void closeGOP();
    void showShadowWidget();
    void hideShadowWidget();

private:
    Ui::SelectGopPathWidget *ui;
    QNetworkReply *reply;
    QDomDocument localXml;
    QDomDocument serverXml;
    QNetworkAccessManager qnam;
    bool updateOrNot;
    QFile* updateExe;

    void getUpdateXml();
    QString getXmlElementText(QDomDocument& doc, QString name);

    void paintEvent(QPaintEvent*);
};

int compareVersion( QString a, QString b);

#endif // SELECTGOPPATHWIDGET_H
