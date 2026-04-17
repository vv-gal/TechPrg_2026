#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>

class ServerConnector;
class MainWindow;
class LoginWindow;

/**
 * @brief Контроллер клиентского приложения
 *
 * Управляет окнами и взаимодействием с сервером
 */
class ClientController : public QObject
{
    Q_OBJECT

public:
    explicit ClientController(QObject *parent = nullptr);
    ~ClientController();

    /**
     * @brief Запуск клиентского приложения
     */
    void start();

private slots:
    /**
     * @brief Обработка успешного входа
     * @param login Логин пользователя
     */
    void onLoginSuccess(const QString& login);

    /**
     * @brief Обработка выхода из системы
     */
    void onLogout();

private:
    /**
     * @brief Показать окно входа
     */
    void showLoginWindow();

    /**
     * @brief Показать окно регистрации
     */
    void showRegisterWindow();

    /**
     * @brief Показать главное окно
     * @param login Логин пользователя
     */
    void showMainWindow(const QString& login);

    ServerConnector* m_server;      ///< Соединение с сервером
    MainWindow* m_mainWindow;       ///< Главное окно
    LoginWindow* m_loginWindow;     ///< Окно входа
    QString m_currentLogin;         ///< Текущий логин пользователя
};

#endif // CLIENTCONTROLLER_H
