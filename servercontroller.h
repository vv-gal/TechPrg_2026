#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class DatabaseManager;
class ServerView;

class ServerController : public QTcpServer
{
    Q_OBJECT

public:
    explicit ServerController(ServerView *view, QObject *parent = nullptr);
    ~ServerController();

    bool startServer(quint16 port = 33333);
    void stopServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QString processRequest(const QString &request, QTcpSocket *socket);
    QString handleAuth(const QString &login, const QString &password, QTcpSocket *socket);
    QString handleReg(const QString &login, const QString &password, const QString &email);
    QString handleStat(const QString &login);
    QString handleCheck(int taskNumber, int variant, const QString &answer, QTcpSocket *socket);
    QString handleSolve(double a, double b);

    ServerView *m_view;
    DatabaseManager *m_dbManager;
    QMap<QTcpSocket*, QString> m_clients;      // Хранит логины клиентов
    QMap<QTcpSocket*, QString> m_buffer;       // Буфер для неполных сообщений
};

#endif
