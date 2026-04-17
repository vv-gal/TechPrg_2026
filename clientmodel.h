#ifndef CLIENTMODEL_H
#define CLIENTMODEL_H

#include <QObject>

class ClientModel : public QObject
{
    Q_OBJECT

public:
    explicit ClientModel(QObject *parent = nullptr);
    QString currentLogin() const { return m_currentLogin; }
    void setCurrentLogin(const QString &login) { m_currentLogin = login; }

private:
    QString m_currentLogin;
};

#endif
