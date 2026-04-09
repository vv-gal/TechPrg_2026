#include <QApplication>
#include <QStackedWidget>
#include <QMessageBox>
#include <QInputDialog>
#include "authview.h"
#include "registrationview.h"
#include "mainview.h"
#include "servercontroller.h"

class ApplicationController : public QObject
{
    Q_OBJECT
public:
    ApplicationController(QObject *parent = nullptr) : QObject(parent)
    {
        m_stackedWidget = new QStackedWidget();

        // Создаём только окна авторизации и регистрации
        m_authView = new AuthView();
        m_registrationView = new RegistrationView();
        m_mainView = nullptr;

        m_stackedWidget->addWidget(m_authView);
        m_stackedWidget->addWidget(m_registrationView);

        // Подключаем сигналы от форм
        connect(m_authView, &AuthView::loginRequested,
                this, &ApplicationController::onLoginRequested);
        connect(m_authView, &AuthView::registerRequested,
                this, &ApplicationController::showRegistration);

        connect(m_registrationView, &RegistrationView::registrationRequested,
                this, &ApplicationController::onRegistrationRequested);
        connect(m_registrationView, &RegistrationView::backToLoginRequested,
                this, &ApplicationController::showAuth);

        // Подключаем сигналы от сервера
        auto &server = ServerController::instance();
        connect(&server, &ServerController::loginCompleted,
                this, &ApplicationController::onLoginCompleted);
        connect(&server, &ServerController::registrationCompleted,
                this, &ApplicationController::onRegistrationCompleted);

        m_stackedWidget->setWindowTitle("Math Client");
        m_stackedWidget->resize(450, 400);
        m_stackedWidget->show();
    }

private slots:
    void onLoginRequested(const QString &email, const QString &password)
    {
        ServerController::instance().loginUser(email, password);
    }

    void onLoginCompleted(bool success, const QString &message,
                          const ServerController::UserData &user)
    {
        if (success) {
            // Создаём главное окно только при успешном входе
            if (!m_mainView) {
                m_mainView = new MainView();
                m_stackedWidget->addWidget(m_mainView);
                connect(m_mainView, &MainView::logoutRequested,
                        this, &ApplicationController::logout);
            }
            m_mainView->setUserData(user);
            m_stackedWidget->setCurrentIndex(2);
            m_stackedWidget->resize(1200, 700);
        } else {
            m_authView->showError(message);
        }
    }

    void onRegistrationRequested(const QString &username, const QString &email,
                                 const QString &password, const QString &confirmPassword)
    {
        if (password != confirmPassword) {
            m_registrationView->showError("Пароли не совпадают!");
            return;
        }
        if (password.length() < 6) {
            m_registrationView->showError("Пароль должен быть не менее 6 символов!");
            return;
        }
        ServerController::instance().registerUser(username, email, password);
    }

    void onRegistrationCompleted(bool success, const QString &message,
                                 const QString &verificationCode)
    {
        if (success) {
            bool ok;
            QString code = QInputDialog::getText(m_stackedWidget,
                                                 "Подтверждение email",
                                                 QString("Введите код подтверждения:\n(Тестовый код: %1)\n\n%2")
                                                     .arg(verificationCode).arg(message),
                                                 QLineEdit::Normal, "", &ok);

            if (ok && code == verificationCode) {
                QMessageBox::information(m_stackedWidget, "Успех",
                                         "Email подтвержден! Теперь вы можете войти.");
                showAuth();
            } else {
                QMessageBox::warning(m_stackedWidget, "Ошибка", "Неверный код!");
            }
        } else {
            m_registrationView->showError(message);
        }
    }

    void showAuth()
    {
        m_stackedWidget->setCurrentIndex(0);
        m_stackedWidget->resize(450, 400);
    }

    void showRegistration()
    {
        m_stackedWidget->setCurrentIndex(1);
        m_stackedWidget->resize(500, 550);
    }

    void logout()
    {
        showAuth();
    }

private:
    QStackedWidget *m_stackedWidget;
    AuthView *m_authView;
    RegistrationView *m_registrationView;
    MainView *m_mainView;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Math Client");

    ApplicationController controller;

    return app.exec();
}

#include "main.moc"
