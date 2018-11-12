#ifndef STORAGEPAGE_H
#define STORAGEPAGE_H

#include <QWidget>
#include <QItemDelegate>

class QStandardItemModel;

namespace Ui {
class StoragePage;
}



class ButtonDelegate : public QItemDelegate
{
public:
    explicit ButtonDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:

public slots:

private:
    QMap<QModelIndex, QStyleOptionButton*> m_btns;

};

class UploadFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadFilePage(QWidget *parent = 0);
    ~UploadFilePage();
private:
    void initWidget();
    void settingModel();
    void listUploadRequests();
    int parseUploadRequests(QString strJson);
    QString getRpcResponse(QString id);

public :
    void uploadSuccess(QString filename, QString filesize);
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::StoragePage *ui;
    QStandardItemModel* m_model;

    QStandardItemModel* m_AllowModel;
    QItemDelegate* m_delegate;
};


#endif // STORAGEPAGE_H
