#ifndef QRCODEWIDGET_H
#define QRCODEWIDGET_H

#include <QWidget>
#include "qrencode.h"

class QRCodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRCodeWidget(QWidget *parent = 0);
    ~QRCodeWidget();

    void setString(QString str);
    int getQRWidth() const;

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    void draw(QPainter &painter, int width, int height);
    QString string;
    QRcode* qr;
};

#endif // QRCODEWIDGET_H
