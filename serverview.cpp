#include "view/serverview.h"
#include <QDateTime>
#include <QDebug>

ServerView::ServerView(QObject *parent) : QObject(parent) {}

void ServerView::displayMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug().noquote() << "[" + timestamp + "] " + message;
}

void ServerView::displayError(const QString &error)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug().noquote() << "[" + timestamp + "] ERROR: " + error;
}
