#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QCryptographicHash>

class ServerView;

class DatabaseManager
{
public:
    explicit DatabaseManager(const QString &dbPath, ServerView *view = nullptr);
    ~DatabaseManager();

    bool initialize();
    bool registerUser(const QString &username, const QString &password, const QString &email);
    bool authenticateUser(const QString &username, const QString &password);
    bool userExists(const QString &username);

    bool getUserStats(const QString &login, int &totalAttempts, int &solvedTasks, int &currentScore);
    bool checkSolution(int taskNumber, int variant, const QString &answer, const QString &login);
    void updateUserStats(const QString &login, bool solved);
    QString solveEquation(double a, double b);

private:
    bool createTables();
    QString hashPassword(const QString &password);
    void logMessage(const QString &message);

    QSqlDatabase m_database;
    QString m_dbPath;
    ServerView *m_view;
};

#endif
