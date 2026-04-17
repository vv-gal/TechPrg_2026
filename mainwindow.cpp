#include "view/mainwindow.h"
#include "view/graphpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_homePage(nullptr), m_graphPage(nullptr), m_currentPage(0)
{
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Math Equation Solver");
    setMinimumSize(1000, 700);

    // Устанавливаем стиль
    setStyleSheet("background-color: #1e1e2e;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Верхняя панель
    QWidget* topBar = new QWidget();
    topBar->setStyleSheet("background-color: #2a2a3a; border-radius: 12px;");
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);

    m_welcomeLabel = new QLabel("Welcome!");
    m_welcomeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffffff;");

    m_statsLabel = new QLabel("📊 Stats: 0 attempts, 0 solved, 0 points");
    m_statsLabel->setStyleSheet("font-size: 13px; color: #aaaaaa; background-color: #1e1e2e; padding: 5px 10px; border-radius: 10px;");

    m_logoutButton = new QPushButton("🚪 Выйти");
    m_logoutButton->setStyleSheet("background-color: #f44336; color: white; padding: 8px 16px; border-radius: 8px; font-weight: bold;");

    topLayout->addWidget(m_welcomeLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_statsLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_logoutButton);

    // Stacked widget для страниц
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setStyleSheet("background-color: #1e1e2e; border-radius: 12px;");

    // Создаем страницы
    setupHomePage();
    setupGraphPage();

    if (m_homePage) {
        m_stackedWidget->addWidget(m_homePage);
    }
    if (m_graphPage) {
        m_stackedWidget->addWidget(m_graphPage);
    }

    // Кнопка навигации
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    m_navButton = new QPushButton("📊 Перейти к графику");
    m_navButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2196F3, stop:1 #1976D2);"
                               "color: white; padding: 12px; font-size: 14px; font-weight: bold;"
                               "border-radius: 10px;");
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_navButton);
    bottomLayout->addStretch();

    // Добавляем все в главный layout
    mainLayout->addWidget(topBar);
    mainLayout->addWidget(m_stackedWidget, 1);
    mainLayout->addLayout(bottomLayout);

    // Подключаем сигналы
    connect(m_logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(m_navButton, &QPushButton::clicked, this, &MainWindow::onNavigateClicked);

    updateStats(0, 0, 0);
}

void MainWindow::setupHomePage()
{
    m_homePage = new QWidget();
    m_homePage->setStyleSheet("background-color: #1e1e2e;");

    QVBoxLayout* layout = new QVBoxLayout(m_homePage);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);

    // Заголовок
    QLabel* titleLabel = new QLabel("📐 MATH EQUATION SOLVER");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #ffffff; background-color: #2a2a3a; padding: 20px; border-radius: 15px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    // Информационная панель
    QGroupBox* infoBox = new QGroupBox("ℹ️ О ПРОГРАММЕ");
    infoBox->setStyleSheet("QGroupBox { color: #64b5f6; font-weight: bold; font-size: 14px; border: 1px solid #444; border-radius: 10px; margin-top: 10px; }"
                           "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");

    QLabel* infoLabel = new QLabel(
        "Данное приложение решает кусочно-заданные уравнения:\n\n"
        "🔴 f₁(x) = ln(x + b)      для x ∈ (-b, 2)\n"
        "🔵 f₂(x) = x³ - 3x + a    для всех x\n"
        "🟢 f₃(x) = 1/(x-2) - b    для x ∈ (2, ∞)\n\n"
        "Используйте страницу «График» для визуализации всех трёх функций."
        );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #cccccc; font-size: 13px; padding: 10px;");

    QVBoxLayout* infoLayout = new QVBoxLayout(infoBox);
    infoLayout->addWidget(infoLabel);

    // Панель проверки ответов
    QGroupBox* checkBox = new QGroupBox("✅ ПРОВЕРКА ОТВЕТА");
    checkBox->setStyleSheet("QGroupBox { color: #81c784; font-weight: bold; font-size: 14px; border: 1px solid #444; border-radius: 10px; margin-top: 10px; }"
                            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    m_taskCombo = new QComboBox();
    m_taskCombo->addItems({"Задача 1: ln(x+b) = 0", "Задача 2: x³ - 3x + a = 0", "Задача 3: 1/(x-2) - b = 0"});
    m_taskCombo->setStyleSheet("background-color: #2a2a3a; color: white; padding: 8px; border-radius: 8px;");

    m_variantCombo = new QComboBox();
    m_variantCombo->addItems({"Вариант 1", "Вариант 2"});
    m_variantCombo->setStyleSheet("background-color: #2a2a3a; color: white; padding: 8px; border-radius: 8px;");

    m_answerEdit = new QLineEdit();
    m_answerEdit->setPlaceholderText("Введите ваш ответ...");
    m_answerEdit->setStyleSheet("background-color: #2a2a3a; color: white; padding: 10px; border-radius: 8px; border: 1px solid #444;");

    m_checkButton = new QPushButton("🔍 ПРОВЕРИТЬ ОТВЕТ");
    m_checkButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4CAF50, stop:1 #388E3C);"
                                 "color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 10px;");

    m_resultLabel = new QLabel();
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setStyleSheet("font-size: 14px; padding: 10px;");

    formLayout->addRow("Задача:", m_taskCombo);
    formLayout->addRow("Вариант:", m_variantCombo);
    formLayout->addRow("Ответ:", m_answerEdit);
    formLayout->addRow("", m_checkButton);

    checkBox->setLayout(formLayout);

    // Добавляем все в layout
    layout->addWidget(titleLabel);
    layout->addWidget(infoBox);
    layout->addWidget(checkBox);
    layout->addStretch();
    layout->addWidget(m_resultLabel);

    connect(m_checkButton, &QPushButton::clicked, this, &MainWindow::onCheckClicked);
}

void MainWindow::setupGraphPage()
{
    m_graphPage = new GraphPage();

    connect(m_graphPage, &GraphPage::solveRequested,
            this, &MainWindow::solveEquationRequested);
    connect(m_graphPage, &GraphPage::backToMain,
            this, &MainWindow::onBackToMain);
}

void MainWindow::onNavigateClicked()
{
    if (m_stackedWidget->currentIndex() == 0) {
        // Переход к графику
        m_stackedWidget->setCurrentIndex(1);
        m_navButton->setText("🏠 Вернуться на главную");
        m_navButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #9C27B0, stop:1 #7B1FA2);"
                                   "color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 10px;");
    } else {
        // Возврат на главную
        onBackToMain();
    }
}

void MainWindow::onBackToMain()
{
    m_stackedWidget->setCurrentIndex(0);
    m_navButton->setText("📊 Перейти к графику");
    m_navButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2196F3, stop:1 #1976D2);"
                               "color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 10px;");
}

void MainWindow::setUserData(const QString& login, const QString& email)
{
    m_login = login;
    m_email = email;
    m_welcomeLabel->setText("👋 Привет, " + login + "!");

    if (m_graphPage) {
        m_graphPage->setLogin(login, email);
    }
}

void MainWindow::onCheckClicked()
{
    int task = m_taskCombo->currentIndex() + 1;
    int variant = m_variantCombo->currentIndex() + 1;
    QString answer = m_answerEdit->text();

    if (answer.isEmpty()) {
        showMessage("❌ Пожалуйста, введите ответ!", true);
        return;
    }

    emit checkRequested(task, variant, answer);
}

void MainWindow::updateStats(int attempts, int solved, int score)
{
    m_statsLabel->setText(QString("📊 Статистика: %1 попыток, %2 решено, %3 очков")
                              .arg(attempts).arg(solved).arg(score));
}

void MainWindow::showMessage(const QString& msg, bool isError)
{
    m_resultLabel->setText(msg);
    m_resultLabel->setStyleSheet(isError ? "color: #ff6b6b; font-size: 14px; padding: 10px;"
                                         : "color: #81c784; font-size: 14px; padding: 10px;");
}

void MainWindow::onLogoutClicked()
{
    emit logoutRequested();
}
