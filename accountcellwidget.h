#ifndef ACCOUNTCELLWIDGET_H
#define ACCOUNTCELLWIDGET_H

#include <QWidget>

namespace Ui {
class AccountCellWidget;
}

class AccountCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountCellWidget( QString name, int r, int c, QWidget *parent = 0);
    ~AccountCellWidget();

    QString accountName;

    void setBackgroundColor(int r, int g, int b, int a = 255);

    int row;
    int column;

signals:
    void showExportDialog(QString);
    void cellEnter(int,int);

private:
    Ui::AccountCellWidget *ui;
    QString registeredLabelString;

    void enterEvent( QEvent* e);
};

#endif // ACCOUNTCELLWIDGET_H
