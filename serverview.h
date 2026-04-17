/**
 * @file serverview.h
 * @brief Заголовочный файл класса ServerView
 * @author Developer
 * @date 2026
 *
 * Определяет класс для вывода сообщений на экран (View в MVC паттерне).
 */

#ifndef SERVERVIEW_H
#define SERVERVIEW_H

#include <QObject>

/**
 * @class ServerView
 * @brief Класс для вывода сообщений на экран (представление)
 *
 * Реализует View компонент в архитектуре MVC.
 * Отвечает за отображение всех сообщений и ошибок сервера
 * с добавлением временных меток.
 */
class ServerView : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса ServerView
     * @param parent Родительский QObject (по умолчанию nullptr)
     */
    explicit ServerView(QObject *parent = nullptr);

    /**
     * @brief Вывод обычного сообщения
     * @param message Текст сообщения для вывода
     *
     * Выводит сообщение в консоль с временной меткой в формате [ЧЧ:ММ:СС]
     */
    void displayMessage(const QString &message);

    /**
     * @brief Вывод сообщения об ошибке
     * @param error Текст ошибки для вывода
     *
     * Выводит сообщение об ошибке в консоль с префиксом "ERROR:"
     * и временной меткой
     */
    void displayError(const QString &error);
};

#endif // SERVERVIEW_H
