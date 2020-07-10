#ifndef TCPMSGSERVER_H
#define TCPMSGSERVER_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QThread>
#include <QDateTime>

#include <QDebug>

#include "tcpserverthread.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TcpMsgServer; }
QT_END_NAMESPACE

class TcpMsgServer : public QWidget
{
    Q_OBJECT

public:
    TcpMsgServer(QWidget *parent = nullptr);
    ~TcpMsgServer();

    quint32 sizePack = 0;

public slots:
    //void onNewConnection();
    //void onConnected();
    //void onDisconnected();
    //void onError(QAbstractSocket::SocketError);
    //void onReadyRead();
    void receiveText(QByteArray const &);
    void receiveImage(QString const &);

private slots:
    void on_btn_send_clicked();

    void on_btn_image_clicked();

signals:
    void stopServer();
    void writeText(QString const &);
    void writeImage(QString const &);

private:
    Ui::TcpMsgServer *ui;
    QTcpServer *_server;
    QList<QTcpSocket*> socketList;
    int imageIndex = 0;

    QThread *tcpThread;
    TcpServerThread *tcpSubThread;
};
#endif // TCPMSGSERVER_H
