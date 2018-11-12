#ifndef DOWNLOADBAR_H
#define DOWNLOADBAR_H

#include <QWidget>

namespace Ui {
class DownloadBar;
}

class DownloadBar : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadBar(QWidget *parent = 0);
    ~DownloadBar();
signals:
    void downloadFile();

private slots:
    void on_toolButton_clicked();

private:
    Ui::DownloadBar *ui;
};

#endif // DOWNLOADBAR_H
