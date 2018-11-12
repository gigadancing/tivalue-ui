#include "downloadfilepage.h"
#include "ui_downloadfilepage.h"

#include "tic.h"
#include "rpcthread.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QStandardItemModel>
#include <QFileDialog>

#include <QHttpMultiPart>
#include <QMessageBox>

DownloadFilePage::DownloadFilePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadFilePage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#16181c"));
    setPalette(palette);
    initWidget();
    ui->choose_lable->move(ui->pushButton->x(),ui->pushButton->y()+ui->pushButton->height());
    _downloadManager = new QNetworkAccessManager(this);//http request
    connect(_downloadManager,SIGNAL(finished(QNetworkReply*)),SLOT(replyFinished(QNetworkReply*)));
}


void DownloadFilePage::initWidget()
{
    ui->pushButton->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
    ui->pushButton_2->setStyleSheet("background-color:#16181c;color:#666666;border:0px");
    ui->downloadTableView->setStyleSheet("background-color:#16181c;color:#666666");
    ui->downloadTableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#16181c;color: #666666;}");
    //ui->downloadTableView->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#16181c;color: #666666;}");
    ui->downloadTableView->verticalHeader()->setVisible(false);
    setttingModel();
    listSavedFiles();
}
void DownloadFilePage::setttingModel()
{
    m_model = new QStandardItemModel();
    ui->downloadTableView->setModel(m_model);
    ui->downloadTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_model->setColumnCount(3);
    m_model->setHeaderData(0, Qt::Horizontal, tr("file Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("file hash"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("download"));
}

void DownloadFilePage::downloadFile()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("choose file"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("All Files(*.*)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::Directory);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
    else
    {
        return;
    }
    QString fileDir = fileNames.at(0);
    for(int i = 0; i < m_model->rowCount(); i++)
    {
        bool value =  m_model->item(i,2)->checkState();
        if(value)
        {
            QString filename = fileDir + "/" + m_model->index(i,0,QModelIndex()).data().toString();
            QString filehash = m_model->index(i,1,QModelIndex()).data().toString();

            QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
            QHttpPart hashPart;
            hashPart.setBody("");
            multiPart->append(hashPart);
            QUrl url = QString("http://localhost:5001/api/v0/cat?arg=")+ filehash;
            QNetworkRequest request(url);
            _reply = _downloadManager->post(request , multiPart );
            multiPart->setParent(_reply);
            connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
            connect(_reply, SIGNAL(downloadProgress( qint64 ,qint64 )), this, SLOT( OndownloadProgress(qint64 ,qint64 )));
            download_file = new QFile(filename);
            download_file->open(QIODevice::ReadWrite);
            //Fry::getInstance()->postRPC(toJsonFormat( "id_downloadfile_from_ipfs", "DownloadFileFromIPFS", \
                                                      QStringList() <<filehash<< filename));
        }
        m_model->item(i,2)->setCheckState(Qt::Unchecked);
    }
}

DownloadFilePage::~DownloadFilePage()
{
    delete ui;
}

void DownloadFilePage::listSavedFiles()
{
//    mutexForConfigFile.lock();
//    Fry::getInstance()->configFile->beginGroup("/accountInfo");
//    QStringList keys = Fry::getInstance()->configFile->childKeys();
//    int size = keys.size();
//    QString accountName;
//    for (int i = size - 1; i > -1; i--)
//    {
//        accountName = Fry::getInstance()->configFile->value(keys.at(i)).toString();
//    }
//    Fry::getInstance()->configFile->endGroup();
//    mutexForConfigFile.unlock();

    Fry::getInstance()->postRPC(toJsonFormat( "id_blockchain_list_file_saved_info", "blockchain_list_file_saved_info", QStringList() <<""));

    QString result = getRpcResponse(QString("id_blockchain_list_file_saved_info"));
    if(result.length() == 0)
    {
        return;
    }

    parseUploadRequests(result);

}

void DownloadFilePage::parseUploadRequests(QString strJson)
{
    QString filename;
    QString filesize;
    QString fileprice;
    QString filehash;

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
               QJsonArray pieces = uploadObject.value("pieces").toArray();
               QJsonObject piece = pieces.at(0).toObject();// 获取指定 key 对应的 value

               filename = uploadObject.value("filename").toString();

               filesize = QString("%1").arg(piece.value("piece_size").toInt());

               fileprice = piece.value("price").toString();
               QJsonObject idObj = uploadObject.value("id").toObject();
               filehash = idObj.value("file_id").toString();

               int rowcount = m_model->rowCount();
               m_model->setItem(rowcount, 0, new QStandardItem(filename));
               m_model->setItem(rowcount, 1, new QStandardItem(filehash));
               QStandardItem* item = new QStandardItem();
               item->setCheckable(true);
               m_model->setItem(rowcount,2,item);
            }

        }

    }
}


QString DownloadFilePage::getRpcResponse(QString id)
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

void DownloadFilePage::replyFinished(QNetworkReply *reply)
{
    QByteArray bytes;
    QString result;
    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug()<<"no error.....";
        bytes = reply->readAll();  //获取字节
        result = QString(bytes);  //转化为字符串
        qDebug()<<result;
    }
    else{
        qDebug() << "replyFinished:  " << reply->error() << " " <<reply->errorString();
    }
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<"DownFile"<<status_code;
    if(status_code.toInt() == 200)
    {
        download_file->write(bytes);
    }
    download_file->close();
}

void DownloadFilePage::downloadError(QNetworkReply::NetworkError errorCode)
{
    QMessageBox::warning(this, tr("warning"), tr("download fail"), QMessageBox::Ok);
    //qDebug() << "upLoadError  errorCode: " << (int)errorCode;
}

void DownloadFilePage::OndownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    //qDebug() << "bytesSent: " << bytesSent << "  bytesTotal: "<< bytesTotal;
}

void DownloadFilePage::on_pushButton_clicked()
{
    ui->choose_lable->move(ui->pushButton->x(),ui->pushButton->y()+ui->pushButton->height());
}

void DownloadFilePage::on_pushButton_2_clicked()
{
    ui->choose_lable->move(ui->pushButton_2->x(),ui->pushButton_2->y()+ui->pushButton_2->height());
}
