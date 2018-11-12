#include"ui_uploadbar.h"
#include "uploadbar.h"

#include "uploadparadialog.h"
#include "tic.h"
#include "rpcthread.h"

#include <QFileDialog>
#include <QJsonParseError>
#include <QJsonObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QMessageBox>

UploadBar::UploadBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StorageBar)
{
    ui->setupUi(this);
    initWidget(parent);

    ui->label->setStyleSheet("color:#666666;border:0px");
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#191b1f"));
    setPalette(palette);

    _uploadManager = new QNetworkAccessManager(this);//http request
    connect(_uploadManager,SIGNAL(finished(QNetworkReply*)),SLOT(replyFinished(QNetworkReply*)));
}

void UploadBar::initWidget(QWidget *parent)
{
    ui->toolButton->setIconSize(QSize(94, 26));
    ui->toolButton->setStyleSheet("background:transparent;border:none;");
    ui->toolButton->setIcon(QIcon(":/pic/ticpic/uploadFileBtn.png"));
    ui->label->setText(tr("please choose a file to upload"));
}

UploadBar::~UploadBar()
{
    delete ui;
}

void UploadBar::on_toolButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("choose file"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("All Files(*.*)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    //fileDialog->setFileMode(QFileDialog::ExistingFiles);
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
    for(auto tmp:fileNames)
    {
        int size = QString(tmp).split("/").size();
        QString filename = QString(tmp).split("/").at(size - 1);
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart contentPart;
        contentPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data;name=\"file\";filename=\"%1\"").arg(filename)));
        contentPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        QFile *file = new QFile(tmp);
        file->open(QIODevice::ReadOnly);
        contentPart.setBodyDevice(file);
        file->setParent(multiPart);
        multiPart->append(contentPart);
        QUrl url = QString("http://localhost:5001/api/v0/add");
        QNetworkRequest request(url);
        _reply = _uploadManager->post(request , multiPart );
        multiPart->setParent(_reply);
        connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
        connect(_reply, SIGNAL(uploadProgress ( qint64 ,qint64 )), this, SLOT( OnUploadProgress(qint64 ,qint64 )));


        /*if(dlg.run() < 0)
        {
            return;
        }
        QString currentAccountName = dlg.getAccountName();
        Fry::getInstance()->postRPC(toJsonFormat( "id_wallet_transfer_to_contract", "wallet_transfer_to_contract", QStringList() <<dlg.getStoragePrice() \
                                                           <<"TV"<<currentAccountName<<"File_upload"<<"1"));
        QString result = getRpcResponse(QString("id_uploadfile_to_ipfs"));
        if(result.length() == 0)
        {
            return;
        }
        QString filename;
        QString filehash;
        QString filesize;
        queryJsonValue(result,"file_name", filename);
        queryJsonValue(result,"file_hash", filehash);
        queryJsonValue(result,"file_size", filesize);
        int size = QString(filename).split("/").size();
        filename = QString(filename).split("/").at(size - 1);
        Fry::getInstance()->postRPC( toJsonFormat( "id_store_file_to_network", "store_file_to_network", QStringList() <<currentAccountName<<filename \
                                                   <<filesize<<"describtion"<<filehash+";"+filehash +","+filesize +";" \
                                                   <<"TV"<<dlg.getStoragePrice()<<dlg.getBackUpNumber()<<"1" \
                                                   <<dlg.getPayCycle()<<"node_id"<<"1"));
        emit uploadSuccess(filename, filesize);*/
    }

}

/*
 *@in  strJson
 *@in  key
 *@return json value
 */
QString UploadBar::queryJsonValue(QString strJson, QString key)
{
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(strJson.toUtf8(), &jsonError);

    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isObject())
        { // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains(key))
            {  // 包含指定的 key
                QJsonValue result = object.value(key);  // 获取指定 key 对应的 value
                if (result.isString())
                {  // 判断 value 是否为字符串
                    return result.toString();
                }
                else
                {
                    return "";
                }
            }

        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}

QString UploadBar::getRpcResponse(QString id)
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

/*
*upload finished
*/
void UploadBar::replyFinished(QNetworkReply *reply)
{
    uploadParaDialog dlg;
    QByteArray bytes;
    QString result;
    if(reply->error() == QNetworkReply::NoError)
    {
        //qDebug()<<"no error.....";
        bytes = reply->readAll();  //获取字节
        result = QString(bytes);  //转化为字符串
        //qDebug()<<result;
    }
    else{
        qDebug() << "replyFinished:  " << reply->error() << " " <<reply->errorString();
    }
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<"Upload code"<<status_code;
    if(status_code.toInt() == 200)//success
    {
        if(dlg.run() < 0)
        {
            return;
        }
        QString currentAccountName = dlg.getAccountName();
        Fry::getInstance()->postRPC(toJsonFormat( "id_wallet_transfer_to_contract", "wallet_transfer_to_contract", QStringList() <<dlg.getStoragePrice() \
                                                           <<"TV"<<currentAccountName<<"File_upload"<<"1"));
        QString filename = queryJsonValue(result, "Name");
        QString filehash = queryJsonValue(result, "Hash");
        QString filesize = queryJsonValue(result, "Size");
        Fry::getInstance()->postRPC( toJsonFormat( "id_store_file_to_network", "store_file_to_network", QStringList() <<currentAccountName<<filename \
                                                   <<filesize<<"describtion"<<filehash+";"+filehash +","+filesize +";" \
                                                   <<"TV"<<dlg.getStoragePrice()<<dlg.getBackUpNumber()<<"1" \
                                                   <<dlg.getPayCycle()<<"node_id"<<"1"));
        emit uploadSuccess(filename, filesize);

    }
}

/*
*upload error
*/
void UploadBar::upLoadError(QNetworkReply::NetworkError errorCode)
{
    QMessageBox::warning(this, tr("warning"), tr("upload fail"), QMessageBox::Ok);
    //qDebug() << "upLoadError  errorCode: " << (int)errorCode;
}

/*
*deal with uploadProgress
*/
void UploadBar::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    //qDebug() << "bytesSent: " << bytesSent << "  bytesTotal: "<< bytesTotal;
}
