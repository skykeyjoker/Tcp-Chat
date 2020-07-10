#include "tcpmsgclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpMsgClient w;
    w.show();
    return a.exec();
}
