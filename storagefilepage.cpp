#include "storagefilepage.h"
#include "ui_storagefilepage.h"

#include "tic.h"
#include "rpcthread.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QStandardItemModel>
#include <QHttpMultiPart>
#include <QMessageBox>

StorageFilePage::StorageFilePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageFilePage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#16181c"));
    setPalette(palette);

    initWidget();
    settingMOdelView();
    ui->choose_label->move(ui->pushButton->x(),ui->pushButton->y()+ui->pushButton->height());

    _uploadManager = new QNetworkAccessManager(this);//http request
    connect(_uploadManager,SIGNAL(finished(QNetworkReply*)),SLOT(replyFinished(QNetworkReply*)));
}

void StorageFilePage::initWidget()
{
    ui->pushButton->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
    ui->pushButton_2->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
    ui->pushButton_3->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
}

void StorageFilePage::settingMOdelView()
{
    ui->uploadTableView->setStyleSheet("background-color:#16181c;color:#666666");
    ui->uploadTableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#16181c;color: #666666;}");
    //ui->uploadTableView->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#16181c;color: #666666;}");
    ui->uploadTableView->verticalHeader()->setVisible(false);
    m_model = new QStandardItemModel();
    ui->uploadTableView->setModel(m_model);
    ui->uploadTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_model->setColumnCount(4);
    m_model->setHeaderData(0, Qt::Horizontal, tr("file Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("file id"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("piece id"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("choose"));
    listUploadRequests();
}

StorageFilePage::~StorageFilePage()
{
    delete ui;
}

void StorageFilePage::listUploadRequests()
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

    Fry::getInstance()->postRPC(toJsonFormat( "id_blockchain_list_can_apply_file", "blockchain_list_can_apply_file", QStringList() <<accountName));

    QString result = getRpcResponse(QString("id_blockchain_list_can_apply_file"));
    if(result.length() == 0)
    {
        return;
    }

    parseUploadRequests(result);
}

int StorageFilePage::parseUploadRequests(QString strJson)
{
    QString filename;
    QString fileId;
    QString pieceId;

    QJsonParseError jsonError;
    strJson = strJson.mid(QString("\"result\"").size() + 1);//remove "result":
    QJsonDocument doucment = QJsonDocument::fromJson(strJson.toUtf8(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isArray())
        { // JSON 文档
            QJsonArray arr = doucment.array();  // 转化为对象
            for (int i=0; i<arr.size(); ++i) {
               QJsonObject uploadObject = arr.at(i).toObject();
               QJsonObject fileObj = uploadObject.value("file_id").toObject();
               // QJsonObject piece = uploadObject.value("piece").toObject();  // 获取指定 key 对应的 value

               filename = uploadObject.value("file_name").toString();
               fileId = fileObj.value("file_id").toString()+fileObj.value("uploader").toString();
               pieceId = fileObj.value("file_id").toString();

               int rowcount = m_model->rowCount();

               m_model->setItem(rowcount, 0, new QStandardItem(filename));
               m_model->setItem(rowcount, 1, new QStandardItem(fileId));
               m_model->setItem(rowcount, 2, new QStandardItem(pieceId));
               QStandardItem* item = new QStandardItem();
               item->setCheckable(true);
               m_model->setItem(rowcount, 3, item);
            }
        }
    }

    return 0;
}

void StorageFilePage::applayStorage()
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

    for(int i = 0; i < m_model->rowCount(); i++)
    {
        bool value =  m_model->item(i,3)->checkState();
        QString fileId = m_model->index(i,1,QModelIndex()).data().toString();
        QString pieceId = m_model->index(i,2,QModelIndex()).data().toString();

        if(value)
        {
            QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

            QHttpPart hashPart;
            hashPart.setBody("");
            multiPart->append(hashPart);
            QUrl url = QString("http://localhost:5001/api/v0/pin/add?arg=")+ pieceId;
            QNetworkRequest request(url);
            _reply = _uploadManager->post(request, multiPart);
            multiPart->setParent(_reply);
            connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(storageError(QNetworkReply::NetworkError)));
            connect(_reply, SIGNAL(uploadProgress ( qint64 ,qint64 )), this, SLOT( OnstorageProgress(qint64 ,qint64 )));
            //Fry::getInstance()->postRPC(toJsonFormat( "id_pinfile_to_ipfs", "PinAddFileToLocal", QStringList() <<pieceId));

            Fry::getInstance()->postRPC( toJsonFormat( "id_declare_piece_saved", "declare_piece_saved", \
                                                       QStringList() << fileId <<pieceId<<accountName<<"node_id"));
        }
        m_model->item(i,3)->setCheckState(Qt::Unchecked);
    }
}

QString StorageFilePage::getRpcResponse(QString id)
{
    size_t count=0;
    QString res = "";
    do {
        count ++;
        QThread::msleep(500);
        res = Fry::getInstance()->jsonDataValue(id);
    } while(count < 3 && res.length() == 0);
    return res;
}


void StorageFilePage::replyFinished(QNetworkReply *reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<"StorageFilePage"<<status_code;
}
void StorageFilePage::storageError(QNetworkReply::NetworkError errorCode)
{
    QMessageBox::warning(this, tr("warning"), tr("storage fail"), QMessageBox::Ok);
    //qDebug() << "upLoadError  errorCode: " << (int)errorCode;
}
void StorageFilePage::OnstorageProgress(qint64 bytesSent, qint64 bytesTotal)
{
    //qDebug() << "bytesSent: " << bytesSent << "  bytesTotal: "<< bytesTotal;
}

void StorageFilePage::on_pushButton_clicked()
{
    ui->choose_label->move(ui->pushButton->x(),ui->pushButton->y()+ui->pushButton->height());
}

void StorageFilePage::on_pushButton_2_clicked()
{
    ui->choose_label->move(ui->pushButton_2->x(),ui->pushButton_2->y()+ui->pushButton_2->height());
}

void StorageFilePage::on_pushButton_3_clicked()
{
    ui->choose_label->move(ui->pushButton_3->x(),ui->pushButton_3->y()+ui->pushButton_3->height());
}
