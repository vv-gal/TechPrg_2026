#include "mainview.h"
#include <QMessageBox>
#include <QPen>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainView::MainView(QWidget *parent) : QWidget(parent)
{
    setupUI();
    setupPlot();
}

void MainView::setupUI()
{
    setMinimumSize(1200, 700);
    setWindowTitle("Math Client - Решение функции");

    setStyleSheet(R"(
        QWidget {
            background-color: #f5f5f5;
        }
        QGroupBox {
            font-size: 14px;
            font-weight: bold;
            border: 2px solid #ccc;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QSlider {
            min-height: 20px;
        }
        QSlider::groove:horizontal {
            height: 6px;
            background: #ddd;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #4CAF50;
            width: 16px;
            height: 16px;
            margin: -5px 0;
            border-radius: 8px;
        }
        QSlider::handle:horizontal:hover {
            background: #45a049;
        }
        QDoubleSpinBox {
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 12px;
            min-width: 80px;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton#logoutButton {
            background-color: #f44336;
        }
        QPushButton#logoutButton:hover {
            background-color: #da190b;
        }
        QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            font-family: monospace;
            background-color: #fafafa;
        }
        QLabel#valueLabel {
            font-weight: bold;
            color: #4CAF50;
            font-size: 14px;
            min-width: 60px;
        }
    )");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // ========== ЛЕВАЯ ЧАСТЬ - ГРАФИК ==========
    QGroupBox *graphGroup = new QGroupBox("📈 График функции", this);
    QVBoxLayout *graphLayout = new QVBoxLayout(graphGroup);

    m_customPlot = new QCustomPlot(graphGroup);
    graphLayout->addWidget(m_customPlot);

    mainLayout->addWidget(graphGroup, 2);

    // ========== ПРАВАЯ ЧАСТЬ - ПАРАМЕТРЫ ==========
    QGroupBox *controlsGroup = new QGroupBox("⚙️ Параметры", this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsGroup);

    m_userLabel = new QLabel("Пользователь: не авторизован", this);
    m_userLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
    controlsLayout->addWidget(m_userLabel);

    controlsLayout->addSpacing(20);

    // ========== ПАРАМЕТР A ==========
    QLabel *labelA = new QLabel("Параметр a (сдвиг кубической части):", this);
    labelA->setStyleSheet("font-weight: bold;");
    controlsLayout->addWidget(labelA);

    // Горизонтальный layout для ползунка и поля ввода
    QHBoxLayout *layoutA = new QHBoxLayout();

    m_sliderA = new QSlider(Qt::Horizontal, this);
    m_sliderA->setRange(0, 1000);  // 0-1000 для точности
    m_sliderA->setValue(500);      // середина = 0
    m_sliderA->setTickPosition(QSlider::TicksBelow);
    m_sliderA->setTickInterval(100);

    m_spinA = new QDoubleSpinBox(this);
    m_spinA->setRange(MIN_A, MAX_A);
    m_spinA->setSingleStep(0.5);
    m_spinA->setValue(0);
    m_spinA->setSuffix("  (от -50 до 50)");

    m_valueA = new QLabel(this);
    m_valueA->setObjectName("valueLabel");
    m_valueA->setAlignment(Qt::AlignRight);
    m_valueA->setText("a = 0.00");

    layoutA->addWidget(m_sliderA, 3);
    layoutA->addWidget(m_spinA, 2);
    layoutA->addWidget(m_valueA, 1);
    controlsLayout->addLayout(layoutA);

    controlsLayout->addSpacing(15);

    // ========== ПАРАМЕТР B ==========
    QLabel *labelB = new QLabel("Параметр b (сдвиг логарифма и асимптоты):", this);
    labelB->setStyleSheet("font-weight: bold;");
    controlsLayout->addWidget(labelB);

    QHBoxLayout *layoutB = new QHBoxLayout();

    m_sliderB = new QSlider(Qt::Horizontal, this);
    m_sliderB->setRange(0, 1000);
    m_sliderB->setValue(500);  // середина = 1? Нужно преобразование
    m_sliderB->setTickPosition(QSlider::TicksBelow);
    m_sliderB->setTickInterval(100);

    m_spinB = new QDoubleSpinBox(this);
    m_spinB->setRange(MIN_B, MAX_B);
    m_spinB->setSingleStep(0.2);
    m_spinB->setValue(1);
    m_spinB->setSuffix("  (от -20 до 20)");

    m_valueB = new QLabel(this);
    m_valueB->setObjectName("valueLabel");
    m_valueB->setAlignment(Qt::AlignRight);
    m_valueB->setText("b = 1.00");

    layoutB->addWidget(m_sliderB, 3);
    layoutB->addWidget(m_spinB, 2);
    layoutB->addWidget(m_valueB, 1);
    controlsLayout->addLayout(layoutB);

    controlsLayout->addSpacing(20);

    m_calculateButton = new QPushButton("🔄 Построить график", this);
    controlsLayout->addWidget(m_calculateButton);

    controlsLayout->addSpacing(20);

    QLabel *infoTitle = new QLabel("📖 Информация о функции:", this);
    infoTitle->setStyleSheet("font-weight: bold;");
    controlsLayout->addWidget(infoTitle);

    m_infoText = new QTextEdit(this);
    m_infoText->setReadOnly(true);
    m_infoText->setMinimumHeight(300);
    controlsLayout->addWidget(m_infoText);

    controlsLayout->addStretch();

    m_logoutButton = new QPushButton("🚪 Выйти", this);
    m_logoutButton->setObjectName("logoutButton");
    controlsLayout->addWidget(m_logoutButton);

    mainLayout->addWidget(controlsGroup, 1);

    setLayout(mainLayout);

    // ========== ПОДКЛЮЧЕНИЕ СИГНАЛОВ ==========
    connect(m_calculateButton, &QPushButton::clicked, this, &MainView::onCalculateClicked);
    connect(m_logoutButton, &QPushButton::clicked, this, &MainView::onLogoutClicked);

    // Связь между ползунком и spinBox
    connect(m_sliderA, &QSlider::valueChanged, this, &MainView::onSliderAChanged);
    connect(m_sliderB, &QSlider::valueChanged, this, &MainView::onSliderBChanged);
    connect(m_spinA, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) {
                // Преобразуем значение spinBox в положение ползунка
                int sliderValue = int((value - MIN_A) / (MAX_A - MIN_A) * 1000);
                m_sliderA->setValue(qBound(0, sliderValue, 1000));
                m_valueA->setText(QString("a = %1").arg(value, 0, 'f', 2));
                updateInfoOnly();
            });
    connect(m_spinB, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) {
                int sliderValue = int((value - MIN_B) / (MAX_B - MIN_B) * 1000);
                m_sliderB->setValue(qBound(0, sliderValue, 1000));
                m_valueB->setText(QString("b = %1").arg(value, 0, 'f', 2));
                updateInfoOnly();
            });

    // Инициализация значений
    onSliderAChanged(500);
    onSliderBChanged(500);
}

void MainView::onSliderAChanged(int value)
{
    // Преобразуем положение ползунка (0-1000) в значение параметра
    double a = MIN_A + (double)value / 1000.0 * (MAX_A - MIN_A);
    m_spinA->setValue(a);
    m_valueA->setText(QString("a = %1").arg(a, 0, 'f', 2));
}

void MainView::onSliderBChanged(int value)
{
    double b = MIN_B + (double)value / 1000.0 * (MAX_B - MIN_B);
    m_spinB->setValue(b);
    m_valueB->setText(QString("b = %1").arg(b, 0, 'f', 2));
}

void MainView::setupPlot()
{
    m_customPlot->setBackground(QBrush(QColor(255, 255, 255)));
    m_customPlot->axisRect()->setBackground(QBrush(QColor(250, 250, 250)));

    m_customPlot->xAxis->setLabel("x");
    m_customPlot->yAxis->setLabel("f(x)");
    m_customPlot->xAxis->setRange(-3, 5);
    m_customPlot->yAxis->setRange(-8, 8);

    m_customPlot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200), 0.5, Qt::DotLine));
    m_customPlot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 0.5, Qt::DotLine));

    m_customPlot->addGraph();
    m_customPlot->graph(0)->setPen(QPen(QColor(76, 175, 80), 2));
    m_customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    m_customPlot->graph(0)->setName("f(x)");

    m_customPlot->legend->setVisible(true);
    m_customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 200)));
    m_customPlot->legend->setBorderPen(QPen(QColor(150, 150, 150)));

    m_customPlot->replot();
}

void MainView::plotFunction(const QVector<QPointF> &data)
{
    if (!m_customPlot) return;

    QVector<double> xData, yData;

    for (const QPointF &point : data) {
        if (!std::isnan(point.y()) && !std::isinf(point.y())) {
            xData.append(point.x());
            double y = point.y();
            if (y > 10) y = 10;
            if (y < -10) y = -10;
            yData.append(y);
        } else {
            xData.append(NAN);
            yData.append(NAN);
        }
    }

    m_customPlot->graph(0)->setData(xData, yData);
    m_customPlot->xAxis->setRange(-3, 5);
    m_customPlot->yAxis->setRange(-8, 8);
    m_customPlot->replot();
}

void MainView::onCalculateClicked()
{
    double a = m_spinA->value();
    double b = m_spinB->value();

    QVector<QPointF> data = m_functionModel.generatePlotData(a, b, -2.5, 4.5, 2000);
    plotFunction(data);
    displayFunctionInfo();

    QMessageBox::information(this, "График построен",
                             QString("График функции построен для параметров:\na = %1, b = %2")
                                 .arg(a, 0, 'f', 2).arg(b, 0, 'f', 2));
}

void MainView::updateInfoOnly()
{
    displayFunctionInfo();
}

void MainView::displayFunctionInfo()
{
    double a = m_spinA->value();
    double b = m_spinB->value();

    auto info = m_functionModel.getFunctionInfo(a, b);
    double f1 = m_functionModel.calculate(1.0, a, b);

    QString text = QString(
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                       "📐 ФУНКЦИЯ:\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                       "f(x) = {\n"
                       "  ln(x + %1),    при -1 < x < 0\n"
                       "  x³ - 3x + %2,    при 0 ≤ x ≤ 2\n"
                       "  1/(x-2) - %3,    при x ≥ 2\n"
                       "}\n\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                       "📌 ОБЛАСТЬ ОПРЕДЕЛЕНИЯ:\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                       "%4\n\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                       "📈 АСИМПТОТЫ:\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                       "%5\n\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                       "🔍 ЭКСТРЕМУМЫ:\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                       "%6\n\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                       "⭐ ОСОБЫЕ ТОЧКИ:\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                       "%7\n\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                       "📊 ПРИМЕР ВЫЧИСЛЕНИЯ:\n"
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                       "f(1) = %8\n\n"
                       "Для построения графика нажмите кнопку 'Построить график'"
                       ).arg(b, 0, 'f', 2).arg(a, 0, 'f', 2).arg(b, 0, 'f', 2)
                       .arg(info.domain)
                       .arg(info.asymptotes)
                       .arg(info.extrema)
                       .arg(info.specialPoints)
                       .arg(f1, 0, 'f', 4);

    m_infoText->setText(text);
}

void MainView::onLogoutClicked()
{
    emit logoutRequested();
    this->close();
}

void MainView::setUserData(const ServerController::UserData &user)
{
    m_userLabel->setText(QString("👤 Пользователь: %1\n📧 %2").arg(user.username).arg(user.email));
}
