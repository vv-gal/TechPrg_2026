#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>
#include <QComboBox>

class GraphPage;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateStats(int attempts, int solved, int score);
    void showMessage(const QString& msg, bool isError = false);
    void setUserData(const QString& login, const QString& email);

signals:
    void checkRequested(int task, int variant, const QString& answer);
    void logoutRequested();
    void solveEquationRequested(double a, double b);

private slots:
    void onCheckClicked();
    void onLogoutClicked();
    void onNavigateClicked();
    void onBackToMain();

private:
    void setupUI();
    void setupHomePage();
    void setupGraphPage();

    QStackedWidget* m_stackedWidget;
    QWidget* m_homePage;
    GraphPage* m_graphPage;
    QPushButton* m_navButton;

    QLabel* m_welcomeLabel;
    QLabel* m_statsLabel;
    QComboBox* m_taskCombo;
    QComboBox* m_variantCombo;
    QLineEdit* m_answerEdit;
    QPushButton* m_checkButton;
    QPushButton* m_logoutButton;
    QLabel* m_resultLabel;

    QString m_login;
    QString m_email;
    int m_currentPage;
};

#endif // MAINWINDOW_H
