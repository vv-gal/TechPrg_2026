#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include "controller/servercontroller.h"
#include "view/serverview.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "========================================";
    qDebug() << "ЗАПУСК СЕРВЕРА";
    qDebug() << "========================================";

    // Проверяем доступные драйверы SQL
    qDebug() << "Доступные SQL драйверы:" << QSqlDatabase::drivers();

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        qDebug() << "ОШИБКА: Драйвер QSQLITE не найден!";
        return 1;
    }
    qDebug() << "Драйвер QSQLITE доступен";

    ServerView view;
    ServerController controller(&view);

    if (controller.startServer(33333)) {
        qDebug() << "========================================";
        qDebug() << "Сервер успешно запущен на порту 33333";
        qDebug() << "Ожидание подключений...";
        qDebug() << "========================================";
        return a.exec();
    } else {
        qDebug() << "ОШИБКА: Не удалось запустить сервер!";
        return 1;
    }
}
