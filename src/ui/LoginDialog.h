#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() = default;

    QString getUsername() const;
    QString getRole() const;

private slots:
    void onLoginClicked();

private:
    void setupUI();
    bool validateCredentials(const QString &username, const QString &password);
    
    QLineEdit *m_usernameEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QPushButton *m_loginButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QLabel *m_errorLabel = nullptr;
    
    QString m_loggedInUsername;
    QString m_loggedInRole;
};

