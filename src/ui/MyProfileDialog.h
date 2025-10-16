#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

class MyProfileDialog : public QDialog {
    Q_OBJECT

public:
    explicit MyProfileDialog(const QString &username, const QString &role, QWidget *parent = nullptr);
    ~MyProfileDialog() = default;

private slots:
    void loadProfile();
    void requestChange();

private:
    void setupUI();
    void submitChangeRequest(const QString &fieldName, const QString &oldValue, const QString &newValue);
    
    QString m_username;
    QString m_role;
    int m_userId;
    
    // Display labels (read-only)
    QLabel *m_usernameLabel = nullptr;
    QLabel *m_fullNameLabel = nullptr;
    QLabel *m_roleLabel = nullptr;
    QLabel *m_emailLabel = nullptr;
    QLabel *m_phoneLabel = nullptr;
    QLabel *m_availabilityLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    
    // Editable fields (for change requests)
    QLineEdit *m_newPhoneEdit = nullptr;
    QTextEdit *m_newAvailabilityEdit = nullptr;
    QLineEdit *m_newEmailEdit = nullptr;
    
    QPushButton *m_submitButton = nullptr;
};

