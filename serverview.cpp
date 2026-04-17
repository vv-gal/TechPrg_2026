/**
 * @file serverview.cpp
 * @brief Реализация класса ServerView
 * @author Developer
 * @date 2026
 */

#include "view/serverview.h"
#include <QDateTime>
#include <QDebug>

/**
 * @brief Конструктор класса ServerView
 * @param parent Родительский QObject (по умолчанию nullptr)
 */
ServerView::ServerView(QObject *parent) : QObject(parent) {}

/**
 * @brief Вывод сообщения с временной меткой
 * @param message Текст сообщения для вывода
 *
 * Формат вывода: [ЧЧ:ММ:СС] сообщение
 */
void ServerView::displayMessage(const QString &message)
{
    // Получаем текущее время в формате ЧЧ:ММ:СС
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    // Выводим в консоль: [время] сообщение
    qDebug().noquote() << "[" + timestamp + "] " + message;
}

/**
 * @brief Вывод ошибки с временной меткой
 * @param error Текст ошибки для вывода
 *
 * Формат вывода: [ЧЧ:ММ:СС] ERROR: сообщение
 */
void ServerView::displayError(const QString &error)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug().noquote() << "[" + timestamp + "] ERROR: " + error;
}
