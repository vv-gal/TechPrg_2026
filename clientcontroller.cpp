#include "controller/clientcontroller.h"
#include "network/serverconnector.h"
#include "view/mainwindow.h"
#include "view/loginwindow.h"
#include "view/registerwindow.h"
#include <QMessageBox>
#include <QDebug>

ClientController::ClientController(QObject *parent)
    : QObject(parent), m_mainWindow(nullptr), m_loginWindow(nullptr), m_currentLogin("")
{
    m_server = ServerConnector::getInstance();
    qDebug() << "ClientController created";
}

ClientController::~ClientController()
{
    qDebug() << "ClientController destroyed";
    if (m_server) {
        m_server->disconnectFromServer();
    }
}

void ClientController::start()
{
    qDebug() << "Starting client controller...";

    // Подключаемся к серверу
    if (!m_server->connectToServer("localhost", 33333)) {
        QMessageBox::critical(nullptr, "Error",
                              "Cannot connect to server!\n"
                              "Make sure the server is running on port 33333.");
        qDebug() << "Failed to connect to server";
        return;
    }

    qDebug() << "Connected to server successfully";

    // Показываем окно входа
    showLoginWindow();
}

void ClientController::showLoginWindow()
{
    qDebug() << "Showing login window";

    m_loginWindow = new LoginWindow();

    connect(m_loginWindow, &LoginWindow::loginRequested,
            [this](const QString& login, const QString& pass) {
                qDebug() << "Login requested for:" << login;
                m_server->auth(login, pass, [this, login](bool success, const QString& result) {
                    if (success) {
                        qDebug() << "Login successful for:" << login;
                        m_currentLogin = login;
                        m_loginWindow->close();
                        onLoginSuccess(login);
                    } else {
                        qDebug() << "Login failed for:" << login;
                        m_loginWindow->showError("Login failed! Invalid username or password.");
                    }
                });
            });

    connect(m_loginWindow, &LoginWindow::registerRequested,
            [this]() {
                qDebug() << "Register requested";
                showRegisterWindow();
            });

    m_loginWindow->show();
}

void ClientController::showRegisterWindow()
{
    qDebug() << "Showing register window";

    RegisterWindow* regWindow = new RegisterWindow();

    connect(regWindow, &RegisterWindow::registerRequested,
            [this, regWindow](const QString& login, const QString& pass, const QString& email) {
                qDebug() << "Register requested for:" << login;
                m_server->reg(login, pass, email, [this, regWindow, login](bool success, const QString& result) {
                    if (success) {
                        qDebug() << "Registration successful for:" << login;
                        regWindow->close();
                        QMessageBox::information(nullptr, "Success",
                                                 "Registration successful!\n"
                                                 "Please login with your credentials.");
                        showLoginWindow();
                    } else {
                        qDebug() << "Registration failed for:" << login;
                        regWindow->showError("Registration failed! Username may already exist.");
                    }
                });
            });

    connect(regWindow, &RegisterWindow::backToLogin, [regWindow]() {
        regWindow->close();
    });

    regWindow->show();
}

void ClientController::onLoginSuccess(const QString& login)
{
    qDebug() << "Login success, showing main window for:" << login;
    m_currentLogin = login;
    showMainWindow(login);

    // Загружаем статистику
    m_server->getStats(login, [this](int attempts, int solved, int score) {
        qDebug() << "Stats received:" << attempts << solved << score;
        if (m_mainWindow) {
            m_mainWindow->updateStats(attempts, solved, score);
        }
    });
}

void ClientController::showMainWindow(const QString& login)
{
    qDebug() << "Creating main window for:" << login;

    if (m_mainWindow) {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }

    m_mainWindow = new MainWindow();
    m_mainWindow->setWindowTitle("Math Equation Solver - " + login);
    m_mainWindow->setUserData(login, login + "@mail.com");

    // Обработка проверки ответа
    connect(m_mainWindow, &MainWindow::checkRequested,
            [this](int task, int variant, const QString& answer) {
                qDebug() << "Check requested: task=" << task << "variant=" << variant << "answer=" << answer;
                m_server->checkSolution(task, variant, answer, [this](bool correct) {
                    if (correct) {
                        m_mainWindow->showMessage("✅ Correct answer! +10 points");
                        qDebug() << "Answer is correct";
                    } else {
                        m_mainWindow->showMessage("❌ Wrong answer! Try again.", true);
                        qDebug() << "Answer is wrong";
                    }

                    // Обновляем статистику
                    m_server->getStats(m_currentLogin, [this](int attempts, int solved, int score) {
                        qDebug() << "Updated stats:" << attempts << solved << score;
                        if (m_mainWindow) {
                            m_mainWindow->updateStats(attempts, solved, score);
                        }
                    });
                });
            });

    // Обработка решения уравнения
    connect(m_mainWindow, &MainWindow::solveEquationRequested,
            [this](double a, double b) {
                qDebug() << "Solve equation requested: a=" << a << "b=" << b;
                m_server->solveEquation(a, b, [this](const QString& result) {
                    qDebug() << "Equation solution received";
                    QMessageBox::information(m_mainWindow, "Equation Solution", result);
                });
            });

    // Обработка выхода
    connect(m_mainWindow, &MainWindow::logoutRequested, this, &ClientController::onLogout);

    m_mainWindow->show();
    qDebug() << "Main window shown";
}

void ClientController::onLogout()
{
    qDebug() << "Logout requested for:" << m_currentLogin;

    if (m_mainWindow) {
        m_mainWindow->close();
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }

    m_currentLogin = "";
    showLoginWindow();
}
