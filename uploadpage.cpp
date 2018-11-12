#include "ui_uploadpage.h"
#include "uploadbar.h"
#include "uploadpage.h"
#include "tic.h"
#include "rpcthread.h"

#include <QStandardItemModel>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>

#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QStyleOption>
#include <QDesktopWidget>
#include <QPushButton>

UploadFilePage::UploadFilePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StoragePage),
    m_model(NULL)
{
    ui->setupUi(this);
    initWidget();
}

UploadFilePage::~UploadFilePage()
{
    delete ui;
}

void UploadFilePage::initWidget()
{
    ui->pushButton->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
    ui->pushButton_2->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
    ui->pushButton_3->setStyleSheet("background-color:#16181c;color:#666666;border:0px");

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#16181c"));
    setPalette(palette);
    settingModel();
    ui->choosed_lable->move(ui->pushButton->x(),ui->pushButton->y()+ui->pushButton->height());
    //listUploadRequests();
}

void UploadFilePage::settingModel()
{


    ui->uploadTableView->setStyleSheet("background-color:#16181c;color:#666666");
    ui->uploadTableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#16181c;color: #666666;}");
    //ui->uploadTableView->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#16181c;color: #666666;}");
    ui->uploadTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->uploadTableView->verticalHeader()->setVisible(false);
    m_model = new QStandardItemModel();
    ui->uploadTableView->setModel(m_model);
    m_model->setColumnCount(3);
    m_model->setHeaderData(0, Qt::Horizontal, tr("file Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("file size"));
    //m_model->setHeaderData(2, Qt::Horizontal, "time");
    //m_model->setHeaderData(3, Qt::Horizontal, "save count");
    //m_model->setHeaderData(4, Qt::Horizontal, "bill cycle");
    //m_model->setHeaderData(5, Qt::Horizontal, "save duration");
    //m_model->setHeaderData(6, Qt::Horizontal, "download fee");
    m_model->setHeaderData(2, Qt::Horizontal, tr("state"));

    //ui->uploadTableView->setColumnWidth(0,262);
    //ui->uploadTableView->setColumnWidth(1,346);
    //ui->uploadTableView->setColumnWidth(2,100);
}

void UploadFilePage::listUploadRequests()
{
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();
    QString accountName;
    for (int i = size - 1; i > -1; i--)
    {
        accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();

    Fry::getInstance()->postRPC(toJsonFormat("id_wallet_list_my_upload_requests", "wallet_list_my_upload_requests", QStringList() <<accountName));

    QString result = getRpcResponse(QString("id_wallet_list_my_upload_requests"));
    if(result.length() == 0)
    {
        return;
    }
    parseUploadRequests(result);
}

void UploadFilePage::uploadSuccess(QString filename, QString filesize)
{
    int rowcount = m_model->rowCount();
    m_model->setItem(rowcount,0,new QStandardItem(filename));
    m_model->setItem(rowcount,1,new QStandardItem(filesize));
    m_model->setItem(rowcount,2,new QStandardItem(tr("to be stored")));
}

int UploadFilePage::parseUploadRequests(QString strJson)
{
    QString filename;
    QString filesize;
    QString fileprice;
    QString filestatus;
    QString storerNode;
    QString storerKey;
    QString fileid;
    QString pieceid;
    QString nodeId;

    QJsonParseError jsonError;
    strJson = strJson.mid(QString("\"result\"").size() + 1);//remove "result":
    QJsonDocument doucment = QJsonDocument::fromJson(strJson.toUtf8(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isArray())
        { // JSON 文档
            QJsonArray arr = doucment.array();  // 转化为对象
            for (int i=0; i<arr.size(); ++i)
            {
               QJsonObject uploadObject = arr.at(i).toObject();
               QJsonObject fileObj = uploadObject.value("file_id").toObject();
               QJsonObject piece = uploadObject.value("piece").toObject();  // 获取指定 key 对应的 value
               QJsonArray storers = uploadObject.value("storers").toArray();  // 获取指定 key 对应的 value
               int confirm = uploadObject.value("num_of_confirmed").toInt();

               filename = uploadObject.value("file_name").toString();
               filesize = QString("%1").arg(piece.value("piece_size").toInt());
               fileprice = piece.value("price").toString();
               fileid = fileObj.value("file_id").toString()+fileObj.value("uploader").toString();
               pieceid = piece.value("pieceid").toString();
               nodeId = uploadObject.value("node_id").toString();


               if (storers.size() == 0)
               {
                   continue;
               }
               else if (storers.size() > 0 )
               {
                  QJsonObject storer = storers.at(0).toObject();
                  storerNode = storer.value("node").toString();
                  storerKey = storer.value("key").toString();
                   if (confirm > 0)
                   {
                       continue;
                   } else
                   {
                       filestatus = "stored";
                   }
               }
               int rowcount = m_AllowModel->rowCount();
               m_AllowModel->setItem(rowcount, 0, new QStandardItem(filename));
               m_AllowModel->setItem(rowcount, 1, new QStandardItem(fileid));
               m_AllowModel->setItem(rowcount, 2, new QStandardItem(pieceid));
               m_AllowModel->setItem(rowcount, 3, new QStandardItem(storerKey));

            }

        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

QString UploadFilePage::getRpcResponse(QString id)
{
    size_t count=0;
    QString res = "";
    do {
        count ++;
        QThread::msleep(500);
        res = Fry::getInstance()->jsonDataValue(id);
    } while(count < 2);
    return res;
}

void UploadFilePage::on_pushButton_2_clicked()
{
    ui->choosed_lable->move(ui->pushButton_2->x(),ui->pushButton_2->y()+ui->pushButton->height());
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();
    QString accountName;
    for (int i = size - 1; i > -1; i--)
    {
        accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
//    Fry::getInstance()->postRPC(toJsonFormat( "id_wallet_list_my_upload_requests", "wallet_list_my_upload_requests", QStringList() <<"qwert"));
//    QString result = getRpcResponse(QString("id_wallet_list_my_upload_requests"));
//    if(result.length() == 0)
//    {
//        return;
//    }

    m_AllowModel = new QStandardItemModel();
    ui->uploadTableView->setModel(m_AllowModel);
    m_delegate = new ButtonDelegate(ui->uploadTableView);
    ui->uploadTableView->setItemDelegateForColumn(4, m_delegate);
    m_AllowModel->setColumnCount(5);
//    m_AllowModel->setRowCount(2);
    m_AllowModel->setHeaderData(0, Qt::Horizontal, tr("file Name"));
    m_AllowModel->setHeaderData(1, Qt::Horizontal, tr("file id"));
    m_AllowModel->setHeaderData(2, Qt::Horizontal, tr("piece id"));
    m_AllowModel->setHeaderData(3, Qt::Horizontal, tr("storer key"));
    m_AllowModel->setHeaderData(4, Qt::Horizontal, tr("choose"));
    //m_AllowModel->setItem(0,0,new QStandardItem("filename"));
    //m_AllowModel->setItem(1,0,new QStandardItem("12131"));
    listUploadRequests();
}

void UploadFilePage::on_pushButton_clicked()
{
    ui->choosed_lable->move(ui->pushButton->x(),ui->pushButton->y()+ui->pushButton->height());
}

void UploadFilePage::on_pushButton_3_clicked()
{
    ui->choosed_lable->move(ui->pushButton_3->x(),ui->pushButton_3->y()+ui->pushButton->height());
}


/**************delegate******************/
ButtonDelegate::ButtonDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton* button = m_btns.value(index);
    if (!button) {
        button = new QStyleOptionButton();
        button->rect = option.rect.adjusted(4, 4, -4, -4);
        button->text = tr("confirm");
        button->state |= QStyle::State_Enabled;
        button->features = QStyleOptionButton::Flat;
        QPalette palette;
        palette.setBrush(QPalette::ButtonText,QBrush(QColor("#666666")));//按钮文字颜色
        button->palette = palette;
        (const_cast<ButtonDelegate *>(this))->m_btns.insert(index, button);
    }
    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());

    }
    painter->restore();
    painter->fillRect(option.rect, QBrush(QColor("#16181c")));
    QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    mutexForConfigFile.lock();
    Fry::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = Fry::getInstance()->configFile->childKeys();
    int size = keys.size();
    QString accountName;
    for (int i = size - 1; i > -1; i--)
    {
        accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
    }
    Fry::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && m_btns.contains(index)) {
            m_btns.value(index)->state |= QStyle::State_Sunken;
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && m_btns.contains(index)) {
            m_btns.value(index)->state &= (~QStyle::State_Sunken);
            QString fileId = model->index(index.row(),1,QModelIndex()).data().toString();
            QString pieceId = model->index(index.row(),2,QModelIndex()).data().toString();
            QString publicKey = model->index(index.row(),3,QModelIndex()).data().toString();
            Fry::getInstance()->postRPC(toJsonFormat( "id_confirm_piece_saved", "confirm_piece_saved", \
                                          QStringList() <<accountName<<fileId \
                                          <<pieceId<<publicKey<<"1"));


            //QDialog *d = new QDialog();
            //d->setGeometry(0, 0, 200, 200);
            //d->move(QApplication::desktop()->screenGeometry().center() - d->rect().center());
            //d->show();
        }
        model->removeRow(index.row());
    }

    return true;
}
