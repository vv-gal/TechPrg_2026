#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class RegisterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);

signals:
    void registerRequested(const QString& login, const QString& password, const QString& email);
    void backToLogin();

public slots:
    void showError(const QString& error);

private slots:
    void onRegisterClicked();
    void onBackClicked();

private:
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_emailEdit;
    QPushButton* m_registerButton;
    QPushButton* m_backButton;
    QLabel* m_statusLabel;
};

#endif // REGISTERWINDOW_H
