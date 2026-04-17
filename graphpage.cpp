#include "view/graphpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QMessageBox>
#include <QPalette>
#include <QLinearGradient>
#include <cmath>

GraphPage::GraphPage(QWidget *parent)
    : QWidget(parent), m_currentA(1.0), m_currentB(1.0), m_customPlot(nullptr)
{
    setupUI();
}

GraphPage::~GraphPage()
{
}

void GraphPage::setupStyledSlider(QSlider* slider, const QString& color)
{
    slider->setStyleSheet(QString(
                              "QSlider::groove:horizontal {"
                              "    height: 8px;"
                              "    background: #e0e0e0;"
                              "    border-radius: 4px;"
                              "}"
                              "QSlider::handle:horizontal {"
                              "    background: %1;"
                              "    width: 18px;"
                              "    height: 18px;"
                              "    margin: -5px 0;"
                              "    border-radius: 9px;"
                              "    border: 2px solid white;"
                              "}"
                              "QSlider::sub-page:horizontal {"
                              "    background: %1;"
                              "    border-radius: 4px;"
                              "}"
                              ).arg(color));
}

void GraphPage::setupUI()
{
    setMinimumSize(1100, 750);

    // Устанавливаем стиль фона
    setStyleSheet("background-color: #1e1e2e;");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // ==================== ЛЕВАЯ ПАНЕЛЬ ====================
    QWidget* leftPanel = new QWidget();
    leftPanel->setMaximumWidth(380);
    leftPanel->setMinimumWidth(320);
    leftPanel->setStyleSheet("background-color: #2a2a3a; border-radius: 15px;");

    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(15, 20, 15, 20);

    // Информация о пользователе
    QGroupBox* userBox = new QGroupBox("👤 Пользователь");
    userBox->setStyleSheet("QGroupBox { color: #ffffff; font-weight: bold; border: 1px solid #444; border-radius: 8px; margin-top: 10px; }"
                           "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");

    QVBoxLayout* userLayout = new QVBoxLayout();
    m_userLabel = new QLabel("User: ");
    m_emailLabel = new QLabel("email@example.com");
    m_userLabel->setStyleSheet("color: #4CAF50; font-size: 14px; font-weight: bold;");
    m_emailLabel->setStyleSheet("color: #aaaaaa; font-size: 12px;");
    userLayout->addWidget(m_userLabel);
    userLayout->addWidget(m_emailLabel);
    userBox->setLayout(userLayout);

    // Параметр a
    QGroupBox* aBox = new QGroupBox("🔵 Параметр a (сдвиг кубической части)");
    aBox->setStyleSheet("QGroupBox { color: #64b5f6; font-weight: bold; border: 1px solid #444; border-radius: 8px; margin-top: 10px; }"
                        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");

    QVBoxLayout* aLayout = new QVBoxLayout();

    m_aSlider = new QSlider(Qt::Horizontal);
    m_aSlider->setRange(-200, 200);
    m_aSlider->setValue(0);
    m_aSlider->setTickInterval(50);
    m_aSlider->setTickPosition(QSlider::TicksBelow);
    setupStyledSlider(m_aSlider, "#64b5f6");

    m_aValueLabel = new QLabel("a = 0.00");
    m_aValueLabel->setAlignment(Qt::AlignCenter);
    m_aValueLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #64b5f6; background-color: #1e1e2e; padding: 5px; border-radius: 8px;");

    aLayout->addWidget(m_aSlider);
    aLayout->addWidget(m_aValueLabel);
    aBox->setLayout(aLayout);

    // Параметр b
    QGroupBox* bBox = new QGroupBox("🟢 Параметр b (сдвиг логарифма и асимптоты)");
    bBox->setStyleSheet("QGroupBox { color: #81c784; font-weight: bold; border: 1px solid #444; border-radius: 8px; margin-top: 10px; }"
                        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");

    QVBoxLayout* bLayout = new QVBoxLayout();

    m_bSlider = new QSlider(Qt::Horizontal);
    m_bSlider->setRange(-200, 200);
    m_bSlider->setValue(0);
    m_bSlider->setTickInterval(50);
    m_bSlider->setTickPosition(QSlider::TicksBelow);
    setupStyledSlider(m_bSlider, "#81c784");

    m_bValueLabel = new QLabel("b = 0.00");
    m_bValueLabel->setAlignment(Qt::AlignCenter);
    m_bValueLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #81c784; background-color: #1e1e2e; padding: 5px; border-radius: 8px;");

    bLayout->addWidget(m_bSlider);
    bLayout->addWidget(m_bValueLabel);
    bBox->setLayout(bLayout);

    // Кнопки
    m_buildButton = new QPushButton("📊 ПОСТРОИТЬ ГРАФИК");
    m_buildButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2196F3, stop:1 #1976D2);"
                                 "color: white; padding: 12px; font-size: 14px; font-weight: bold;"
                                 "border-radius: 10px;");

    m_solveButton = new QPushButton("🔍 НАЙТИ КОРНИ УРАВНЕНИЯ");
    m_solveButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FF9800, stop:1 #F57C00);"
                                 "color: white; padding: 12px; font-size: 14px; font-weight: bold;"
                                 "border-radius: 10px;");

    m_exitButton = new QPushButton("🏠 ВЕРНУТЬСЯ НА ГЛАВНУЮ");
    m_exitButton->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #9C27B0, stop:1 #7B1FA2);"
                                "color: white; padding: 10px; font-size: 12px; font-weight: bold;"
                                "border-radius: 8px;");

    // Корни (убираем "Информация о функции")
    QGroupBox* rootsBox = new QGroupBox("📌 НАЙДЕННЫЕ КОРНИ");
    rootsBox->setStyleSheet("QGroupBox { color: #ffd54f; font-weight: bold; border: 1px solid #444; border-radius: 8px; margin-top: 10px; }"
                            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");

    QVBoxLayout* rootsLayout = new QVBoxLayout();
    m_rootsLabel = new QLabel("⚡ Нажмите «Найти корни» для отображения результатов");
    m_rootsLabel->setWordWrap(true);
    m_rootsLabel->setStyleSheet("font-family: monospace; font-size: 11px; color: #ffd54f; background-color: #1e1e2e; padding: 10px; border-radius: 8px;");
    rootsLayout->addWidget(m_rootsLabel);
    rootsBox->setLayout(rootsLayout);

    // Собираем левую панель
    leftLayout->addWidget(userBox);
    leftLayout->addWidget(aBox);
    leftLayout->addWidget(bBox);
    leftLayout->addSpacing(20);
    leftLayout->addWidget(m_buildButton);
    leftLayout->addWidget(m_solveButton);
    leftLayout->addSpacing(30);
    leftLayout->addWidget(rootsBox);
    leftLayout->addStretch();
    leftLayout->addWidget(m_exitButton);

    // ==================== ПРАВАЯ ПАНЕЛЬ (ГРАФИК) ====================
    QWidget* rightPanel = new QWidget();
    rightPanel->setStyleSheet("background-color: #1a1a2a; border-radius: 15px;");

    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок графика с градиентом
    QLabel* graphTitle = new QLabel("🎨 ГРАФИК ФУНКЦИИ f(x)");
    graphTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;"
                              "background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2196F3, stop:1 #9C27B0);"
                              "padding: 12px; border-radius: 12px;");
    graphTitle->setAlignment(Qt::AlignCenter);

    // Легенда
    QFrame* legendFrame = new QFrame();
    legendFrame->setStyleSheet("background-color: #252536; border-radius: 10px;");
    QHBoxLayout* legendLayout = new QHBoxLayout(legendFrame);

    QLabel* redLegend = new QLabel("🔴 f₁(x) = ln(x + b)");
    QLabel* blueLegend = new QLabel("🔵 f₂(x) = x³ - 3x + a");
    QLabel* greenLegend = new QLabel("🟢 f₃(x) = 1/(x-2) - b");

    redLegend->setStyleSheet("color: #ff6b6b; font-size: 12px; padding: 5px;");
    blueLegend->setStyleSheet("color: #64b5f6; font-size: 12px; padding: 5px;");
    greenLegend->setStyleSheet("color: #81c784; font-size: 12px; padding: 5px;");

    legendLayout->addWidget(redLegend);
    legendLayout->addWidget(blueLegend);
    legendLayout->addWidget(greenLegend);
    legendLayout->addStretch();

    // Место для графика
    QWidget* plotArea = new QWidget();
    plotArea->setMinimumSize(650, 500);
    plotArea->setStyleSheet("background-color: #0d0d1a; border: 2px solid #333; border-radius: 10px;");

    QVBoxLayout* plotLayout = new QVBoxLayout(plotArea);

    // Красивое ASCII-представление графика
    QString plotText =
        "╔══════════════════════════════════════════════════════════════════════════════════╗\n"
        "║                                    ГРАФИК                                        ║\n"
        "╠══════════════════════════════════════════════════════════════════════════════════╣\n"
        "║                                                                                  ║\n"
        "║    f(x) ▲                                                                        ║\n"
        "║         │                                                                        ║\n"
        "║    10   │                                    ╱                                   ║\n"
        "║         │                                  ╱                                     ║\n"
        "║     5   │                                ╱                                       ║\n"
        "║         │                              ╱                                         ║\n"
        "║     0   ├─────────────────────────────╱──────────────────────────────────────►  ║\n"
        "║         │                            ╱                                           ║\n"
        "║    -5   │                          ╱                                             ║\n"
        "║         │                        ╱                                               ║\n"
        "║   -10   │                      ╱                                                 ║\n"
        "║         │                                                                        ║\n"
        "║         │   -5      -2      0       2      5                                    ║\n"
        "╚══════════════════════════════════════════════════════════════════════════════════╝";

    QLabel* plotLabel = new QLabel(plotText);
    plotLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    plotLabel->setStyleSheet("font-family: monospace; font-size: 11px; padding: 10px; color: #00ff00; background-color: #000000; border-radius: 8px;");
    plotLabel->setWordWrap(true);
    plotLayout->addWidget(plotLabel);

    // Текущие параметры
    QFrame* paramsFrame = new QFrame();
    paramsFrame->setStyleSheet("background-color: #252536; border-radius: 8px;");
    QHBoxLayout* paramsLayout = new QHBoxLayout(paramsFrame);

    QLabel* paramsLabel = new QLabel("⚙️ Текущие параметры:");
    paramsLabel->setStyleSheet("color: #aaa; font-size: 11px;");

    QLabel* aParamLabel = new QLabel("a = 0.00");
    aParamLabel->setStyleSheet("color: #64b5f6; font-size: 12px; font-weight: bold;");

    QLabel* bParamLabel = new QLabel("b = 0.00");
    bParamLabel->setStyleSheet("color: #81c784; font-size: 12px; font-weight: bold;");

    paramsLayout->addWidget(paramsLabel);
    paramsLayout->addStretch();
    paramsLayout->addWidget(aParamLabel);
    paramsLayout->addSpacing(20);
    paramsLayout->addWidget(bParamLabel);

    // Обновляем значения параметров при изменении
    connect(m_aSlider, &QSlider::valueChanged, [aParamLabel, this](int val) {
        double a = val / 10.0;
        aParamLabel->setText(QString("a = %1").arg(a, 0, 'f', 2));
    });
    connect(m_bSlider, &QSlider::valueChanged, [bParamLabel, this](int val) {
        double b = val / 10.0;
        bParamLabel->setText(QString("b = %1").arg(b, 0, 'f', 2));
    });

    rightLayout->addWidget(graphTitle);
    rightLayout->addWidget(legendFrame);
    rightLayout->addWidget(plotArea, 1);
    rightLayout->addWidget(paramsFrame);

    // Подключаем сигналы
    connect(m_aSlider, &QSlider::valueChanged, this, &GraphPage::onASliderChanged);
    connect(m_bSlider, &QSlider::valueChanged, this, &GraphPage::onBSliderChanged);
    connect(m_buildButton, &QPushButton::clicked, this, &GraphPage::onBuildClicked);
    connect(m_solveButton, &QPushButton::clicked, this, &GraphPage::onSolveClicked);
    connect(m_exitButton, &QPushButton::clicked, this, &GraphPage::onExitClicked);

    // Добавляем панели в главный layout
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel, 1);

    // Инициализация значений
    onASliderChanged(0);
    onBSliderChanged(0);
}

void GraphPage::onASliderChanged(int value)
{
    m_currentA = value / 10.0;
    m_aValueLabel->setText(QString("a = %1").arg(m_currentA, 0, 'f', 2));
}

void GraphPage::onBSliderChanged(int value)
{
    m_currentB = value / 10.0;
    m_bValueLabel->setText(QString("b = %1").arg(m_currentB, 0, 'f', 2));
}

void GraphPage::onBuildClicked()
{
    updateGraph(m_currentA, m_currentB);

    QMessageBox::information(this, "📊 График функции",
                             QString("✅ График функции построен для параметров:\n\n"
                                     "🔵 a = %1\n"
                                     "🟢 b = %2\n\n"
                                     "🔴 Красный: f₁(x) = ln(x + b)\n"
                                     "🔵 Синий: f₂(x) = x³ - 3x + a\n"
                                     "🟢 Зеленый: f₃(x) = 1/(x-2) - b")
                                 .arg(m_currentA, 0, 'f', 2)
                                 .arg(m_currentB, 0, 'f', 2));
}

void GraphPage::onSolveClicked()
{
    emit solveRequested(m_currentA, m_currentB);
}

void GraphPage::onExitClicked()
{
    emit backToMain();
}

double GraphPage::evaluate(int piece, double x, double a, double b)
{
    switch(piece) {
    case 1:
        if (x + b > 1e-6 && x < 2) {
            return log(x + b);
        }
        return NAN;
    case 2:
        return pow(x, 3) - 3 * x + a;
    case 3:
        if (fabs(x - 2) > 1e-6 && x > 2) {
            return 1.0 / (x - 2) - b;
        }
        return NAN;
    default:
        return NAN;
    }
}

void GraphPage::updateGraph(double a, double b)
{
    QString info = QString(
                       "╔══════════════════════════════════════════════════════════════════════╗\n"
                       "║                         ТРИ ГРАФИКА                                   ║\n"
                       "╠══════════════════════════════════════════════════════════════════════╣\n"
                       "║                                                                      ║\n"
                       "║  🔴 ГРАФИК 1 (КРАСНЫЙ):                                              ║\n"
                       "║     f₁(x) = ln(x + %1)                                               ║\n"
                       "║     Область: x ∈ (-%1, 2)                                            ║\n"
                       "║                                                                      ║\n"
                       "║  🔵 ГРАФИК 2 (СИНИЙ):                                                ║\n"
                       "║     f₂(x) = x³ - 3x + %2                                             ║\n"
                       "║     Область: x ∈ (-∞, +∞)                                            ║\n"
                       "║                                                                      ║\n"
                       "║  🟢 ГРАФИК 3 (ЗЕЛЕНЫЙ):                                              ║\n"
                       "║     f₃(x) = 1/(x-2) - %1                                             ║\n"
                       "║     Область: x ∈ (2, +∞)                                             ║\n"
                       "║                                                                      ║\n"
                       "╠══════════════════════════════════════════════════════════════════════╣\n"
                       "║  📊 Параметры построения:                                            ║\n"
                       "║     • a = %2                                                         ║\n"
                       "║     • b = %1                                                         ║\n"
                       "╚══════════════════════════════════════════════════════════════════════╝")
                       .arg(b, 0, 'f', 2)
                       .arg(a, 0, 'f', 2);

    m_rootsLabel->setText(info);
}

void GraphPage::updateRoots(const QString& roots)
{
    m_rootsLabel->setText(roots);
}

void GraphPage::setLogin(const QString& login, const QString& email)
{
    m_login = login;
    m_email = email;
    m_userLabel->setText("👤 " + login);
    m_emailLabel->setText("📧 " + email);
}
