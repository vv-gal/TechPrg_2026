/**
 * @file main.cpp
 * @brief Главный файл серверного приложения
 * @author Developer
 * @date 2026
 *
 * Точка входа в программу сервера. Инициализирует MVC компоненты
 * и запускает TCP сервер на указанном порту.
 */

#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include "controller/servercontroller.h"
#include "view/serverview.h"

/**
 * @brief Главная функция - точка входа в программу
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код возврата (0 - успех, 1 - ошибка)
 *
 * Выполняет следующие действия:
 * 1. Проверяет наличие SQLite драйвера
 * 2. Создает объекты MVC (View и Controller)
 * 3. Запускает сервер на порту 33333
 * 4. Запускает цикл обработки событий Qt
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "========================================";
    qDebug() << "ЗАПУСК СЕРВЕРА";
    qDebug() << "========================================";

    // Проверяем, есть ли драйвер SQLite
    qDebug() << "Доступные SQL драйверы:" << QSqlDatabase::drivers();

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        qDebug() << "ОШИБКА: Драйвер QSQLITE не найден!";
        return 1;
    }
    qDebug() << "Драйвер QSQLITE доступен";

    // Создаём объекты MVC
    ServerView view;                    ///< Представление (вывод сообщений)
    ServerController controller(&view); ///< Контроллер (вся логика)

    // Запускаем сервер на порту 33333
    if (controller.startServer(33333)) {
        qDebug() << "========================================";
        qDebug() << "Сервер успешно запущен на порту 33333";
        qDebug() << "Ожидание подключений...";
        qDebug() << "========================================";
        return a.exec();  ///< Запускаем бесконечный цикл обработки событий
    } else {
        qDebug() << "ОШИБКА: Не удалось запустить сервер!";
        return 1;
    }
}
