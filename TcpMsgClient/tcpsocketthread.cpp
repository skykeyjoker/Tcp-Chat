#include "tcpsocketthread.h"

TcpSocketThread::TcpSocketThread(QObject *parent) : QObject(parent)
{

}

void TcpSocketThread::startWork()
{
    _socket = new QTcpSocket;

    connect(_socket, &QTcpSocket::readyRead,
            this, &TcpSocketThread::onReadyRead);
    connect(_socket, &QTcpSocket::connected,
            this, &TcpSocketThread::onConnected);
    connect(_socket, &QTcpSocket::disconnected,
            this, &TcpSocketThread::onDisconnected);
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));

    _socket->connectToHost("127.0.0.1",9999);

    if(!_socket->isValid())
    {
        emit workNotStarted();
        exit(0);
    }

    emit workStarted();
}

void TcpSocketThread::onConnected()
{
    //连接成功
    qDebug()<<"connected";
}

void TcpSocketThread::onDisconnected()
{
    //连接断开
    qDebug()<<"disconnected";
}

void TcpSocketThread::onError(QAbstractSocket::SocketError error)
{
    //错误
    qDebug()<<"error: "<<error;
}

void TcpSocketThread::onReadyRead()
{
    quint64 sizeNow = 0;
    //当前缓存区内数据大小

    do  //出问题直接删do循环！！！
    {
        //获取包头
        if(sizePack == 0)
        {
            QDataStream in(_socket);

            //如果包头没有完整接收
            if(_socket->bytesAvailable() < sizeof(quint32))
                return;
            in >>sizePack;
            qDebug()<<sizePack;
        }


        qDebug()<<"current: "<<_socket->bytesAvailable();

        //包不完整，等待包完整
        if(_socket->bytesAvailable() < sizePack)
            return;

        //包接受完整
        qDebug()<<"full pack";
        QByteArray dataFull;
        dataFull=_socket->read(sizePack);
        //stream>>dataFull;
        //qDebug()<<dataFull;

        //判断剩下的字节数，是否会有粘包情况
        sizeNow = _socket->bytesAvailable();

        if(dataFull.mid(4,4) == "TXT:")  //文本
            emit receiveText(dataFull.mid(8,-1));
        else  //图片
        {
            qDebug()<<"photo";
            QString content(dataFull.mid(0,20));

            QFile file;
            QString fileName;

            if(dataFull.mid(0,20).contains("PNG"))
            {
                file.setFileName(QString::number(imageIndex)+".png");
                fileName = QString::number(imageIndex)+".png";
            }
            else
                if(dataFull.mid(0,20).contains("JFIF"))
                {
                    file.setFileName(QString::number(imageIndex)+".jpg");
                    fileName = QString::number(imageIndex)+".jpg";
                }
                else
                {
                    file.setFileName(QString::number(imageIndex)+".png");
                    fileName = QString::number(imageIndex)+".png";
                }

            //写入图片文件
            file.open(QFile::WriteOnly | QFile::Truncate);
            file.write(dataFull.mid(8));
            file.close();

            imageIndex++;

            //发送接收图片消息
            emit receiveImage(fileName);

        }
    } while(sizeNow > 0);

    //多次接受信息，每次接受完后将包大小清零
    sizePack = 0;
}

void TcpSocketThread::writeText(QString const &text)
{
    QString buf = ("TXT:"+text).toUtf8();
    //封装包头
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << (quint32)buf.toUtf8().size()+(quint32)sizeof(quint32) << buf.toUtf8();

    _socket->write(data);
}

void TcpSocketThread::writeImage(QString const &fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QByteArray buf = "IMG:" + file.readAll();
    file.close();

    //封装包头
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    quint32 dataSize = (quint32)buf.size()+(quint32)sizeof(quint32);
    stream << dataSize;
    stream << buf;

    _socket->write(data);
}

void TcpSocketThread::stopWork()
{
    if(_socket->isValid())
    {
        _socket->disconnectFromHost();
        _socket->deleteLater();
    }

    qDebug()<<"Server stoped";
    emit workStopped();
}
