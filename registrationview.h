#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class RegistrationView : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationView(QWidget *parent = nullptr);
    void showError(const QString &message);
    void clearFields();

signals:
    void registrationRequested(const QString &username, const QString &email,
                               const QString &password, const QString &confirmPassword);
    void backToLoginRequested();

private slots:
    void onRegisterClicked();
    void onBackClicked();

private:
    void setupUI();

    QLineEdit *m_usernameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QPushButton *m_registerButton;
    QPushButton *m_backButton;
    QLabel *m_errorLabel;
};

#endif
