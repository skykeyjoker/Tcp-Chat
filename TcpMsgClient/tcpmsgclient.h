#ifndef TCPMSGCLIENT_H
#define TCPMSGCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QFileDialog>
#include <QThread>
#include <QFileInfo>
#include <QDateTime>

#include <QDebug>

#include "tcpsocketthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpMsgClient; }
QT_END_NAMESPACE

class TcpMsgClient : public QWidget
{
    Q_OBJECT

public:
    TcpMsgClient(QWidget *parent = nullptr);
    ~TcpMsgClient();

    quint32 sizePack = 0;

public slots:
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
    void stopSocket();
    void writeText(QString const &);
    void writeImage(QString const &);

private:
    Ui::TcpMsgClient *ui;
    //QTcpSocket *tcpSocket;
    int imageIndex = 0;

    QThread *socketThread;
    TcpSocketThread *socketSubThread;
};
#endif // TCPMSGCLIENT_H
