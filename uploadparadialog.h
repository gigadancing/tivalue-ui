#ifndef UPLOADPARADIALOG_H
#define UPLOADPARADIALOG_H

#include <QDialog>

namespace Ui {
class uploadParaDialog;
}

class uploadParaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit uploadParaDialog(QWidget *parent = 0);
    ~uploadParaDialog();

private:
    Ui::uploadParaDialog *ui;
    bool bCancel;
protected :
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool m_MousePressed;
    QPoint m_MousePos;
    QPoint m_WindowPos;

public :
    int run();
    QString getAccountName();
    QString getBackUpNumber();
    QString getPayCycle();
    QString getStoragePrice();
private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_comboBox_currentIndexChanged(const QString &arg1);
};

#endif // UPLOADPARADIALOG_H
