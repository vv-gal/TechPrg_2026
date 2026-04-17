#ifndef GRAPHPAGE_H
#define GRAPHPAGE_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QVector>
#include <QPair>
#include <QTextEdit>
#include <QFrame>
#include <QProgressBar>

class QCustomPlot;

class GraphPage : public QWidget
{
    Q_OBJECT

public:
    explicit GraphPage(QWidget *parent = nullptr);
    ~GraphPage();

    void updateGraph(double a, double b);
    void updateRoots(const QString& roots);
    void setLogin(const QString& login, const QString& email);

signals:
    void solveRequested(double a, double b);
    void backToMain();

private slots:
    void onASliderChanged(int value);
    void onBSliderChanged(int value);
    void onBuildClicked();
    void onSolveClicked();
    void onExitClicked();

private:
    void setupUI();
    double evaluate(int piece, double x, double a, double b);
    void generateFunctionInfo(double a, double b);
    void setupStyledSlider(QSlider* slider, const QString& color);

    QCustomPlot* m_customPlot;

    QSlider* m_aSlider;
    QSlider* m_bSlider;
    QLabel* m_aValueLabel;
    QLabel* m_bValueLabel;
    QLabel* m_userLabel;
    QLabel* m_emailLabel;

    QPushButton* m_buildButton;
    QPushButton* m_solveButton;
    QPushButton* m_exitButton;

    QTextEdit* m_functionInfo;
    QLabel* m_rootsLabel;

    double m_currentA;
    double m_currentB;
    QString m_login;
    QString m_email;
};

#endif // GRAPHPAGE_H
