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

private:
    void setupUI();
    void createTopSection();
    void createBottomSection();
    void loadUpcomingOrders();
    void loadCurrentInventory();
    void loadEmergencies();
    void loadLowInventory();
    void updateMonthlyCalendar();
    
    // User info
    UserInfo m_userInfo;
    
    // Top section widgets
    QLabel *m_userNameLabel = nullptr;
    QLabel *m_userContactLabel = nullptr;
    QLabel *m_userRoleLabel = nullptr;
    QTextEdit *m_emergenciesText = nullptr;
    QTextEdit *m_lowInventoryText = nullptr;
    QCalendarWidget *m_monthlyCalendar = nullptr;
    
    // Bottom section widgets
    QTableWidget *m_upcomingOrdersTable = nullptr;
    QCalendarWidget *m_twoWeekCalendar = nullptr;
    QTableWidget *m_currentInventoryTable = nullptr;
    
    QFrame* createSection(const QString &title, QWidget *content);
    QGroupBox* createGroupBox(const QString &title);
};

