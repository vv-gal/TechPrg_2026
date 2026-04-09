#include "servercontroller.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>

ServerController::ServerController(QObject *parent) : QObject(parent) {}

ServerController& ServerController::instance()
{
    static ServerController instance;
    return instance;
}

void ServerController::registerUser(const QString &username, const QString &email, const QString &password)
{
    QTimer::singleShot(1000, [this, username, email, password]() {
        if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
            emit registrationCompleted(false, "Все поля должны быть заполнены!", "");
            return;
        }
        if (password.length() < 6) {
            emit registrationCompleted(false, "Пароль должен содержать минимум 6 символов!", "");
            return;
        }

        QString code = generateVerificationCode();
        qDebug() << "=====================================";
        qDebug() << "Код подтверждения для" << email << ":" << code;
        qDebug() << "=====================================";

        emit registrationCompleted(true, QString("Код отправлен на %1").arg(email), code);
    });
}

void ServerController::loginUser(const QString &email, const QString &password)
{
    QTimer::singleShot(800, [this, email, password]() {
        if (email.contains("@") && !password.isEmpty()) {
            m_currentUser.username = email.split("@").first();
            m_currentUser.email = email;
            m_currentUser.token = "token_" + QString::number(QRandomGenerator::global()->generate());
            m_currentUser.isAuthenticated = true;
            emit loginCompleted(true, "Добро пожаловать!", m_currentUser);
        } else {
            emit loginCompleted(false, "Неверный email или пароль!", UserData());
        }
    });
}

void ServerController::verifyEmail(const QString &email, const QString &code)
{
    QTimer::singleShot(500, [this, email, code]() {
        if (code == "123456") {
            emit verificationCompleted(true, "Email подтвержден!");
        } else {
            emit verificationCompleted(false, "Неверный код!");
        }
    });
}

QString ServerController::generateVerificationCode() const
{
    int code = QRandomGenerator::global()->bounded(100000, 999999);
    return QString::number(code);
}
