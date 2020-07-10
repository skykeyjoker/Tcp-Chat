#include "tcpserverthread.h"

TcpServerThread::TcpServerThread(QObject *parent) : QObject(parent)
{

}

void TcpServerThread::startWork()
{
    _server = new QTcpServer;

    bool ret = _server->listen(QHostAddress::Any, 9999);

    if(!ret)
    {
        emit workNotStarted();
        exit(1);
    }

    emit workStarted();


    connect(_server, &QTcpServer::newConnection,
            this, &TcpServerThread::onNewConnection);


}

void TcpServerThread::onNewConnection()
{
    //使用socket与客户端进行通信
    QTcpSocket* socket = _server->nextPendingConnection();
    _clientList.append(socket);

    //连接信号与槽
    connect(socket, &QTcpSocket::disconnected,
            this, &TcpServerThread::onDisconnected);
    connect(socket, &QTcpSocket::connected,
            this, &TcpServerThread::onConnected);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(socket, &QTcpSocket::readyRead,
            this, &TcpServerThread::onReadyRead);
}

void TcpServerThread::onConnected()
{
    //连接成功
    qDebug()<<"New client connected";
}

void TcpServerThread::onDisconnected()
{
    //连接断开
    QTcpSocket* socket = (QTcpSocket*)sender();

    _clientList.removeAll(socket);

    socket->deleteLater();

    qDebug()<<"Clinet disconnected";
}

void TcpServerThread::onError(QAbstractSocket::SocketError error)
{
    //错误
    qDebug()<<"error:"<<error;
}

void TcpServerThread::onReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    //QByteArray buf = socket->readAll();

    quint64 sizeNow = 0;
    //当前缓存区内数据大小

    QByteArray dataFull; //完整的socket包

    do  //出问题直接删do循环！！！
    {
        //获取包头
        if(sizePack == 0)
        {
            QDataStream in(socket);

            //如果包头没有完整接收
            if(socket->bytesAvailable() < sizeof(quint32))
                return;
            in >>sizePack;
            qDebug()<<sizePack;
        }


        qDebug()<<"current: "<<socket->bytesAvailable();

        //包不完整，等待包完整
        if(socket->bytesAvailable() < sizePack - 4)
            return;

        //包接受完整
        qDebug()<<"full pack";

        dataFull=socket->read(sizePack);
        //stream>>dataFull;

        //判断剩下的字节数，是否会有粘包情况
        sizeNow = socket->bytesAvailable();

        if(dataFull.mid(4,4) == "TXT:")  //文本
        {
            //发送插入文本消息
            emit receiveText(dataFull.mid(8,-1));

            //转发到其他客户端
            QByteArray buf = "TXT:"+dataFull.mid(8,-1);
            //封装包头
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << (quint32)QString(buf).toUtf8().size()+(quint32)sizeof(quint32) << QString(buf).toUtf8();

            for(QList<QTcpSocket*>::iterator itr = _clientList.begin(); itr != _clientList.end(); ++itr)
            {
                QTcpSocket* currentSocket = *itr;

                if(currentSocket != socket)
                    currentSocket->write(data);
            }
        }
        else  //图片
        {
            qDebug()<<"photo";
            QString content(dataFull.mid(0,20));


            QString fileName;
            QFile file;

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

            //发送图片消息
            emit receiveImage(fileName);


            //转发到其他客户端
            QByteArray buf = "IMG:" + dataFull.mid(8);
            //封装包头
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            quint32 dataSize = (quint32)buf.size()+(quint32)sizeof(quint32);
            stream << dataSize;
            stream << buf;

            for(QList<QTcpSocket*>::iterator itr = _clientList.begin(); itr != _clientList.end(); ++itr)
            {
                QTcpSocket* currentSocket = *itr;

                if(currentSocket != socket)
                    currentSocket->write(data);
            }
        }
    } while(sizeNow > 0);

    //多次接受信息，每次接受完后将包大小清零
    sizePack = 0;

}

void TcpServerThread::writeText(QString const &text)
{
    QString buf = ("TXT:"+text).toUtf8();
    //封装包头
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << (quint32)buf.toUtf8().size()+(quint32)sizeof(quint32) << buf.toUtf8();


    for(QList<QTcpSocket*>::iterator itr = _clientList.begin(); itr != _clientList.end(); ++itr)
    {
        QTcpSocket* socket = *itr;

        socket->write(data);
    }
}

void TcpServerThread::writeImage(QString const &fileName)
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


    for(QList<QTcpSocket*>::iterator itr = _clientList.begin(); itr != _clientList.end(); ++itr)
    {
        QTcpSocket* socket = *itr;

        socket->write(data);

    }
}

void TcpServerThread::stopWork()
{
    if(_server->isListening())
    {
        _server->pauseAccepting();
        _server->disconnect();
        _server->deleteLater();
    }

    qDebug()<<"Server stoped";
    emit workStopped();
}
