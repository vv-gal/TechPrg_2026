#include "model/databasemanager.h"
#include "model/functionSolver.h"
#include "view/serverview.h"
#include <QDebug>
#include <QSqlError>
#include <QFile>

DatabaseManager::DatabaseManager(const QString &dbPath, ServerView *view)
    : m_dbPath(dbPath), m_view(view)
{
    logMessage("[DB] Constructor called with path: " + m_dbPath);
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
        logMessage("[DB] Database closed");
    }
}

void DatabaseManager::logMessage(const QString &message)
{
    qDebug().noquote() << message;
    if (m_view) {
        m_view->displayMessage(message);
    }
}

bool DatabaseManager::initialize()
{
    logMessage("[DB] ========================================");
    logMessage("[DB] INITIALIZING DATABASE");
    logMessage("[DB] ========================================");

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    logMessage("[DB] Opening database...");

    if (!m_database.open()) {
        logMessage("[DB] ERROR: " + m_database.lastError().text());
        return false;
    }

    logMessage("[DB] SUCCESS: Database opened!");

    QFile dbFile(m_dbPath);
    if (dbFile.exists()) {
        logMessage("[DB] Database file exists at: " + m_dbPath);
        logMessage("[DB] File size: " + QString::number(dbFile.size()) + " bytes");
    } else {
        logMessage("[DB] Database file will be created at: " + m_dbPath);
    }

    return createTables();
}

bool DatabaseManager::createTables()
{
    logMessage("[DB] Creating tables if not exist...");

    QSqlQuery query;

    // Таблица пользователей
    QString createUsersTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "email TEXT NOT NULL, "
        "password_hash TEXT NOT NULL)";

    if (!query.exec(createUsersTable)) {
        logMessage("[DB] ERROR creating users table: " + query.lastError().text());
        return false;
    }

    logMessage("[DB] Table 'users' is ready");

    // Таблица статистики
    QString createStatsTable =
        "CREATE TABLE IF NOT EXISTS user_stats ("
        "username TEXT PRIMARY KEY, "
        "total_attempts INTEGER DEFAULT 0, "
        "solved_tasks INTEGER DEFAULT 0, "
        "current_score INTEGER DEFAULT 0)";

    if (!query.exec(createStatsTable)) {
        logMessage("[DB] ERROR creating stats table: " + query.lastError().text());
        return false;
    }

    logMessage("[DB] Table 'user_stats' is ready");

    // Подсчет пользователей
    if (query.exec("SELECT COUNT(*) FROM users") && query.next()) {
        int userCount = query.value(0).toInt();
        logMessage("[DB] Users in database: " + QString::number(userCount));
    }

    logMessage("[DB] ========================================");
    return true;
}

QString DatabaseManager::hashPassword(const QString &password)
{
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

bool DatabaseManager::registerUser(const QString &username, const QString &password, const QString &email)
{
    logMessage("[DB] Registering user: " + username);

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, email, password_hash) VALUES (:username, :email, :password_hash)");
    query.bindValue(":username", username);
    query.bindValue(":email", email);
    query.bindValue(":password_hash", hashPassword(password));

    if (query.exec()) {
        logMessage("[DB] SUCCESS: User registered: " + username);

        // Создаем запись статистики для нового пользователя
        QSqlQuery statsQuery;
        statsQuery.prepare("INSERT OR IGNORE INTO user_stats (username, total_attempts, solved_tasks, current_score) "
                           "VALUES (:username, 0, 0, 0)");
        statsQuery.bindValue(":username", username);
        statsQuery.exec();

        return true;
    } else {
        logMessage("[DB] ERROR: " + query.lastError().text());
        return false;
    }
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password)
{
    logMessage("[DB] Authenticating user: " + username);

    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username AND password_hash = :password_hash");
    query.bindValue(":username", username);
    query.bindValue(":password_hash", hashPassword(password));

    if (query.exec() && query.next()) {
        logMessage("[DB] SUCCESS: User authenticated: " + username);
        return true;
    }

    logMessage("[DB] FAILED: Authentication failed for: " + username);
    return false;
}

bool DatabaseManager::userExists(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username");
    query.bindValue(":username", username);
    return query.exec() && query.next();
}

bool DatabaseManager::getUserStats(const QString &login, int &totalAttempts, int &solvedTasks, int &currentScore)
{
    logMessage("[DB] Getting stats for user: " + login);

    QSqlQuery query;
    query.prepare("SELECT total_attempts, solved_tasks, current_score FROM user_stats WHERE username = :username");
    query.bindValue(":username", login);

    if (query.exec() && query.next()) {
        totalAttempts = query.value(0).toInt();
        solvedTasks = query.value(1).toInt();
        currentScore = query.value(2).toInt();
        return true;
    }

    // Если нет статистики, возвращаем нули
    totalAttempts = 0;
    solvedTasks = 0;
    currentScore = 0;
    return true;
}

bool DatabaseManager::checkSolution(int taskNumber, int variant, const QString &answer, const QString &login)
{
    QMap<QPair<int,int>, QString> correctAnswers;
    correctAnswers[qMakePair(1,1)] = "42";
    correctAnswers[qMakePair(1,2)] = "100";
    correctAnswers[qMakePair(2,1)] = "15";
    correctAnswers[qMakePair(2,2)] = "25";
    correctAnswers[qMakePair(3,1)] = "7";
    correctAnswers[qMakePair(3,2)] = "12";

    bool isCorrect = (answer == correctAnswers[qMakePair(taskNumber, variant)]);
    logMessage("[DB] Check task " + QString::number(taskNumber) + " variant " +
               QString::number(variant) + " = " + (isCorrect ? "correct" : "wrong") +
               " for user: " + login);
    return isCorrect;
}

void DatabaseManager::updateUserStats(const QString &login, bool solved)
{
    logMessage("[DB] Update stats for " + login + " solved: " + (solved ? "yes" : "no"));

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO user_stats (username, total_attempts, solved_tasks, current_score) "
                  "VALUES (:username, "
                  "COALESCE((SELECT total_attempts FROM user_stats WHERE username = :username), 0) + 1, "
                  "COALESCE((SELECT solved_tasks FROM user_stats WHERE username = :username), 0) + :solved, "
                  "COALESCE((SELECT current_score FROM user_stats WHERE username = :username), 0) + :points)");
    query.bindValue(":username", login);
    query.bindValue(":solved", solved ? 1 : 0);
    query.bindValue(":points", solved ? 10 : 0);

    if (!query.exec()) {
        logMessage("[DB] ERROR updating stats: " + query.lastError().text());
    }
}

QString DatabaseManager::solveEquation(double a, double b)
{
    logMessage("[DB] Solving equation with a=" + QString::number(a) + ", b=" + QString::number(b));

    FunctionSolver solver(a, b);
    QVector<SolutionPoint> roots = solver.findAllRoots();

    QString result;
    if (roots.isEmpty()) {
        result = "No roots found.\n";
    } else {
        result = QString("Found %1 roots:\n").arg(roots.size());
        for (const SolutionPoint &point : roots) {
            result += point.description + "\n";
        }
    }

    result += QString("\nParameters: a=%1, b=%2\n").arg(a).arg(b);
    return result;
}
