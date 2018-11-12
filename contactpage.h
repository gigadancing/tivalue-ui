#ifndef CONTACTPAGE_H
#define CONTACTPAGE_H

#include <QWidget>
#include <QVector>
#include "singlecontactwidget.h"

namespace Ui {
class ContactPage;
}

class ContactPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContactPage(QWidget *parent = 0);
    ~ContactPage();
    void showAddContactPage();
    void retranslator(QString language);

signals:
    void showShadowWidget();
    void hideShadowWidget();
    void gotoTransferPage(QString);

private slots:
    void on_addContactBtn_clicked();

    void updateContactsList();

    void shadowWidgetShow();
    void shadowWidgetHide();

    void editContact(QString address, QString remark);

    void jsonDataUpdated(QString id);

private:
    Ui::ContactPage *ui;  
    QString oldRemark;
    QVector<SingleContactWidget*> contactVector;

    void releaseVector();
    void paintEvent(QPaintEvent*);
};

#endif // CONTACTPAGE_H
