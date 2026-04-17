/**
 * @file databasemanager.h
 * @brief Заголовочный файл класса DatabaseManager
 * @author Developer
 * @date 2026
 *
 * Определяет класс для управления базой данных SQLite.
 * Статистика пользователей хранится в памяти (QMap).
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QCryptographicHash>
#include <QMap>

class ServerView;

/**
 * @struct UserStats
 * @brief Структура для хранения статистики пользователя в памяти
 */
struct UserStats {
    int totalAttempts;   ///< Общее количество попыток
    int solvedTasks;     ///< Количество решенных задач
    int currentScore;    ///< Текущее количество очков

    UserStats() : totalAttempts(0), solvedTasks(0), currentScore(0) {}
    UserStats(int attempts, int solved, int score)
        : totalAttempts(attempts), solvedTasks(solved), currentScore(score) {}
};

/**
 * @class DatabaseManager
 * @brief Класс для работы с базой данных
 *
 * Реализует Model компонент в архитектуре MVC.
 * Отвечает за:
 * - Подключение к SQLite базе данных
 * - Создание и управление таблицами
 * - CRUD операции с пользователями
 * - Хэширование паролей
 * - Хранение и обновление статистики в памяти
 */
class DatabaseManager
{
public:
    /**
     * @brief Конструктор менеджера БД
     * @param dbPath Путь к файлу базы данных
     * @param view Указатель на представление для логирования
     */
    explicit DatabaseManager(const QString &dbPath, ServerView *view = nullptr);

    /**
     * @brief Деструктор менеджера БД
     *
     * Закрывает соединение с базой данных
     */
    ~DatabaseManager();

    /**
     * @brief Инициализация БД
     * @return true - успешное подключение и создание таблиц
     *
     * Открывает соединение с БД, создает необходимые таблицы
     * и загружает существующих пользователей в память
     */
    bool initialize();

    // Работа с пользователями

    /**
     * @brief Регистрация нового пользователя
     * @param username Имя пользователя (логин)
     * @param password Пароль (будет захэширован)
     * @param email Email пользователя
     * @return true - регистрация успешна, false - ошибка
     */
    bool registerUser(const QString &username, const QString &password, const QString &email);

    /**
     * @brief Аутентификация пользователя
     * @param username Имя пользователя
     * @param password Пароль
     * @return true - пароль верный, false - неверный
     */
    bool authenticateUser(const QString &username, const QString &password);

    /**
     * @brief Проверка существования пользователя
     * @param username Имя пользователя
     * @return true - пользователь существует
     */
    bool userExists(const QString &username);

    // Статистика (хранится в памяти)

    /**
     * @brief Получение статистики пользователя
     * @param login Логин пользователя
     * @param totalAttempts (выходной) Общее количество попыток
     * @param solvedTasks (выходной) Количество решенных задач
     * @param currentScore (выходной) Текущее количество очков
     * @return true - статистика получена
     */
    bool getUserStats(const QString &login, int &totalAttempts, int &solvedTasks, int &currentScore);

    /**
     * @brief Обновление статистики пользователя
     * @param login Логин пользователя
     * @param solved true - задача решена, false - не решена
     *
     * При правильном ответе: +1 попытка, +1 решено, +10 очков
     * При неправильном: только +1 попытка
     */
    void updateUserStats(const QString &login, bool solved);

    // Проверка решения и решение уравнения

    /**
     * @brief Проверка правильности ответа
     * @param taskNumber Номер задачи (1, 2 или 3)
     * @param variant Номер варианта (1 или 2)
     * @param answer Ответ пользователя
     * @param login Логин пользователя
     * @return true - ответ правильный
     */
    bool checkSolution(int taskNumber, int variant, const QString &answer, const QString &login);

    /**
     * @brief Решение уравнения с параметрами a и b
     * @param a Параметр a
     * @param b Параметр b
     * @return Строка с результатами решения
     */
    QString solveEquation(double a, double b);

private:
    /**
     * @brief Создание таблиц в базе данных
     * @return true - таблицы созданы успешно
     */
    bool createTables();

    /**
     * @brief Хэширование пароля
     * @param password Исходный пароль
     * @return Хэш пароля в шестнадцатеричном формате (SHA-256)
     */
    QString hashPassword(const QString &password);

    /**
     * @brief Логирование сообщений
     * @param message Сообщение для лога
     */
    void logMessage(const QString &message);

    /**
     * @brief Загрузка статистики существующих пользователей в память
     */
    void loadStatsToMemory();

    QSqlDatabase m_database;                    ///< Подключение к БД
    QString m_dbPath;                           ///< Путь к файлу БД
    ServerView *m_view;                         ///< Для вывода сообщений
    QMap<QString, UserStats> m_stats;           ///< Статистика пользователей в памяти
};

#endif // DATABASEMANAGER_H
