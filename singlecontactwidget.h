#ifndef SINGLECONTACTWIDGET_H
#define SINGLECONTACTWIDGET_H

#include <QWidget>

namespace Ui {
class SingleContactWidget;
}

class SingleContactWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SingleContactWidget(int num,QString address,QString remarkName,QWidget *parent = 0);
    ~SingleContactWidget();

private slots:
    void on_deleteBtn_clicked();

    void on_transferBtn_clicked();

    void on_editBtn_clicked();

signals:
    void deleteContact();
    void showShadowWidget();
    void hideShadowWidget();
    void editContact(QString,QString);
    void gotoTransferPage(QString);

private:
    Ui::SingleContactWidget *ui;
    int number;

    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
};

#endif // SINGLECONTACTWIDGET_H
