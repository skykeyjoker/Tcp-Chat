#ifndef TCPSOCKETTHREAD_H
#define TCPSOCKETTHREAD_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QDataStream>
#include <QFile>

class TcpSocketThread : public QObject
{
    Q_OBJECT
public:
    TcpSocketThread(QObject *parent = nullptr);

public slots:
    void startWork();
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
    QTcpSocket *_socket;
    quint32 sizePack = 0;
    int imageIndex = 0;

};

#endif // TCPSOCKETTHREAD_H
