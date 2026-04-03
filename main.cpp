#include <QCoreApplication>
#include "echotcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Starting Echo Server...";

    EchoTcpServer server;

    qDebug() << "Server is running. Press Ctrl+C to stop.";

    return a.exec();
}
