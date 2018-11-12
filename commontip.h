#ifndef COMMONTIP_H
#define COMMONTIP_H

#include <QDialog>

namespace Ui {
class CommonTip;
}

class CommonTip : public QDialog
{
    Q_OBJECT

public:
    explicit CommonTip(QWidget *parent = 0);
    ~CommonTip();

    void setText(QString);

private:
    Ui::CommonTip *ui;
};

#endif // COMMONTIP_H
