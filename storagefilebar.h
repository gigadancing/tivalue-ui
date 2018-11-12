#ifndef STORAGEFILEBAR_H
#define STORAGEFILEBAR_H

#include <QWidget>

namespace Ui {
class StorageFileBar;
}

class StorageFileBar : public QWidget
{
    Q_OBJECT

public:
    explicit StorageFileBar(QWidget *parent = 0);
    ~StorageFileBar();
private slots:
    void on_toolButton_clicked();
signals :
    void applayStorage();

private:
    Ui::StorageFileBar *ui;
};

#endif // STORAGEFILEBAR_H
