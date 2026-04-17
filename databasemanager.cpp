/**
 * @file databasemanager.cpp
 * @brief Реализация класса DatabaseManager
 * @author Developer
 * @date 2026
 *
 * Содержит реализацию методов для работы с SQLite базой данных.
 * Статистика пользователей хранится в памяти (QMap).
 */

#include "model/databasemanager.h"
#include "model/functionSolver.h"
#include "view/serverview.h"
#include <QDebug>
#include <QSqlError>
#include <QFile>

/**
 * @brief Конструктор менеджера базы данных
 * @param dbPath Путь к файлу базы данных
 * @param view Указатель на объект представления для логирования
 */
DatabaseManager::DatabaseManager(const QString &dbPath, ServerView *view)
    : m_dbPath(dbPath), m_view(view)
{
    logMessage("[DB] Constructor called with path: " + m_dbPath);
}

/**
 * @brief Деструктор менеджера базы данных
 */
DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
        logMessage("[DB] Database closed");
    }
}

/**
 * @brief Вывод сообщения в лог
 * @param message Сообщение для вывода
 */
void DatabaseManager::logMessage(const QString &message)
{
    qDebug().noquote() << message;
    if (m_view) {
        m_view->displayMessage(message);
    }
}

/**
 * @brief Подключение к БД и создание таблиц
 * @return true - успешное подключение и инициализация
 */
bool DatabaseManager::initialize()
{
    logMessage("[DB] ========================================");
    logMessage("[DB] INITIALIZING DATABASE");

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    if (!m_database.open()) {
        logMessage("[DB] ERROR: " + m_database.lastError().text());
        return false;
    }

    logMessage("[DB] SUCCESS: Database opened!");

    QFile dbFile(m_dbPath);
    if (dbFile.exists()) {
        logMessage("[DB] Database file size: " + QString::number(dbFile.size()) + " bytes");
    }

    if (!createTables()) {
        return false;
    }

    // Загружаем статистику существующих пользователей в память
    loadStatsToMemory();

    return true;
}

/**
 * @brief Создание таблиц в базе данных
 * @return true - таблицы созданы успешно
 *
 * Создает таблицу users только с полями:
 * - id, username, email, password_hash
 * (без полей статистики)
 */
bool DatabaseManager::createTables()
{
    logMessage("[DB] Creating tables if not exist...");

    QSqlQuery query;

    // Таблица пользователей (только ID, имя, почта, хэш пароля)
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

    if (query.exec("SELECT COUNT(*) FROM users") && query.next()) {
        int userCount = query.value(0).toInt();
        logMessage("[DB] Users in database: " + QString::number(userCount));
    }

    logMessage("[DB] ========================================");
    return true;
}

/**
 * @brief Загрузка статистики существующих пользователей в память
 *
 * При запуске сервера загружает всех пользователей из БД
 * и инициализирует для них нулевую статистику в памяти
 */
void DatabaseManager::loadStatsToMemory()
{
    logMessage("[DB] Loading user stats to memory...");

    QSqlQuery query;
    query.exec("SELECT username FROM users");

    int loadedCount = 0;
    while (query.next()) {
        QString username = query.value(0).toString();
        m_stats[username] = UserStats(0, 0, 0);
        loadedCount++;
        logMessage("[DB] Loaded user: " + username + " with zero stats");
    }

    logMessage("[DB] Loaded " + QString::number(loadedCount) + " users to memory");
}

/**
 * @brief Хэширование пароля
 * @param password Исходный пароль
 * @return Хэш пароля в шестнадцатеричном формате
 */
QString DatabaseManager::hashPassword(const QString &password)
{
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

/**
 * @brief Регистрация нового пользователя
 * @param username Имя пользователя (логин)
 * @param password Пароль (будет захэширован)
 * @param email Email пользователя
 * @return true - регистрация успешна, false - ошибка
 */
bool DatabaseManager::registerUser(const QString &username, const QString &password, const QString &email)
{
    logMessage("[DB] Registering user: " + username);

    if (userExists(username)) {
        logMessage("[DB] ERROR: User already exists: " + username);
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, email, password_hash) "
                  "VALUES (:username, :email, :password_hash)");
    query.bindValue(":username", username);
    query.bindValue(":email", email);
    query.bindValue(":password_hash", hashPassword(password));

    if (query.exec()) {
        // Инициализируем статистику в памяти для нового пользователя
        m_stats[username] = UserStats(0, 0, 0);
        logMessage("[DB] SUCCESS: User registered: " + username);
        logMessage("[DB] Stats initialized in memory for: " + username);
        return true;
    } else {
        logMessage("[DB] ERROR: " + query.lastError().text());
        return false;
    }
}

/**
 * @brief Аутентификация пользователя
 * @param username Имя пользователя
 * @param password Пароль
 * @return true - пароль верный, пользователь существует
 */
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

/**
 * @brief Проверка существования пользователя
 * @param username Имя пользователя
 * @return true - пользователь существует в базе данных
 */
bool DatabaseManager::userExists(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username");
    query.bindValue(":username", username);
    return query.exec() && query.next();
}

/**
 * @brief Получение статистики пользователя
 * @param login Логин пользователя
 * @param totalAttempts (выходной) Общее количество попыток
 * @param solvedTasks (выходной) Количество решенных задач
 * @param currentScore (выходной) Текущее количество очков
 * @return true - статистика успешно получена
 *
 * Статистика берется из памяти (QMap), а не из БД
 */
bool DatabaseManager::getUserStats(const QString &login, int &totalAttempts, int &solvedTasks, int &currentScore)
{
    logMessage("[DB] Getting stats for user: " + login);

    if (m_stats.contains(login)) {
        totalAttempts = m_stats[login].totalAttempts;
        solvedTasks = m_stats[login].solvedTasks;
        currentScore = m_stats[login].currentScore;
        logMessage("[DB] Stats from memory: attempts=" + QString::number(totalAttempts) +
                   ", solved=" + QString::number(solvedTasks) +
                   ", score=" + QString::number(currentScore));
        return true;
    }

    // Если пользователь существует в БД, но почему-то не в памяти
    if (userExists(login)) {
        m_stats[login] = UserStats(0, 0, 0);
        totalAttempts = 0;
        solvedTasks = 0;
        currentScore = 0;
        logMessage("[DB] User found in DB, created stats in memory");
        return true;
    }

    totalAttempts = 0;
    solvedTasks = 0;
    currentScore = 0;
    logMessage("[DB] User not found: " + login);
    return false;
}

/**
 * @brief Обновление статистики пользователя
 * @param login Логин пользователя
 * @param solved true - задача решена, false - не решена
 *
 * Правила начисления очков:
 * - Правильный ответ: +1 попытка, +1 решено, +10 очков
 * - Неправильный ответ: только +1 попытка
 *
 * Статистика обновляется только в памяти, БД не изменяется
 */
void DatabaseManager::updateUserStats(const QString &login, bool solved)
{
    logMessage("[DB] Update stats for " + login + " solved: " + (solved ? "yes" : "no"));

    if (!m_stats.contains(login)) {
        if (userExists(login)) {
            m_stats[login] = UserStats(0, 0, 0);
        } else {
            logMessage("[DB] ERROR: User not found for stats update: " + login);
            return;
        }
    }

    UserStats& stats = m_stats[login];

    if (solved) {
        // Правильный ответ: +1 попытка, +1 решено, +10 очков
        stats.totalAttempts++;
        stats.solvedTasks++;
        stats.currentScore += 10;
    } else {
        // Неправильный ответ: только +1 попытка
        stats.totalAttempts++;
    }

    logMessage("[DB] Stats updated in memory: attempts=" + QString::number(stats.totalAttempts) +
               ", solved=" + QString::number(stats.solvedTasks) +
               ", score=" + QString::number(stats.currentScore));
}

/**
 * @brief Проверка правильности ответа
 * @param taskNumber Номер задачи (1, 2 или 3)
 * @param variant Номер варианта (1 или 2)
 * @param answer Ответ пользователя
 * @param login Логин пользователя (для логирования)
 * @return true - ответ правильный
 */
bool DatabaseManager::checkSolution(int taskNumber, int variant, const QString &answer, const QString &login)
{
    // Таблица правильных ответов
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

/**
 * @brief Решение уравнения с параметрами a и b
 * @param a Параметр a
 * @param b Параметр b
 * @return Строка с результатами решения
 */
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
