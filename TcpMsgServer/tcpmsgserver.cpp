#include "tcpmsgserver.h"
#include "ui_tcpmsgserver.h"

TcpMsgServer::TcpMsgServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpMsgServer)
{
    ui->setupUi(this);

    tcpThread = new QThread;

    tcpSubThread = new TcpServerThread;
    tcpSubThread->moveToThread(tcpThread);

    // Start thread
    connect(tcpThread, &QThread::started, tcpSubThread, &TcpServerThread::startWork);

    // Receive text
    void (TcpServerThread::*pSignalReceiveText)(QByteArray const &) = &TcpServerThread::receiveText;
    void (TcpMsgServer::*pSlotReceiveText)(QByteArray const &) = &TcpMsgServer::receiveText;
    connect(tcpSubThread,pSignalReceiveText,this,pSlotReceiveText);
    // Receive photo
    void (TcpServerThread::*pSignalReceiveImage)(QString const &) = &TcpServerThread::receiveImage;
    void (TcpMsgServer::*pSlotReceiveImage)(QString const &) = &TcpMsgServer::receiveImage;
    connect(tcpSubThread, pSignalReceiveImage, this, pSlotReceiveImage);

    // Write text
    void (TcpMsgServer::*pSignalWriteText)(QString const &) = &TcpMsgServer::writeText;
    void (TcpServerThread::*pSlotWriteText)(QString const &) = &TcpServerThread::writeText;
    connect(this, pSignalWriteText, tcpSubThread, pSlotWriteText);
    //Write photo
    void (TcpMsgServer::*pSignalWriteImage)(QString const &) = &TcpMsgServer::writeImage;
    void (TcpServerThread::*pSlotWriteImage)(QString const &) = &TcpServerThread::writeImage;
    connect(this, pSignalWriteImage, tcpSubThread, pSlotWriteImage);

    tcpThread->start();

    ui->textEdit_browser->setReadOnly(true);

}

TcpMsgServer::~TcpMsgServer()
{
    // Destroy thread
    connect(this, &TcpMsgServer::stopServer, tcpSubThread, &TcpServerThread::stopWork);
    connect(tcpSubThread, &TcpServerThread::workStopped, tcpSubThread, &TcpServerThread::deleteLater);
    connect(tcpSubThread, &TcpServerThread::destroyed, tcpThread, &QThread::quit);
    connect(tcpThread, &QThread::finished, tcpThread, &QThread::deleteLater);

    emit stopServer();
    delete ui;
}

void TcpMsgServer::receiveText(const QByteArray & text)
{
    //插入文本
    //ui->textEdit_browser->append(text);
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");
    ui->textEdit_browser->insertHtml("<p>"+QString::fromUtf8(text)+"</p><br>");
}

void TcpMsgServer::receiveImage(const QString & fileName)
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");

    QString htmlTag;
    htmlTag = tr("<img src=%1></img><br>").arg(fileName);

    //将图片插入
    ui->textEdit_browser->insertHtml(htmlTag);
}

void TcpMsgServer::on_btn_send_clicked()
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_browser->append(datetime+"\n");

    //ui->textEdit_browser->append(ui->textEdit_text->toPlainText());
    ui->textEdit_browser->insertHtml("<p>"+ui->textEdit_text->toPlainText().toUtf8()+"</p><br>");

    emit writeText(ui->textEdit_text->toPlainText().toUtf8());


    ui->textEdit_text->clear();
}

void TcpMsgServer::on_btn_image_clicked()
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
