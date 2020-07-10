#ifndef TCPSERVERTHREAD_H
#define TCPSERVERTHREAD_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QFile>

class TcpServerThread : public QObject
{
    Q_OBJECT
public:
    TcpServerThread(QObject *parent = nullptr);

public slots:
    void startWork();
    //void writeToClinets(QTcpSocket* const &, QByteArray const &);
    void onNewConnection();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError);
    void onReadyRead();
    void writeText(QString const &);
    void writeImage(QString const &);
    void stopWork();

signals:
    void workStarted();
    void workNotStarted();
    void workStopped();
    void receiveText(QByteArray const &);
    void receiveImage(QString const &);

private:
    QTcpServer *_server;
    QList<QTcpSocket*> _clientList;
    quint32 sizePack = 0;
    int imageIndex = 0;

};

#endif // TCPSERVERTHREAD_H
