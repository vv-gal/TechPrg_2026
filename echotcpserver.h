#ifndef ECHOTGPSERVER_H
#define ECHOTGPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QStringList>

class EchoTcpServer : public QObject
{
    Q_OBJECT

public:
    explicit EchoTcpServer(QObject *parent = nullptr);
    ~EchoTcpServer();

public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();

private:
    QString processRequest(const QString& request);  // НОВЫЙ МЕТОД

private:
    QTcpServer* mTcpServer;
    QTcpSocket* mTcpSocket;
};

#endif // ECHOTGPSERVER_H
