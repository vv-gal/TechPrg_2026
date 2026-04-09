#ifndef SERVERVIEW_H
#define SERVERVIEW_H

#include <QObject>

class ServerView : public QObject
{
    Q_OBJECT

public:
    explicit ServerView(QObject *parent = nullptr);

    void displayMessage(const QString &message);
    void displayError(const QString &error);
};

#endif
