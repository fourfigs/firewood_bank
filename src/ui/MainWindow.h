#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QString>

struct UserInfo; // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString &username, const QString &fullName, 
                       const QString &role, QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void addClient();
    void editClient();
    void onClientDoubleClicked(const QModelIndex &index);

private:
    void setupUI();
    void setupDatabaseModels();
    void setupToolbar();
    void logDatabaseStatus();
    void loadUserInfo();
    
    // User info
    QString m_username;
    QString m_fullName;
    QString m_role;
    QString m_contactNumber;
    QString m_email;
    
    // UI Components
    QTabWidget *m_tabs = nullptr;
    QTableView *m_householdsView = nullptr;
    QTableView *m_inventoryView = nullptr;
    
    // Database Models
    QSqlTableModel *m_householdsModel = nullptr;
    QSqlTableModel *m_inventoryModel = nullptr;
};

// Factory function for creating MainWindow instances
QMainWindow* createMainWindow(const QString &username, const QString &fullName, const QString &role);

