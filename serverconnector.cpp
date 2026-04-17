#include "network/serverconnector.h"
#include <QDebug>
#include <QDateTime>

ServerConnector* ServerConnector::m_instance = nullptr;
ServerConnector::Destroyer ServerConnector::m_destroyer;

ServerConnector* ServerConnector::getInstance()
{
    if (!m_instance) {
        m_instance = new ServerConnector();
        m_destroyer.initialize(m_instance);
    }
    return m_instance;
}

ServerConnector::ServerConnector(QObject *parent)
    : QObject(parent), m_socket(nullptr), m_requestCounter(0)
{
}

ServerConnector::~ServerConnector()
{
    disconnectFromServer();
}

bool ServerConnector::connectToServer(const QString& host, quint16 port)
{
    if (m_socket) {
        disconnectFromServer();
    }

    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &ServerConnector::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ServerConnector::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ServerConnector::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &ServerConnector::onReadyRead);

    m_socket->connectToHost(host, port);

    bool connected = m_socket->waitForConnected(5000);
    return connected;
}

void ServerConnector::disconnectFromServer()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    m_buffer.clear();
    m_pendingCallbacks.clear();
}

void ServerConnector::sendRequest(const QString& request, std::function<void(const QString&)> callback)
{
    if (!isConnected()) {
        qDebug() << "Not connected to server";
        if (callback) callback("ERROR: Not connected");
        return;
    }

    QString requestId = QString::number(m_requestCounter++);

    if (callback) {
        m_pendingCallbacks[requestId] = callback;
    }

    QString fullRequest = request + "\n";
    m_socket->write(fullRequest.toUtf8());
    m_socket->flush();

    qDebug() << "Sent:" << request;
}

void ServerConnector::onReadyRead()
{
    m_buffer += m_socket->readAll();

    QStringList messages = m_buffer.split('\n', Qt::SkipEmptyParts);

    if (messages.size() > 0 && !m_buffer.endsWith('\n')) {
        m_buffer = messages.takeLast();
    } else {
        m_buffer.clear();
    }

    for (const QString& response : messages) {
        qDebug() << "Received:" << response;

        if (!m_pendingCallbacks.isEmpty()) {
            auto it = m_pendingCallbacks.begin();
            if (it.value()) {
                it.value()(response);
            }
            m_pendingCallbacks.erase(it);
        }
    }
}

void ServerConnector::onConnected()
{
    qDebug() << "Connected to server";
    emit connected();
}

void ServerConnector::onDisconnected()
{
    qDebug() << "Disconnected from server";
    emit disconnected();
}

void ServerConnector::onError(QAbstractSocket::SocketError /*error*/)  // ← Убрали unused parameter
{
    QString errorMsg = QString("Socket error: %1").arg(m_socket->errorString());
    qDebug() << errorMsg;
    emit errorOccurred(errorMsg);
}

void ServerConnector::auth(const QString& login, const QString& password,
                           std::function<void(bool, const QString&)> callback)
{
    QString request = QString("auth&%1&%2").arg(login).arg(password);

    sendRequest(request, [callback, login](const QString& response) {
        if (response.startsWith("auth+")) {
            QString returnedLogin = response.mid(6);
            if (callback) callback(true, returnedLogin);
        } else {
            if (callback) callback(false, "");
        }
    });
}

void ServerConnector::reg(const QString& login, const QString& password, const QString& email,
                          std::function<void(bool, const QString&)> callback)
{
    QString request = QString("reg&%1&%2&%3").arg(login).arg(password).arg(email);

    sendRequest(request, [callback, login](const QString& response) {
        if (response.startsWith("reg+")) {
            QString returnedLogin = response.mid(6);
            if (callback) callback(true, returnedLogin);
        } else {
            if (callback) callback(false, "");
        }
    });
}

void ServerConnector::getStats(const QString& login, std::function<void(int, int, int)> callback)
{
    QString request = QString("stat&%1").arg(login);

    sendRequest(request, [callback](const QString& response) {
        if (response.startsWith("stat&")) {
            QStringList parts = response.split('&');
            if (parts.size() >= 4) {
                int attempts = parts[1].toInt();
                int solved = parts[2].toInt();
                int score = parts[3].toInt();
                if (callback) callback(attempts, solved, score);
                return;
            }
        }
        if (callback) callback(0, 0, 0);
    });
}

void ServerConnector::checkSolution(int taskNumber, int variant, const QString& answer,
                                    std::function<void(bool)> callback)
{
    QString request = QString("check&%1&%2&%3").arg(taskNumber).arg(variant).arg(answer);

    sendRequest(request, [callback](const QString& response) {
        if (callback) callback(response == "check+");
    });
}

void ServerConnector::solveEquation(double a, double b,
                                    std::function<void(const QString&)> callback)
{
    QString request = QString("solve&%1&%2").arg(a).arg(b);
    sendRequest(request, callback);
}
