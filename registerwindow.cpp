#include "view/registerwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Register");
    setMinimumSize(300, 250);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_loginEdit = new QLineEdit();
    m_loginEdit->setPlaceholderText("Login");

    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText("Email");

    m_registerButton = new QPushButton("Register");
    m_backButton = new QPushButton("Back to Login");

    m_statusLabel = new QLabel();
    m_statusLabel->setStyleSheet("color: red;");

    layout->addWidget(m_loginEdit);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(m_emailEdit);
    layout->addWidget(m_registerButton);
    layout->addWidget(m_backButton);
    layout->addWidget(m_statusLabel);

    connect(m_registerButton, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(m_backButton, &QPushButton::clicked, this, &RegisterWindow::onBackClicked);
}

void RegisterWindow::onRegisterClicked()
{
    QString login = m_loginEdit->text();
    QString pass = m_passwordEdit->text();
    QString email = m_emailEdit->text();

    if (login.isEmpty() || pass.isEmpty() || email.isEmpty()) {
        m_statusLabel->setText("Please fill all fields!");
        return;
    }

    emit registerRequested(login, pass, email);
}

void RegisterWindow::onBackClicked()
{
    emit backToLogin();
    close();
}

void RegisterWindow::showError(const QString& error)
{
    m_statusLabel->setText(error);
}
