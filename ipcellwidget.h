#ifndef IPCELLWIDGET_H
#define IPCELLWIDGET_H

#include <QWidget>

namespace Ui {
class IpCellWidget;
}

class IpCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IpCellWidget( QString ip, bool flag, QWidget *parent = 0);
    ~IpCellWidget();

private slots:
    void on_nodeSetBtn_clicked();

signals:
    void blockSet(QString,bool);

private:
    Ui::IpCellWidget *ui;
    bool blackOrNot;
};

#endif // IPCELLWIDGET_H
