#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginRequested(const QString& login, const QString& password);
    void registerRequested();

public slots:
    void showError(const QString& error);

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
    QLabel* m_statusLabel;
};

#endif // LOGINWINDOW_H
