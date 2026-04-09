#include "registrationview.h"

RegistrationView::RegistrationView(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void RegistrationView::setupUI()
{
    setFixedSize(450, 550);
    setWindowTitle("Регистрация");

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
        QPushButton#backButton {
            background: #f44336;
        }
        QPushButton#backButton:hover {
            background: #da190b;
        }
        QLabel#errorLabel {
            color: #ff6b6b;
            font-size: 12px;
        }
    )");

    QLabel *titleLabel = new QLabel("📝 Регистрация", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("👤 Имя пользователя");

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("📧 Email");

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("🔒 Пароль (мин. 6 символов)");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_confirmPasswordEdit = new QLineEdit(this);
    m_confirmPasswordEdit->setPlaceholderText("🔒 Подтвердите пароль");
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    m_registerButton = new QPushButton("Зарегистрироваться", this);
    m_registerButton->setCursor(Qt::PointingHandCursor);

    m_backButton = new QPushButton("← Назад ко входу", this);
    m_backButton->setObjectName("backButton");
    m_backButton->setCursor(Qt::PointingHandCursor);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(m_usernameEdit);
    layout->addWidget(m_emailEdit);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(m_confirmPasswordEdit);
    layout->addWidget(m_errorLabel);
    layout->addWidget(m_registerButton);
    layout->addWidget(m_backButton);
    layout->addStretch();

    setLayout(layout);

    connect(m_registerButton, &QPushButton::clicked, this, &RegistrationView::onRegisterClicked);
    connect(m_backButton, &QPushButton::clicked, this, &RegistrationView::onBackClicked);
}

void RegistrationView::onRegisterClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirmPassword = m_confirmPasswordEdit->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        showError("Заполните все поля!");
        return;
    }

    if (!email.contains('@')) {
        showError("Введите корректный email!");
        return;
    }

    if (password.length() < 6) {
        showError("Пароль должен содержать минимум 6 символов!");
        return;
    }

    if (password != confirmPassword) {
        showError("Пароли не совпадают!");
        return;
    }

    m_errorLabel->setVisible(false);
    emit registrationRequested(username, email, password, confirmPassword);
}

void RegistrationView::onBackClicked()
{
    clearFields();
    emit backToLoginRequested();
}

void RegistrationView::showError(const QString &message)
{
    m_errorLabel->setText("⚠️ " + message);
    m_errorLabel->setVisible(true);
}

void RegistrationView::clearFields()
{
    m_usernameEdit->clear();
    m_emailEdit->clear();
    m_passwordEdit->clear();
    m_confirmPasswordEdit->clear();
    m_errorLabel->setVisible(false);
}
