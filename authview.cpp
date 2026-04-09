#include "authview.h"
#include <QMessageBox>

AuthView::AuthView(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void AuthView::setupUI()
{
    setFixedSize(400, 380);
    setWindowTitle("Авторизация");

    setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #667eea, stop:1 #764ba2);
        }
        QLabel#titleLabel {
            color: white;
            font-size: 28px;
            font-weight: bold;
        }
        QLineEdit {
            padding: 12px;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 14px;
            background: white;
        }
        QPushButton {
            background: #4CAF50;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #45a049;
        }
        QPushButton#registerButton {
            background: transparent;
            color: #ffd700;
            font-size: 14px;
        }
        QPushButton#registerButton:hover {
            color: #ffed4a;
        }
        QLabel#errorLabel {
            color: #ff6b6b;
            font-size: 12px;
        }
    )");

    QLabel *titleLabel = new QLabel("🔐 Вход в систему", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("📧 Email");

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("🔒 Пароль");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_loginButton = new QPushButton("Войти", this);
    m_loginButton->setCursor(Qt::PointingHandCursor);

    m_registerButton = new QPushButton("Нет аккаунта? Зарегистрироваться", this);
    m_registerButton->setObjectName("registerButton");
    m_registerButton->setCursor(Qt::PointingHandCursor);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(m_emailEdit);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(m_errorLabel);
    layout->addWidget(m_loginButton);
    layout->addWidget(m_registerButton);
    layout->addStretch();

    setLayout(layout);

    connect(m_loginButton, &QPushButton::clicked, this, &AuthView::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &AuthView::onRegisterClicked);
}

void AuthView::onLoginClicked()
{
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        showError("Заполните все поля!");
        return;
    }

    if (!email.contains('@')) {
        showError("Введите корректный email!");
        return;
    }

    m_errorLabel->setVisible(false);
    emit loginRequested(email, password);
}

void AuthView::onRegisterClicked()
{
    emit registerRequested();
}

void AuthView::showError(const QString &message)
{
    m_errorLabel->setText("⚠️ " + message);
    m_errorLabel->setVisible(true);
}
