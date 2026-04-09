#ifndef AUTHVIEW_H
#define AUTHVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class AuthView : public QWidget
{
    Q_OBJECT

public:
    explicit AuthView(QWidget *parent = nullptr);
    void showError(const QString &message);

signals:
    void loginRequested(const QString &email, const QString &password);
    void registerRequested();

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    void setupUI();

    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_registerButton;
    QLabel *m_errorLabel;
};

#endif
