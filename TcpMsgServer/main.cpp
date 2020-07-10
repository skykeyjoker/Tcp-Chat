#include "tcpmsgserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpMsgServer w;
    w.show();
    return a.exec();
}
