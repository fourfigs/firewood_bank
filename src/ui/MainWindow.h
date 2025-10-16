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

<<<<<<< Updated upstream
private slots:
    void addClient();
    void editClient();
    void onClientDoubleClicked(const QModelIndex &index);
=======
signals:
    void logoutRequested();

private slots:
    void logout();
    void viewMyProfile();
    void viewEmployeeDirectory();
    void viewProfileChangeRequests();
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
>>>>>>> Stashed changes

private:
    void setupUI();
    void setupDatabaseModels();
    void setupToolbar();
    void logDatabaseStatus();
    void loadUserInfo();
<<<<<<< Updated upstream
=======
    void applyRoleBasedPermissions();
    void setupVolunteerView();
>>>>>>> Stashed changes
    
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
<<<<<<< Updated upstream
=======
    QTableView *m_ordersView = nullptr;
>>>>>>> Stashed changes
    
    // Database Models
    QSqlTableModel *m_householdsModel = nullptr;
    QSqlTableModel *m_inventoryModel = nullptr;
<<<<<<< Updated upstream
=======
    QSqlTableModel *m_ordersModel = nullptr;
>>>>>>> Stashed changes
};

// Factory function for creating MainWindow instances
QMainWindow* createMainWindow(const QString &username, const QString &fullName, const QString &role);

