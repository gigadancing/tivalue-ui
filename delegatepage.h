#ifndef DELEGATEPAGE_H
#define DELEGATEPAGE_H

#include <QWidget>

namespace Ui {
class DelegatePage;
}

int applyEnable();    //  返回可以申请代理的账户个数
class SearchOptionWidget;

#define MODULE_DELEGATE     "DELEGATE"

class DelegatePage : public QWidget
{
    Q_OBJECT

public:
    explicit DelegatePage(QWidget *parent = 0);
    ~DelegatePage();

    void updateDelegateList();
    void updateFavoriteDelegateList();
    void updateIncomeLabel();
    void showDelegates(int);
    void showFavoriteDelegates(int);
//    bool haveRegistedAccount();
    void refresh();
//    void retranslator(QString);

public slots:
    void  delegateListUpdated();

signals:
    void back();
    void applyDelegate(QString);

private slots:

//    void on_firstPageBtn_clicked();

    void on_delegateTableWidget_cellClicked(int row, int column);

    void on_favoriteDelegateTableWidget_cellClicked(int row, int column);

    void on_applyBtn_clicked();

    void on_searchBtn_clicked();

    void on_prePageBtn_clicked();

    void on_nextPageBtn_clicked();

//    void on_lastPageBtn_clicked();

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_pageLineEdit_editingFinished();

    void on_pageLineEdit_textEdited(const QString &arg1);

    void on_infoBtn1_clicked();

    void on_infoBtn2_clicked();

    void on_totalBtn_clicked();

    void on_favoriteBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_prePageBtn2_clicked();

    void on_nextPageBtn2_clicked();

    void on_pageLineEdit2_editingFinished();

    void on_pageLineEdit2_textEdited(const QString &arg1);

private:
    Ui::DelegatePage *ui;
    int currentPageIndex;
    int currentPageIndex2;
    QStringList searchList;
    QStringList approveList;
    SearchOptionWidget* searchOptionWidget;

    bool eventFilter(QObject *watched, QEvent *e);
//    void paintOnScrollarea(QWidget *w);
    void paintEvent(QPaintEvent*);
    void stringListSort(QStringList& stringList);  // 对stringlist 按代理排名排序
};

#endif // DELEGATEPAGE_H
