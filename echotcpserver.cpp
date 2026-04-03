#include "echotcpserver.h"
#include "mathsolver.h"  // ← ДОБАВИТЬ ЭТУ СТРОКУ!

EchoTcpServer::EchoTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &EchoTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "Server is not started!";
    } else {
        qDebug() << "Task server is started on port 33333";
    }
}

EchoTcpServer::~EchoTcpServer()
{
    if (mTcpServer) {
        mTcpServer->close();
    }
}

void EchoTcpServer::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    qDebug() << "New client connected!";

    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &EchoTcpServer::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected,
            this, &EchoTcpServer::slotClientDisconnected);
}

void EchoTcpServer::slotServerRead()
{
    while (mTcpSocket->bytesAvailable() > 0) {
        QByteArray array = mTcpSocket->readAll();
        QString request = QString::fromUtf8(array).trimmed();

        qDebug() << "Received:" << request;

        QString response = processRequest(request);

        qDebug() << "Response:" << response;

        mTcpSocket->write(response.toUtf8());
        mTcpSocket->write("\n");
    }
}

QString EchoTcpServer::processRequest(const QString& request)
{
    if (request.startsWith("auth&")) {
        QStringList parts = request.split('&');
        if (parts.size() >= 3) {
            QString login = parts[1];
            QString password = parts[2];

            if (login == "admin" && password == "123") {
                return "auth+&" + login;
            } else {
                return "auth-";
            }
        }
        return "auth-";
    }
    else if (request.startsWith("reg&")) {
        QStringList parts = request.split('&');
        if (parts.size() >= 4) {
            QString login = parts[1];
            QString password = parts[2];
            QString email = parts[3];

            qDebug() << "Register:" << login << email;
            return "reg+&" + login;
        }
        return "reg-";
    }
    else if (request.startsWith("stat&")) {
        QStringList parts = request.split('&');
        if (parts.size() >= 2) {
            QString login = parts[1];
            return "stat&3&6&21";
        }
        return "stat&0&0&0";
    }
    else if (request.startsWith("check&")) {
        QStringList parts = request.split('&');
        if (parts.size() >= 4) {
            int taskNumber = parts[1].toInt();
            QString variant = parts[2];
            QString answer = parts[3];

            qDebug() << "Check task:" << taskNumber << variant << answer;

            // ← ИСПРАВЛЕНО: вызываем MathSolver!
            bool correct = MathSolver::check(taskNumber, variant, answer);

            if (correct) {
                return "check+";
            } else {
                return "check-";
            }
        }
        return "check-";
    }

    return "auth-";
}

void EchoTcpServer::slotClientDisconnected()
{
    qDebug() << "Client disconnected!";
    if (mTcpSocket) {
        mTcpSocket->close();
    }
}
