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

signals:
    void logoutRequested();

private slots:
    void logout();
    void viewMyProfile();
    void viewEmployeeDirectory();
    void viewProfileChangeRequests();
    void viewDeliveryLog();  // NEW: View delivery log for leads/admins
    void addClient();
    void editClient();
    void onClientDoubleClicked(const QModelIndex &index);
    void addWorkOrder();
    void editWorkOrder();
    void onWorkOrderDoubleClicked(const QModelIndex &index);
    void addInventoryItem();
    void editInventoryItem();
    void onInventoryDoubleClicked(const QModelIndex &index);
    void manageEquipment();
    void manageUsers();
    void manageAgencies();
    void loadSampleData();
    void exportClientsToCSV();
    void exportOrdersToCSV();
    void exportInventoryToCSV();
    void clearAllData();
    void deleteSelectedClient();
    void deleteSelectedOrder();
    void deleteSelectedInventoryItem();
    void searchClients(const QString &text);
    void searchOrders(const QString &text);
    void searchInventory(const QString &text);

private:
    void setupUI();
    void setupMenuBar();
    void setupDatabaseModels();
    void setupToolbar();
    void logDatabaseStatus();
    void loadUserInfo();
    void applyRoleBasedPermissions();
    void setupVolunteerView();
    void addSearchBoxToTab(QWidget *tabWidget, QTableView *tableView, QSqlTableModel *model, const QString &searchColumns);
    void updateStatusBar();
    void setupKeyboardShortcuts();
    
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
    QTableView *m_ordersView = nullptr;
    class DashboardWidget *m_dashboard = nullptr;  // Dashboard reference for refreshing
    class QStatusBar *m_statusBar = nullptr;
    
    // Database Models
    QSqlTableModel *m_householdsModel = nullptr;
    QSqlTableModel *m_inventoryModel = nullptr;
    QSqlTableModel *m_ordersModel = nullptr;
};

// Factory function for creating MainWindow instances
QMainWindow* createMainWindow(const QString &username, const QString &fullName, const QString &role);

