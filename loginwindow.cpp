#include "view/loginwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Login");
    setMinimumSize(300, 200);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_loginEdit = new QLineEdit();
    m_loginEdit->setPlaceholderText("Login");

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_loginButton = new QPushButton("Login");
    m_registerButton = new QPushButton("Register");

    m_statusLabel = new QLabel();
    m_statusLabel->setStyleSheet("color: red;");

    layout->addWidget(m_loginEdit);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(m_loginButton);
    layout->addWidget(m_registerButton);
    layout->addWidget(m_statusLabel);

    connect(m_loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
}

void LoginWindow::onLoginClicked()
{
    QString login = m_loginEdit->text();
    QString pass = m_passwordEdit->text();

    if (login.isEmpty() || pass.isEmpty()) {
        m_statusLabel->setText("Please fill all fields!");
        return;
    }

    emit loginRequested(login, pass);
}

void LoginWindow::onRegisterClicked()
{
    emit registerRequested();
}

void LoginWindow::showError(const QString& error)
{
    m_statusLabel->setText(error);
}
