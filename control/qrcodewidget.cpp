#include "qrcodewidget.h"

#include <QPainter>
#include <QDebug>

QRCodeWidget::QRCodeWidget(QWidget *parent) : QWidget(parent)
{
    qr = NULL;
}

QRCodeWidget::~QRCodeWidget()
{
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
}

int QRCodeWidget::getQRWidth() const
{
    if(qr != NULL)
    {
        return qr->width;
    }
    else
    {
        return 0;
    }
}

void QRCodeWidget::setString(QString str)
{
    string = str;
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
    qr = QRcode_encodeString(string.toStdString().c_str(),
                             1,
                             QR_ECLEVEL_L,
                             QR_MODE_8,
                             1);
    update();
}
QSize QRCodeWidget::sizeHint()  const
{
    QSize s;
    if(qr != NULL)
    {
        int qr_width = qr->width > 0 ? qr->width : 1;
        s = QSize(qr_width * 3, qr_width * 3);
    }
    else
    {
        s = QSize(50, 50);
    }
    return s;
}

QSize QRCodeWidget::minimumSizeHint()  const
{
    QSize s;
    if(qr != NULL)
    {
        int qr_width = qr->width > 0 ? qr->width : 1;
        s = QSize(qr_width, qr_width);
    }
    else
    {
        s = QSize(50, 50);
    }
    return s;
}


void QRCodeWidget::draw(QPainter &painter, int width, int height)
{
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    const int qr_width = qr->width > 0 ? qr->width : 1;
    double scale_x = width / qr_width;
    double scale_y = height / qr_width;
    int int_scale_x = (int) scale_x;
    int int_scale_y = (int) scale_y;
    int padding_width = width - int_scale_x * qr_width;
    int padding_height = height - int_scale_y * qr_width;
    int half_padding_width = (int)(padding_width / 2);
    for( int y = 0; y < qr_width; y ++)
    {
        for(int x = 0; x < qr_width; x++)
        {
            unsigned char b = qr->data[y * qr_width + x];
            if(b & 0x01)
            {
                QRectF r(x * scale_x+half_padding_width, y * scale_y+half_padding_width, scale_x, scale_y);
                painter.drawRects(&r, 1);
            }
        }
    }
}

void QRCodeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width(), height());
    if(qr != NULL)
    {
        draw(painter, width(), height());
    }
}
