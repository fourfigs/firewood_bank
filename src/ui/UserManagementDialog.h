#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class UserManagementDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserManagementDialog(QWidget *parent = nullptr);
    ~UserManagementDialog() = default;

private slots:
    void loadUsers();
    void viewUserProfile();
    void editUser();
    void addUser();
    void removeUser();
    void searchUsers();

private:
    void setupUI();
    
    QTableWidget *m_usersTable = nullptr;
    QLineEdit *m_searchEdit = nullptr;
    QPushButton *m_viewButton = nullptr;
    QPushButton *m_editButton = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_removeButton = nullptr;
    QPushButton *m_refreshButton = nullptr;
};

