#include "tcpmsgclient.h"
#include "ui_tcpmsgclient.h"

TcpMsgClient::TcpMsgClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpMsgClient)
{
    ui->setupUi(this);

    socketThread = new QThread;

    socketSubThread = new TcpSocketThread;
    socketSubThread->moveToThread(socketThread);


    // Start thread
    connect(socketThread, &QThread::started, socketSubThread, &TcpSocketThread::startWork);

    // Receive text
    void (TcpSocketThread::*pSignalReceiveText)(QByteArray const &) = &TcpSocketThread::receiveText;
    void (TcpMsgClient::*pSlotReceiveText)(QByteArray const &) = &TcpMsgClient::receiveText;
    connect(socketSubThread,pSignalReceiveText,this,pSlotReceiveText);
    // Receive photo
    void (TcpSocketThread::*pSignalReceiveImage)(QString const &) = &TcpSocketThread::receiveImage;
    void (TcpMsgClient::*pSlotReceiveImage)(QString const &) = &TcpMsgClient::receiveImage;
    connect(socketSubThread, pSignalReceiveImage, this, pSlotReceiveImage);

    // Write text
    void (TcpMsgClient::*pSignalWriteText)(QString const &) = &TcpMsgClient::writeText;
    void (TcpSocketThread::*pSlotWriteText)(QString const &) = &TcpSocketThread::writeText;
    connect(this, pSignalWriteText, socketSubThread, pSlotWriteText);
    //Write photo
    void (TcpMsgClient::*pSignalWriteImage)(QString const &) = &TcpMsgClient::writeImage;
    void (TcpSocketThread::*pSlotWriteImage)(QString const &) = &TcpSocketThread::writeImage;
    connect(this, pSignalWriteImage, socketSubThread, pSlotWriteImage);



    socketThread->start();

    ui->textEdit_browser->setReadOnly(true);
}

TcpMsgClient::~TcpMsgClient()
{
    // Destroy thread
    connect(this, &TcpMsgClient::stopSocket, socketSubThread, &TcpSocketThread::stopWork);
    connect(socketSubThread, &TcpSocketThread::workStopped, socketSubThread, &TcpSocketThread::deleteLater);
    connect(socketSubThread, &TcpSocketThread::destroyed, socketThread, &QThread::quit);
    connect(socketThread, &QThread::finished, socketThread, &QThread::deleteLater);

    emit stopSocket();

    delete ui;
}

void TcpMsgClient::receiveText(const QByteArray & text)
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");

    //插入文本
    //ui->textEdit_browser->append(text);
    ui->textEdit_browser->insertHtml("<p>"+QString::fromUtf8(text)+"</p><br>");
    qDebug()<<text;

}

void TcpMsgClient::receiveImage(const QString & fileName)
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");

    QString htmlTag;
    htmlTag = tr("<img src=%1></img><br>").arg(fileName);

    //将图片插入
    ui->textEdit_browser->insertHtml(htmlTag);
}

void TcpMsgClient::on_btn_send_clicked()
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");

    ui->textEdit_browser->insertHtml("<p>"+ui->textEdit_text->toPlainText().toUtf8()+"</p><br>");

    emit writeText(ui->textEdit_text->toPlainText().toUtf8());

    ui->textEdit_text->clear();
}

void TcpMsgClient::on_btn_image_clicked()
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");

    QString imageName = QFileDialog::getOpenFileName(this, "Select a image",".","Images (*.png *.xpm *.jpg)");

    if(imageName.isEmpty())
        return;

    QString htmlTag;
    htmlTag = tr("<img src=%1></img><br>").arg(imageName);
    qDebug()<<"htmlTag: "<<htmlTag;
    //将图片插入
    ui->textEdit_browser->insertHtml(htmlTag);

    emit writeImage(imageName);

}
