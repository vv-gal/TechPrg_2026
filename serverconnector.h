#ifndef SERVERCONNECTOR_H
#define SERVERCONNECTOR_H

#include <QTcpSocket>
#include <QObject>
#include <functional>
#include <QMap>

class ServerConnector : public QObject
{
    Q_OBJECT

public:
    static ServerConnector* getInstance();

    bool connectToServer(const QString& host = "localhost", quint16 port = 33333);
    void disconnectFromServer();
    bool isConnected() const { return m_socket && m_socket->state() == QTcpSocket::ConnectedState; }

    void sendRequest(const QString& request, std::function<void(const QString&)> callback = nullptr);

    void auth(const QString& login, const QString& password,
              std::function<void(bool, const QString&)> callback);

    void reg(const QString& login, const QString& password, const QString& email,
             std::function<void(bool, const QString&)> callback);

    void getStats(const QString& login, std::function<void(int, int, int)> callback);

    void checkSolution(int taskNumber, int variant, const QString& answer,
                       std::function<void(bool)> callback);

    void solveEquation(double a, double b, std::function<void(const QString&)> callback);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);

private:
    explicit ServerConnector(QObject *parent = nullptr);
    ~ServerConnector();

    ServerConnector(const ServerConnector&) = delete;
    ServerConnector& operator=(const ServerConnector&) = delete;

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    QTcpSocket* m_socket;
    QString m_buffer;
    QMap<QString, std::function<void(const QString&)>> m_pendingCallbacks;
    int m_requestCounter;

    static ServerConnector* m_instance;
    static class Destroyer {
    public:
        ~Destroyer() { delete m_instance; }
        void initialize(ServerConnector* p) { m_instance = p; }
    } m_destroyer;

    friend class Destroyer;
};

#endif // SERVERCONNECTOR_H
