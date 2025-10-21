#pragma once

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QTableWidget>
#include <QCalendarWidget>
#include <QFrame>
#include <QGroupBox>

struct UserInfo {
    QString username;
    QString fullName;
    QString role;
    QString contactNumber;
    QString email;
};

class DashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit DashboardWidget(const UserInfo &userInfo, QWidget *parent = nullptr);
    ~DashboardWidget() = default;
    
public slots:
    void refreshData();  // Refresh all dashboard data

private:
    void setupUI();
    void createStatisticsSection();  // NEW: For leads and admins
    void createTopSection();
    void createBottomSection();
    void loadUpcomingOrders();
    void loadCurrentInventory();
    void loadEmergencies();
    void loadLowInventory();
    void updateMonthlyCalendar();
    void loadStatistics();  // NEW: Load statistics data
    void checkInventoryAlerts();  // NEW: Check for low inventory alerts
    
    // User info
    UserInfo m_userInfo;
    
    // Statistics widgets (for leads and admins)
    QLabel *m_totalHouseholdsLabel = nullptr;
    QLabel *m_woodDeliveredWeekLabel = nullptr;
    QLabel *m_woodDeliveredMonthLabel = nullptr;
    QLabel *m_woodDeliveredYearLabel = nullptr;
    QLabel *m_woodDeliveredAllTimeLabel = nullptr;
    QLabel *m_expenseWeekLabel = nullptr;
    QLabel *m_expenseMonthLabel = nullptr;
    QLabel *m_expenseYearLabel = nullptr;
    QLabel *m_expenseAllTimeLabel = nullptr;
    
    // Top section widgets
    QLabel *m_userNameLabel = nullptr;
    QLabel *m_userContactLabel = nullptr;
    QLabel *m_userRoleLabel = nullptr;
    QTextEdit *m_emergenciesText = nullptr;
    QTextEdit *m_lowInventoryText = nullptr;
    QCalendarWidget *m_monthlyCalendar = nullptr;
    QWidget *m_inventoryAlertsWidget = nullptr;  // NEW: Inventory alerts widget
    
    // Bottom section widgets
    QTableWidget *m_upcomingOrdersTable = nullptr;
    QCalendarWidget *m_twoWeekCalendar = nullptr;
    QTableWidget *m_currentInventoryTable = nullptr;
    
    // Inventory At-A-Glance widgets
    QLabel *m_splitWoodLabel = nullptr;
    QLabel *m_roundsWoodLabel = nullptr;
    QLabel *m_regularGasLabel = nullptr;
    QLabel *m_mixedGasLabel = nullptr;
    QLabel *m_sawsLabel = nullptr;
    
    void loadInventoryAtAGlance();  // Load real inventory data
    
    QFrame* createSection(const QString &title, QWidget *content);
    QGroupBox* createGroupBox(const QString &title);
    QFrame* createSeparator();
};

