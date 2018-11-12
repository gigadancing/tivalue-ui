#ifndef INCOMECELLWIDGET_H
#define INCOMECELLWIDGET_H

#include <QWidget>

namespace Ui {
class IncomeCellWidget;
}

class IncomeCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IncomeCellWidget(QString name, QWidget *parent = 0);
    ~IncomeCellWidget();

    QString text();
    void setText(QString text);

signals:
    void withdrawSalary(QString name, QString salary);

private:
    Ui::IncomeCellWidget *ui;
    QString accountName;
    QString salary;

    void mousePressEvent(QMouseEvent* event);
};

#endif // INCOMECELLWIDGET_H
