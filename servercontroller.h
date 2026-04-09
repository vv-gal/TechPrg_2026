#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QString>

class ServerController : public QObject
{
    Q_OBJECT
private:
    explicit ServerController(QObject *parent = nullptr);

public:
    static ServerController& instance();

    struct UserData {
        QString username;
        QString email;
        QString token;
        bool isAuthenticated = false;
    };

    void registerUser(const QString &username, const QString &email, const QString &password);
    void loginUser(const QString &email, const QString &password);
    void verifyEmail(const QString &email, const QString &code);

    UserData currentUser() const { return m_currentUser; }
    bool isAuthenticated() const { return m_currentUser.isAuthenticated; }

signals:
    void registrationCompleted(bool success, const QString &message, const QString &verificationCode);
    void loginCompleted(bool success, const QString &message, const UserData &user);
    void verificationCompleted(bool success, const QString &message);

private:
    UserData m_currentUser;
    QString generateVerificationCode() const;
};

#endif
