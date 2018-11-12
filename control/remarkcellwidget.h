#ifndef REMARKCELLWIDGET_H
#define REMARKCELLWIDGET_H

#include <QWidget>

namespace Ui {
class RemarkCellWidget;
}

class RemarkCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RemarkCellWidget( QString str, QWidget *parent = 0);
    ~RemarkCellWidget();

    QString text();

private:
    Ui::RemarkCellWidget *ui;
    QString ss;
};

#endif // REMARKCELLWIDGET_H
