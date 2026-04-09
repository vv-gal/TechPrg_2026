#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "functionmodel.h"
#include "servercontroller.h"
#include "qcustomplot.h"

class MainView : public QWidget
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);
    void setUserData(const ServerController::UserData &user);

signals:
    void logoutRequested();

private slots:
    void onCalculateClicked();
    void onLogoutClicked();
    void updateInfoOnly();
    void onSliderAChanged(int value);
    void onSliderBChanged(int value);

private:
    void setupUI();
    void setupPlot();
    void plotFunction(const QVector<QPointF> &data);
    void displayFunctionInfo();

    FunctionModel m_functionModel;

    QCustomPlot *m_customPlot;

    // Для параметра A
    QSlider *m_sliderA;
    QDoubleSpinBox *m_spinA;
    QLabel *m_valueA;

    // Для параметра B
    QSlider *m_sliderB;
    QDoubleSpinBox *m_spinB;
    QLabel *m_valueB;

    QPushButton *m_calculateButton;
    QPushButton *m_logoutButton;
    QTextEdit *m_infoText;
    QLabel *m_userLabel;

    // Диапазоны параметров
    const double MIN_A = -50.0;
    const double MAX_A = 50.0;
    const double MIN_B = -20.0;
    const double MAX_B = 20.0;
};

#endif
