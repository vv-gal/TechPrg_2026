/**
 * @file servercontroller.h
 * @brief Заголовочный файл класса ServerController
 * @author Developer
 * @date 2026
 *
 * Определяет контроллер сервера, который обрабатывает клиентские запросы
 * и управляет бизнес-логикой приложения.
 */

#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class DatabaseManager;
class ServerView;

/**
 * @class ServerController
 * @brief Контроллер сервера - обрабатывает запросы клиентов
 *
 * Реализует Controller компонент в архитектуре MVC.
 * Отвечает за:
 * - Прием подключений от клиентов
 * - Обработку входящих запросов
 * - Управление сессиями пользователей
 * - Взаимодействие с DatabaseManager
 */
class ServerController : public QTcpServer
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор контроллера
     * @param view Указатель на объект представления для вывода сообщений
     * @param parent Родительский QObject (по умолчанию nullptr)
     */
    explicit ServerController(ServerView *view, QObject *parent = nullptr);

    /**
     * @brief Деструктор контроллера
     *
     * Освобождает ресурсы и останавливает сервер
     */
    ~ServerController();

    /**
     * @brief Запуск сервера
     * @param port Номер порта для прослушивания (по умолчанию 33333)
     * @return true - сервер успешно запущен, false - ошибка запуска
     */
    bool startServer(quint16 port = 33333);

    /**
     * @brief Остановка сервера
     *
     * Закрывает все соединения и освобождает ресурсы
     */
    void stopServer();

protected:
    /**
     * @brief Обработка нового подключения
     * @param socketDescriptor Дескриптор сокета нового клиента
     *
     * Вызывается автоматически при подключении нового клиента
     */
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    /**
     * @brief Обработка готовности данных от клиента
     *
     * Читает данные из сокета и обрабатывает запросы
     */
    void onReadyRead();

    /**
     * @brief Обработка отключения клиента
     *
     * Освобождает ресурсы, связанные с отключившимся клиентом
     */
    void onDisconnected();

private:
    /**
     * @brief Обработка входящего запроса
     * @param request Строка запроса от клиента
     * @param socket Указатель на сокет клиента
     * @return Строка ответа для клиента
     */
    QString processRequest(const QString &request, QTcpSocket *socket);

    /**
     * @brief Обработка запроса авторизации
     * @param login Логин пользователя
     * @param password Пароль пользователя
     * @param socket Указатель на сокет клиента
     * @return Ответ сервера в формате "auth+&login" или "auth-"
     */
    QString handleAuth(const QString &login, const QString &password, QTcpSocket *socket);

    /**
     * @brief Обработка запроса регистрации
     * @param login Логин пользователя
     * @param password Пароль пользователя
     * @param email Email пользователя
     * @return Ответ сервера в формате "reg+&login" или "reg-"
     */
    QString handleReg(const QString &login, const QString &password, const QString &email);

    /**
     * @brief Обработка запроса статистики
     * @param login Логин пользователя
     * @return Статистика в формате "stat&attempts&solved&score"
     */
    QString handleStat(const QString &login);

    /**
     * @brief Обработка запроса проверки решения
     * @param taskNumber Номер задачи
     * @param variant Номер варианта
     * @param answer Ответ пользователя
     * @param socket Указатель на сокет клиента
     * @return "check+" или "check-"
     */
    QString handleCheck(int taskNumber, int variant, const QString &answer, QTcpSocket *socket);

    /**
     * @brief Обработка запроса решения уравнения
     * @param a Параметр a
     * @param b Параметр b
     * @return Результат решения уравнения
     */
    QString handleSolve(double a, double b);

    ServerView *m_view;                    ///< Для вывода сообщений
    DatabaseManager *m_dbManager;          ///< Для работы с БД
    QMap<QTcpSocket*, QString> m_clients;  ///< Сокет -> логин (сессии)
    QMap<QTcpSocket*, QString> m_buffer;   ///< Буфер для неполных сообщений
};

#endif // SERVERCONTROLLER_H
