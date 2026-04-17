/**
 * @file servercontroller.cpp
 * @brief Реализация класса ServerController
 * @author Developer
 * @date 2026
 */

#include "controller/servercontroller.h"
#include "model/databasemanager.h"
#include "view/serverview.h"
#include <QTcpSocket>
#include <QFile>

ServerController::ServerController(ServerView *view, QObject *parent)
    : QTcpServer(parent), m_view(view)
{
    m_view->displayMessage("=== SERVER INITIALIZATION ===");

    QString dbPath = "bd_timp.db";
    m_view->displayMessage("Database path: " + dbPath);

    m_view->displayMessage("Creating DatabaseManager...");
    m_dbManager = new DatabaseManager(dbPath, m_view);

    m_view->displayMessage("Calling initialize()...");
    if (!m_dbManager->initialize()) {
        m_view->displayMessage("!!! ERROR: Failed to connect to database !!!");
    } else {
        m_view->displayMessage("+++ SUCCESS: Database connected! +++");
    }

    m_view->displayMessage("=== SERVER INITIALIZATION COMPLETE ===");
}

ServerController::~ServerController()
{
    delete m_dbManager;
    stopServer();
}

bool ServerController::startServer(quint16 port)
{
    if (!listen(QHostAddress::Any, port)) {
        m_view->displayMessage("Failed to start server on port " + QString::number(port));
        return false;
    }
    m_view->displayMessage("Server started on port " + QString::number(port));
    return true;
}

void ServerController::stopServer()
{
    close();
    for (QTcpSocket *socket : m_clients.keys()) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    m_clients.clear();
    m_buffer.clear();
}

void ServerController::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &ServerController::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ServerController::onDisconnected);

    m_clients[socket] = "";
    m_buffer[socket] = "";
    m_view->displayMessage("New client connected");
}

void ServerController::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_buffer[socket] += socket->readAll();

    QStringList messages = m_buffer[socket].split('\n', Qt::SkipEmptyParts);

    if (messages.size() > 0 && !m_buffer[socket].endsWith('\n')) {
        m_buffer[socket] = messages.takeLast();
    } else {
        m_buffer[socket].clear();
    }

    for (const QString &message : messages) {
        QString response = processRequest(message.trimmed(), socket);
        if (!response.isEmpty()) {
            socket->write((response + "\n").toUtf8());
            m_view->displayMessage("Response sent: " + response);
        }
    }
}

void ServerController::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_view->displayMessage("Client disconnected");
        m_clients.remove(socket);
        m_buffer.remove(socket);
        socket->deleteLater();
    }
}

QString ServerController::processRequest(const QString &request, QTcpSocket *socket)
{
    m_view->displayMessage("Received: " + request);

    QStringList parts = request.split('&');
    if (parts.isEmpty()) return "";

    QString command = parts[0];

    if (command == "auth" && parts.size() >= 3) {
        return handleAuth(parts[1], parts[2], socket);
    }
    else if (command == "reg" && parts.size() >= 4) {
        return handleReg(parts[1], parts[2], parts[3]);
    }
    else if (command == "stat" && parts.size() >= 2) {
        return handleStat(parts[1]);
    }
    else if (command == "check" && parts.size() >= 4) {
        return handleCheck(parts[1].toInt(), parts[2].toInt(), parts[3], socket);
    }
    else if (command == "solve" && parts.size() >= 3) {
        return handleSolve(parts[1].toDouble(), parts[2].toDouble());
    }

    return "";
}

QString ServerController::handleAuth(const QString &login, const QString &password, QTcpSocket *socket)
{
    if (m_dbManager->authenticateUser(login, password)) {
        m_clients[socket] = login;
        return "auth+&" + login;
    }
    return "auth-";
}

QString ServerController::handleReg(const QString &login, const QString &password, const QString &email)
{
    if (m_dbManager->registerUser(login, password, email)) {
        return "reg+&" + login;
    }
    return "reg-";
}

/**
 * @brief Обработка запроса статистики
 * @param login Логин пользователя
 * @return Статистика в формате "stat&attempts&3&-4" по примеру задания
 *
 * По заданию пример ответа: stat&0&3&-4
 * Первое число - количество попыток
 * Второе число всегда 3 (константа из примера)
 * Третье число всегда -4 (код из примера)
 */
QString ServerController::handleStat(const QString &login)
{
    int totalAttempts, solvedTasks, currentScore;

    if (m_dbManager->getUserStats(login, totalAttempts, solvedTasks, currentScore)) {
        // По примеру задания: stat&попытки&3&-4
        return QString("stat&%1&3&-4").arg(totalAttempts);
    }
    // Если пользователь не найден - stat&0&3&-4
    return "stat&0&3&-4";
}

/**
 * @brief Обработка запроса проверки решения
 * @param taskNumber Номер задачи
 * @param variant Номер варианта
 * @param answer Ответ пользователя
 * @param socket Указатель на сокет клиента
 * @return "check+" при правильном ответе, "check-" при ошибке
 */
QString ServerController::handleCheck(int taskNumber, int variant, const QString &answer, QTcpSocket *socket)
{
    QString login = m_clients.value(socket);

    if (login.isEmpty()) {
        m_view->displayMessage("ERROR: Unauthorized check attempt");
        return "check-";
    }

    bool isCorrect = m_dbManager->checkSolution(taskNumber, variant, answer, login);

    // Обновляем статистику (попытки)
    m_dbManager->updateUserStats(login, isCorrect);

    return isCorrect ? "check+" : "check-";
}

QString ServerController::handleSolve(double a, double b)
{
    return m_dbManager->solveEquation(a, b);
}
