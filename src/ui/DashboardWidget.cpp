#include "DashboardWidget.h"
#include "StyleSheet.h"
#include "Authorization.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QScrollArea>
#include <QDebug>

using namespace firewood::core;

DashboardWidget::DashboardWidget(const UserInfo &userInfo, QWidget *parent)
    : QWidget(parent), m_userInfo(userInfo)
{
    setupUI();
    
    // Load data
    if (Authorization::isAdmin(m_userInfo.role) || Authorization::isLead(m_userInfo.role)) {
        loadStatistics();  // NEW: Load statistics for leads and admins
    }
    loadUpcomingOrders();
    loadCurrentInventory();
    loadInventoryAtAGlance();  // Load real inventory data
    loadEmergencies();
    loadLowInventory();
    updateMonthlyCalendar();
}

void DashboardWidget::setupUI()
{
    // Set modern dark background
    setStyleSheet("QWidget { background-color: " + AdobeStyles::DARK_BG + "; }");
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    
    // Create modern header
    createModernHeader();
    
    // Show statistics for leads and admins only
    if (Authorization::isAdmin(m_userInfo.role) || Authorization::isLead(m_userInfo.role)) {
        createStatisticsSection();
    }
    
    // Create modern dashboard grid
    createModernDashboard();
}

void DashboardWidget::createModernHeader()
{
    auto *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(
        "QWidget { "
        "   background: " + AdobeStyles::CARD_BG + "; "
        "   border-radius: 12px; "
        "   padding: 20px; "
        "}"
    );
    
    auto *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setSpacing(20);
    
    // User info section
    auto *userInfoWidget = new QWidget();
    auto *userInfoLayout = new QVBoxLayout(userInfoWidget);
    userInfoLayout->setSpacing(8);
    
    auto *welcomeLabel = new QLabel("Welcome back, " + m_userInfo.fullName, this);
    welcomeLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 24px; "
        "   font-weight: 700; "
        "   color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "   margin-bottom: 4px; "
        "}"
    );
    userInfoLayout->addWidget(welcomeLabel);
    
    auto *roleLabel = new QLabel(m_userInfo.role + " • " + QDate::currentDate().toString("dddd, MMMM d, yyyy"), this);
    roleLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 14px; "
        "   color: " + AdobeStyles::TEXT_SECONDARY + "; "
        "}"
    );
    userInfoLayout->addWidget(roleLabel);
    
    headerLayout->addWidget(userInfoWidget);
    headerLayout->addStretch();
    
    // Quick stats for all users
    auto *quickStatsWidget = new QWidget();
    auto *quickStatsLayout = new QHBoxLayout(quickStatsWidget);
    quickStatsLayout->setSpacing(16);
    
    // Today's date card
    auto *dateCard = createQuickStatCard(AdobeStyles::ICON_CALENDAR, "Today", QDate::currentDate().toString("MMM d"));
    quickStatsLayout->addWidget(dateCard);
    
    // Time card
    auto *timeCard = createQuickStatCard("🕐", "Time", QTime::currentTime().toString("h:mm AP"));
    quickStatsLayout->addWidget(timeCard);
    
    headerLayout->addWidget(quickStatsWidget);
    
    layout()->addWidget(headerWidget);
}

QWidget* DashboardWidget::createQuickStatCard(const QString &icon, const QString &label, const QString &value)
{
    auto *card = new QWidget();
    card->setStyleSheet(
        "QWidget { "
        "   background: " + AdobeStyles::SURFACE_BG + "; "
        "   border-radius: 8px; "
        "   padding: 12px 16px; "
        "   border: 1px solid " + AdobeStyles::BORDER_GRAY + "; "
        "}"
    );
    
    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(4);
    
    auto *iconLabel = new QLabel(icon, this);
    iconLabel->setStyleSheet("font-size: 16px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);
    
    auto *valueLabel = new QLabel(value, this);
    valueLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 16px; "
        "   font-weight: 600; "
        "   color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "}"
    );
    valueLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(valueLabel);
    
    auto *labelLabel = new QLabel(label, this);
    labelLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 12px; "
        "   color: " + AdobeStyles::TEXT_SECONDARY + "; "
        "}"
    );
    labelLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(labelLabel);
    
    return card;
}

void DashboardWidget::createModernDashboard()
{
    auto *dashboardWidget = new QWidget();
    auto *dashboardLayout = new QGridLayout(dashboardWidget);
    dashboardLayout->setSpacing(20);
    dashboardLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create modern cards in a grid layout
    // Row 1: Work Orders and Calendar
    auto *workOrdersCard = createModernCard("📋 Work Orders", "Upcoming deliveries and tasks", createWorkOrdersContent());
    dashboardLayout->addWidget(workOrdersCard, 0, 0);
    
    auto *calendarCard = createModernCard("📅 Schedule", "Today's schedule and upcoming events", createCalendarContent());
    dashboardLayout->addWidget(calendarCard, 0, 1);
    
    // Row 2: Inventory and Alerts
    auto *inventoryCard = createModernCard("📦 Inventory", "Current stock levels", createInventoryContent());
    dashboardLayout->addWidget(inventoryCard, 1, 0);
    
    auto *alertsCard = createModernCard("⚠️ Alerts", "Important notifications", createAlertsContent());
    dashboardLayout->addWidget(alertsCard, 1, 1);
    
    layout()->addWidget(dashboardWidget);
}

QWidget* DashboardWidget::createModernCard(const QString &title, const QString &subtitle, QWidget *content)
{
    auto *card = new QWidget();
    card->setStyleSheet(
        "QWidget { "
        "   background: " + AdobeStyles::CARD_BG + "; "
        "   border-radius: 12px; "
        "   border: 1px solid " + AdobeStyles::BORDER_GRAY + "; "
        "}"
    );
    
    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(16);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // Header
    auto *headerWidget = new QWidget();
    auto *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setSpacing(4);
    
    auto *titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 18px; "
        "   font-weight: 600; "
        "   color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "}"
    );
    headerLayout->addWidget(titleLabel);
    
    auto *subtitleLabel = new QLabel(subtitle, this);
    subtitleLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 14px; "
        "   color: " + AdobeStyles::TEXT_SECONDARY + "; "
        "}"
    );
    headerLayout->addWidget(subtitleLabel);
    
    layout->addWidget(headerWidget);
    layout->addWidget(content);
    
    return card;
}

QWidget* DashboardWidget::createWorkOrdersContent()
{
    auto *content = new QWidget();
    auto *layout = new QVBoxLayout(content);
    layout->setSpacing(12);
    
    // Create a modern table for work orders
    m_upcomingOrdersTable = new QTableWidget(this);
    m_upcomingOrdersTable->setColumnCount(3);
    m_upcomingOrdersTable->setHorizontalHeaderLabels({"Date", "Client", "Status"});
    m_upcomingOrdersTable->horizontalHeader()->setStretchLastSection(true);
    m_upcomingOrdersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_upcomingOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_upcomingOrdersTable->setAlternatingRowColors(true);
    m_upcomingOrdersTable->verticalHeader()->setVisible(false);
    m_upcomingOrdersTable->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_upcomingOrdersTable->setShowGrid(false);
    m_upcomingOrdersTable->setMaximumHeight(200);
    
    layout->addWidget(m_upcomingOrdersTable);
    
    return content;
}

QWidget* DashboardWidget::createCalendarContent()
{
    auto *content = new QWidget();
    auto *layout = new QVBoxLayout(content);
    layout->setSpacing(12);
    
    // Create a modern calendar widget
    m_twoWeekCalendar = new QCalendarWidget(this);
    m_twoWeekCalendar->setStyleSheet(
        "QCalendarWidget { "
        "   background: " + AdobeStyles::SURFACE_BG + "; "
        "   border: 1px solid " + AdobeStyles::BORDER_GRAY + "; "
        "   border-radius: 8px; "
        "   color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "} "
        "QCalendarWidget QAbstractItemView { "
        "   selection-background-color: " + AdobeStyles::ADOBE_BLUE + "; "
        "   selection-color: white; "
        "} "
        "QCalendarWidget QToolButton { "
        "   background: " + AdobeStyles::ADOBE_BLUE + "; "
        "   color: white; "
        "   border-radius: 4px; "
        "   padding: 4px; "
        "} "
        "QCalendarWidget QToolButton:hover { "
        "   background: " + AdobeStyles::ADOBE_BLUE_LIGHT + "; "
        "}"
    );
    m_twoWeekCalendar->setMaximumHeight(200);
    
    layout->addWidget(m_twoWeekCalendar);
    
    return content;
}

QWidget* DashboardWidget::createInventoryContent()
{
    auto *content = new QWidget();
    auto *layout = new QVBoxLayout(content);
    layout->setSpacing(12);
    
    // Create modern inventory display
    auto *inventoryGrid = new QGridLayout();
    inventoryGrid->setSpacing(12);
    
    // Wood section
    auto *woodLabel = new QLabel("🪵 Wood", this);
    woodLabel->setStyleSheet("font-weight: 600; color: " + AdobeStyles::TEXT_PRIMARY + ";");
    inventoryGrid->addWidget(woodLabel, 0, 0);
    
    m_splitWoodLabel = new QLabel("Loading...", this);
    m_splitWoodLabel->setStyleSheet("color: " + AdobeStyles::SUCCESS_GREEN + "; font-weight: 600;");
    inventoryGrid->addWidget(m_splitWoodLabel, 0, 1);
    
    // Gas section
    auto *gasLabel = new QLabel("⛽ Gas", this);
    gasLabel->setStyleSheet("font-weight: 600; color: " + AdobeStyles::TEXT_PRIMARY + ";");
    inventoryGrid->addWidget(gasLabel, 1, 0);
    
    m_regularGasLabel = new QLabel("Loading...", this);
    m_regularGasLabel->setStyleSheet("color: " + AdobeStyles::SUCCESS_GREEN + "; font-weight: 600;");
    inventoryGrid->addWidget(m_regularGasLabel, 1, 1);
    
    // Equipment section
    auto *equipmentLabel = new QLabel("🔧 Equipment", this);
    equipmentLabel->setStyleSheet("font-weight: 600; color: " + AdobeStyles::TEXT_PRIMARY + ";");
    inventoryGrid->addWidget(equipmentLabel, 2, 0);
    
    m_sawsLabel = new QLabel("Loading...", this);
    m_sawsLabel->setStyleSheet("color: " + AdobeStyles::SUCCESS_GREEN + "; font-weight: 600;");
    inventoryGrid->addWidget(m_sawsLabel, 2, 1);
    
    layout->addLayout(inventoryGrid);
    
    return content;
}

QWidget* DashboardWidget::createAlertsContent()
{
    auto *content = new QWidget();
    auto *layout = new QVBoxLayout(content);
    layout->setSpacing(12);
    
    // Create alerts display
    m_emergenciesText = new QTextEdit(this);
    m_emergenciesText->setReadOnly(true);
    m_emergenciesText->setMaximumHeight(80);
    m_emergenciesText->setStyleSheet(
        "QTextEdit { "
        "   background: " + AdobeStyles::SURFACE_BG + "; "
        "   border: 1px solid " + AdobeStyles::BORDER_GRAY + "; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "}"
    );
    layout->addWidget(m_emergenciesText);
    
    m_lowInventoryText = new QTextEdit(this);
    m_lowInventoryText->setReadOnly(true);
    m_lowInventoryText->setMaximumHeight(80);
    m_lowInventoryText->setStyleSheet(
        "QTextEdit { "
        "   background: " + AdobeStyles::SURFACE_BG + "; "
        "   border: 1px solid " + AdobeStyles::BORDER_GRAY + "; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "}"
    );
    layout->addWidget(m_lowInventoryText);
    
    return content;
}

void DashboardWidget::createStatisticsSection()
{
    // Statistics banner at the top for leads and admins
    auto *statsBox = createGroupBox("📊 Organization Statistics Dashboard");
    statsBox->setStyleSheet(
        "QGroupBox { "
        "   border: 3px solid " + AdobeStyles::ADOBE_BLUE + "; "
        "   border-radius: 15px; "
        "   margin-top: 18px; "
        "   padding: 15px 20px 15px 20px; "
        "   font-weight: bold; "
        "   background: " + AdobeStyles::CARD_BG + "; "
        "} "
        "QGroupBox::title { "
        "   subcontrol-origin: margin; "
        "   subcontrol-position: top center; "
        "   padding: 8px 20px; "
        "   background: " + AdobeStyles::GRADIENT_PRIMARY + "; "
        "   color: white; "
        "   border-radius: 8px; "
        "   font-size: 13pt; "
        "   font-weight: bold; "
        "}"
    );
    auto *statsLayout = new QGridLayout(statsBox);
    statsLayout->setSpacing(15);
    statsLayout->setContentsMargins(10, 10, 10, 10);
    // Adjust column stretch to prevent text cut-off
    statsLayout->setColumnStretch(0, 1);  // Households
    statsLayout->setColumnStretch(1, 2);  // Wood Delivered (needs more space)
    statsLayout->setColumnStretch(2, 2);  // Expenses (needs more space)
    statsLayout->setColumnStretch(3, 1);  // Calendar (fixed width)
    
        // === HOUSEHOLD COUNT ===
    auto *householdsLabel = new QLabel("<b style='font-size: 13pt; color: " + AdobeStyles::ADOBE_BLUE + ";'>👥 HOUSEHOLDS SERVED</b>", this);
    statsLayout->addWidget(householdsLabel, 0, 0);
    
    m_totalHouseholdsLabel = new QLabel("Loading...", this);
    m_totalHouseholdsLabel->setStyleSheet("font-size: 20pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + "; padding: 10px 5px 15px 5px;");
    m_totalHouseholdsLabel->setAlignment(Qt::AlignCenter);
    m_totalHouseholdsLabel->setMinimumHeight(60);
    statsLayout->addWidget(m_totalHouseholdsLabel, 1, 0);
    
    // === WOOD DELIVERED ===
    auto *woodLabel = new QLabel("<b style='font-size: 13pt; color: " + AdobeStyles::ADOBE_BLUE + ";'>🪵 WOOD DELIVERED</b>", this);
    woodLabel->setWordWrap(false);
    woodLabel->setMinimumWidth(150);
    statsLayout->addWidget(woodLabel, 0, 1);
    
    auto *woodStatsLayout = new QVBoxLayout();
    woodStatsLayout->setSpacing(5);
    
    m_woodDeliveredWeekLabel = new QLabel("This Week: <b>0.0 cords</b>", this);
    m_woodDeliveredWeekLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    m_woodDeliveredWeekLabel->setWordWrap(false);
    woodStatsLayout->addWidget(m_woodDeliveredWeekLabel);
    
    m_woodDeliveredMonthLabel = new QLabel("This Month: <b>0.0 cords</b>", this);
    m_woodDeliveredMonthLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    m_woodDeliveredMonthLabel->setWordWrap(false);
    woodStatsLayout->addWidget(m_woodDeliveredMonthLabel);
    
    m_woodDeliveredYearLabel = new QLabel("This Year: <b>0.0 cords</b>", this);
    m_woodDeliveredYearLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    m_woodDeliveredYearLabel->setWordWrap(false);
    woodStatsLayout->addWidget(m_woodDeliveredYearLabel);
    
    m_woodDeliveredAllTimeLabel = new QLabel("All Time: <b>0.0 cords</b>", this);
    m_woodDeliveredAllTimeLabel->setStyleSheet("font-size: 13pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + "; padding: 3px;");
    m_woodDeliveredAllTimeLabel->setWordWrap(false);
    woodStatsLayout->addWidget(m_woodDeliveredAllTimeLabel);
    
    statsLayout->addLayout(woodStatsLayout, 1, 1);
    
    // === EXPENSES ===
    auto *expensesLabel = new QLabel("<b style='font-size: 13pt; color: " + AdobeStyles::ADOBE_BLUE + ";'>💰 EXPENSES</b>", this);
    expensesLabel->setWordWrap(false);
    expensesLabel->setMinimumWidth(120);
    statsLayout->addWidget(expensesLabel, 0, 2);
    
    auto *expenseStatsLayout = new QVBoxLayout();
    expenseStatsLayout->setSpacing(5);
    
    m_expenseWeekLabel = new QLabel("This Week: <b>$0.00</b>", this);
    m_expenseWeekLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    m_expenseWeekLabel->setWordWrap(false);
    expenseStatsLayout->addWidget(m_expenseWeekLabel);
    
    m_expenseMonthLabel = new QLabel("This Month: <b>$0.00</b>", this);
    m_expenseMonthLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    m_expenseMonthLabel->setWordWrap(false);
    expenseStatsLayout->addWidget(m_expenseMonthLabel);
    
    m_expenseYearLabel = new QLabel("This Year: <b>$0.00</b>", this);
    m_expenseYearLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    m_expenseYearLabel->setWordWrap(false);
    expenseStatsLayout->addWidget(m_expenseYearLabel);
    
    m_expenseAllTimeLabel = new QLabel("All Time: <b>$0.00</b>", this);
    m_expenseAllTimeLabel->setStyleSheet("font-size: 13pt; font-weight: bold; color: " + AdobeStyles::WARNING_ORANGE + "; padding: 3px;");
    m_expenseAllTimeLabel->setWordWrap(false);
    expenseStatsLayout->addWidget(m_expenseAllTimeLabel);
    
    statsLayout->addLayout(expenseStatsLayout, 1, 2);
    
    // === CALENDAR (far right) ===
    m_monthlyCalendar = new QCalendarWidget(this);
    m_monthlyCalendar->setGridVisible(true);
    
    // Set proper sizing for the calendar to show all content
    m_monthlyCalendar->setMinimumWidth(240);
    m_monthlyCalendar->setMaximumWidth(260);
    m_monthlyCalendar->setMinimumHeight(190);
    m_monthlyCalendar->setMaximumHeight(210);
    
    m_monthlyCalendar->setStyleSheet(
        "QCalendarWidget { "
        "  background-color: white; "
        "  border: 2px solid " + AdobeStyles::BORDER_GRAY + "; "
        "  border-radius: 6px; "
        "  padding: 3px; "
        "} "
        "QCalendarWidget QWidget { "
        "  alternate-background-color: " + AdobeStyles::SURFACE_BG + "; "
        "  background-color: white; "
        "} "
        "QCalendarWidget QAbstractItemView { "
        "  selection-background-color: " + AdobeStyles::ADOBE_BLUE + "; "
        "  selection-color: white; "
        "  font-size: 9pt; "
        "  border-radius: 3px; "
        "} "
        "QCalendarWidget QAbstractItemView:enabled { "
        "  color: " + AdobeStyles::TEXT_PRIMARY + "; "
        "} "
        "QCalendarWidget QToolButton { "
        "  color: white; "
        "  background-color: " + AdobeStyles::ADOBE_BLUE + "; "
        "  border-radius: 3px; "
        "  padding: 3px; "
        "  font-weight: bold; "
        "  font-size: 9pt; "
        "  min-width: 20px; "
        "  margin: 1px; "
        "} "
        "QCalendarWidget QToolButton:hover { "
        "  background-color: " + AdobeStyles::ADOBE_BLUE_LIGHT + "; "
        "  color: white; "
        "} "
        "QCalendarWidget QToolButton:pressed { "
        "  background-color: " + AdobeStyles::ADOBE_BLUE_DARK + "; "
        "} "
        "QCalendarWidget QMenu { "
        "  background-color: white; "
        "  border: 1px solid " + AdobeStyles::BORDER_GRAY + "; "
        "} "
        "QCalendarWidget QSpinBox { "
        "  font-size: 9pt; "
        "  padding: 2px; "
        "} "
        "QCalendarWidget QWidget#qt_calendar_navigationbar { "
        "  background-color: " + AdobeStyles::ADOBE_BLUE + "; "
        "  border-radius: 4px; "
        "  padding: 1px; "
        "} "
        "QCalendarWidget QAbstractItemView:disabled { "
        "  color: #cccccc; "
        "}"
    );
    
    // Add calendar to the statistics section, spanning both rows
    statsLayout->addWidget(m_monthlyCalendar, 0, 3, 2, 1, Qt::AlignTop | Qt::AlignRight);
    
    layout()->addWidget(statsBox);
}

void DashboardWidget::createTopSection()
{
    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(15);
    
    // LEFT: User Information
    auto *userInfoBox = createGroupBox("User Information");
    auto *userInfoLayout = new QVBoxLayout(userInfoBox);
    userInfoLayout->setSpacing(8);
    
    m_userNameLabel = new QLabel(QString("<b>Name:</b> %1").arg(m_userInfo.fullName), this);
    m_userNameLabel->setStyleSheet("font-size: 14px;");
    userInfoLayout->addWidget(m_userNameLabel);
    
    m_userContactLabel = new QLabel(QString("<b>Contact:</b> %1").arg(
        m_userInfo.contactNumber.isEmpty() ? "N/A" : m_userInfo.contactNumber), this);
    m_userContactLabel->setStyleSheet("font-size: 14px;");
    userInfoLayout->addWidget(m_userContactLabel);
    
    m_userRoleLabel = new QLabel(QString("<b>Position:</b> %1").arg(
        m_userInfo.role.isEmpty() ? "N/A" : m_userInfo.role), this);
    m_userRoleLabel->setStyleSheet("font-size: 14px; padding-bottom: 10px;");
    userInfoLayout->addWidget(m_userRoleLabel);
    
    userInfoLayout->addStretch();
    topLayout->addWidget(userInfoBox, 1);
    
    // CENTER: Alerts (Emergencies and Low Inventory)
    auto *alertsBox = createGroupBox("Alerts & Notifications");
    auto *alertsLayout = new QVBoxLayout(alertsBox);
    alertsLayout->setSpacing(10);
    
    auto *emergencyLabel = new QLabel("<b style='color: red;'>🚨 EMERGENCIES</b>", this);
    emergencyLabel->setStyleSheet("font-size: 14px;");
    alertsLayout->addWidget(emergencyLabel);
    
    m_emergenciesText = new QTextEdit(this);
    m_emergenciesText->setReadOnly(true);
    m_emergenciesText->setMaximumHeight(80);
    m_emergenciesText->setStyleSheet(
        "QTextEdit { background-color: #fff3cd; border: 1px solid #ffc107; padding: 5px; }"
    );
    alertsLayout->addWidget(m_emergenciesText);
    
    auto *lowInventoryLabel = new QLabel("<b>⚠️ Low Inventory</b>", this);
    lowInventoryLabel->setStyleSheet("font-size: 14px; margin-top: 5px;");
    alertsLayout->addWidget(lowInventoryLabel);
    
    m_lowInventoryText = new QTextEdit(this);
    m_lowInventoryText->setReadOnly(true);
    m_lowInventoryText->setMaximumHeight(80);
    m_lowInventoryText->setStyleSheet(
        "QTextEdit { background-color: #f8d7da; border: 1px solid #dc3545; padding: 5px; }"
    );
    alertsLayout->addWidget(m_lowInventoryText);
    
    topLayout->addWidget(alertsBox, 2);
    
    layout()->addItem(topLayout);
}

void DashboardWidget::createBottomSection()
{
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(15);
    
    // ==================== LEFT: Upcoming Work Orders List ====================
    auto *ordersBox = createGroupBox("📋 Upcoming Work Orders");
    ordersBox->setStyleSheet(AdobeStyles::GROUP_BOX_FIRE);
    auto *ordersLayout = new QVBoxLayout(ordersBox);
    
    // Simple scrollable list
    auto *ordersScroll = new QScrollArea(this);
    ordersScroll->setWidgetResizable(true);
    ordersScroll->setFrameShape(QFrame::NoFrame);
    
    m_upcomingOrdersTable = new QTableWidget(this);
    m_upcomingOrdersTable->setColumnCount(3);
    m_upcomingOrdersTable->setHorizontalHeaderLabels({"Date Received", "Client Name", "Contact #"});
    m_upcomingOrdersTable->horizontalHeader()->setStretchLastSection(true);
    m_upcomingOrdersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_upcomingOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_upcomingOrdersTable->setAlternatingRowColors(true);
    m_upcomingOrdersTable->verticalHeader()->setVisible(false);
    m_upcomingOrdersTable->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_upcomingOrdersTable->setShowGrid(false);
    ordersScroll->setWidget(m_upcomingOrdersTable);
    ordersLayout->addWidget(ordersScroll);
    
    bottomLayout->addWidget(ordersBox, 1);
    
    // ==================== CENTER: 2-Week Calendar ====================
    auto *twoWeekBox = createGroupBox("📅 Next 2 Weeks");
    twoWeekBox->setStyleSheet(AdobeStyles::GROUP_BOX);
    auto *twoWeekLayout = new QVBoxLayout(twoWeekBox);
    
    // Custom 2-week calendar display
    auto *calendarScroll = new QScrollArea(this);
    calendarScroll->setWidgetResizable(true);
    calendarScroll->setFrameShape(QFrame::NoFrame);
    
    m_twoWeekCalendar = new QCalendarWidget(this);  // Will be replaced with custom widget
    auto *calendarWidget = new QWidget(this);
    auto *calendarContentLayout = new QVBoxLayout(calendarWidget);
    calendarContentLayout->setSpacing(0);
    
    // Create 14-day list with zebra striping
    QDate startDate = QDate::currentDate();
    for (int i = 0; i < 14; i++) {
        QDate date = startDate.addDays(i);
        
        // Zebra striping - alternating row colors
        QString bgColor = (i % 2 == 0) ? AdobeStyles::CARD_BG : AdobeStyles::SURFACE_BG;
        
        // Day name label
        auto *dayLabel = new QLabel(date.toString("dddd"), this);
        dayLabel->setStyleSheet(
            "font-size: 12pt; "
            "font-weight: bold; "
            "color: " + AdobeStyles::ADOBE_BLUE + "; "
            "padding: 8px 10px 2px 10px; "
            "background: " + bgColor + ";"
        );
        calendarContentLayout->addWidget(dayLabel);
        
        // Full date label
        auto *dateLabel = new QLabel(date.toString("MMMM d, yyyy"), this);
        dateLabel->setStyleSheet(
            "font-size: 10pt; "
            "color: " + AdobeStyles::TEXT_PRIMARY + "; "
            "padding: 2px 10px 8px 10px; "
            "background: " + bgColor + ";"
        );
        calendarContentLayout->addWidget(dateLabel);
        
        // Separator (only between rows, not after last row)
        if (i < 13) {
            auto *separator = new QFrame(this);
            separator->setFrameShape(QFrame::HLine);
            separator->setFrameShadow(QFrame::Sunken);
            separator->setStyleSheet("background-color: " + AdobeStyles::BORDER_GRAY + "; min-height: 1px; max-height: 1px;");
            calendarContentLayout->addWidget(separator);
        }
    }
    
    calendarContentLayout->addStretch();
    calendarScroll->setWidget(calendarWidget);
    twoWeekLayout->addWidget(calendarScroll);
    
    bottomLayout->addWidget(twoWeekBox, 1);
    
    // ==================== RIGHT: Visual Inventory Display ====================
    auto *inventoryBox = createGroupBox("📦 Inventory At-A-Glance");
    inventoryBox->setStyleSheet(AdobeStyles::GROUP_BOX_SUCCESS);
    auto *inventoryLayout = new QVBoxLayout(inventoryBox);
    inventoryLayout->setSpacing(8);
    inventoryLayout->setContentsMargins(15, 18, 15, 15);
    
    // Wood Section
    auto *woodLabel = new QLabel("<b style='font-size: 9pt; color: " + AdobeStyles::ADOBE_BLUE + ";'>🪵 WOOD</b>", this);
    inventoryLayout->addWidget(woodLabel);
    inventoryLayout->addSpacing(4);
    
    auto *woodGrid = new QGridLayout();
    woodGrid->setSpacing(6);
    woodGrid->setContentsMargins(10, 0, 10, 0);
    
    // Wood - Split
    auto *splitLabel = new QLabel("<b>Split:</b>", this);
    splitLabel->setStyleSheet("font-size: 8pt;");
    splitLabel->setMinimumWidth(65);
    splitLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_splitWoodLabel = new QLabel("Loading...", this);
    m_splitWoodLabel->setTextFormat(Qt::RichText);
    m_splitWoodLabel->setStyleSheet("font-size: 9pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + ";");
    m_splitWoodLabel->setMinimumWidth(140);
    m_splitWoodLabel->setWordWrap(true);
    m_splitWoodLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_splitWoodLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    woodGrid->addWidget(splitLabel, 0, 0);
    woodGrid->addWidget(m_splitWoodLabel, 0, 1);
    
    // Wood - Not Split
    auto *unsplitLabel = new QLabel("<b>Rounds:</b>", this);
    unsplitLabel->setStyleSheet("font-size: 8pt;");
    unsplitLabel->setMinimumWidth(65);
    unsplitLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_roundsWoodLabel = new QLabel("Loading...", this);
    m_roundsWoodLabel->setTextFormat(Qt::RichText);
    m_roundsWoodLabel->setStyleSheet("font-size: 9pt; font-weight: bold; color: " + AdobeStyles::WARNING_ORANGE + ";");
    m_roundsWoodLabel->setMinimumWidth(140);
    m_roundsWoodLabel->setWordWrap(false);
    m_roundsWoodLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_roundsWoodLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    woodGrid->addWidget(unsplitLabel, 1, 0);
    woodGrid->addWidget(m_roundsWoodLabel, 1, 1);
    
    // Set column stretch to prevent label compression
    woodGrid->setColumnStretch(0, 0);  // Fixed width for labels
    woodGrid->setColumnStretch(1, 1);  // Expandable for values
    
    inventoryLayout->addLayout(woodGrid);
    inventoryLayout->addSpacing(8);
    inventoryLayout->addWidget(createSeparator());
    inventoryLayout->addSpacing(8);
    
    // Gas Section
    auto *gasLabel = new QLabel("<b style='font-size: 9pt; color: " + AdobeStyles::ADOBE_BLUE + ";'>⛽ GAS</b>", this);
    inventoryLayout->addWidget(gasLabel);
    inventoryLayout->addSpacing(4);
    
    auto *gasGrid = new QGridLayout();
    gasGrid->setSpacing(6);
    gasGrid->setContentsMargins(10, 0, 10, 0);
    
    auto *regularGasLabel = new QLabel("<b>Regular:</b>", this);
    regularGasLabel->setStyleSheet("font-size: 8pt;");
    regularGasLabel->setMinimumWidth(65);
    regularGasLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_regularGasLabel = new QLabel("Loading...", this);
    m_regularGasLabel->setStyleSheet("font-size: 9pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + ";");
    m_regularGasLabel->setMinimumWidth(100);
    m_regularGasLabel->setWordWrap(false);
    m_regularGasLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_regularGasLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    gasGrid->addWidget(regularGasLabel, 0, 0);
    gasGrid->addWidget(m_regularGasLabel, 0, 1);
    
    auto *mixedGasLabel = new QLabel("<b>Mixed:</b>", this);
    mixedGasLabel->setStyleSheet("font-size: 8pt;");
    mixedGasLabel->setMinimumWidth(65);
    mixedGasLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_mixedGasLabel = new QLabel("Loading...", this);
    m_mixedGasLabel->setStyleSheet("font-size: 9pt; font-weight: bold; color: " + AdobeStyles::WARNING_ORANGE + ";");
    m_mixedGasLabel->setMinimumWidth(100);
    m_mixedGasLabel->setWordWrap(false);
    m_mixedGasLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_mixedGasLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    gasGrid->addWidget(mixedGasLabel, 1, 0);
    gasGrid->addWidget(m_mixedGasLabel, 1, 1);
    
    // Set column stretch to prevent label compression
    gasGrid->setColumnStretch(0, 0);  // Fixed width for labels
    gasGrid->setColumnStretch(1, 1);  // Expandable for values
    
    inventoryLayout->addLayout(gasGrid);
    inventoryLayout->addSpacing(8);
    inventoryLayout->addWidget(createSeparator());
    inventoryLayout->addSpacing(8);
    
    // Saws Section
    auto *sawsLabel = new QLabel("<b style='font-size: 9pt; color: " + AdobeStyles::ADOBE_BLUE + ";'>🪚 SAWS</b>", this);
    inventoryLayout->addWidget(sawsLabel);
    inventoryLayout->addSpacing(4);
    
    m_sawsLabel = new QLabel("Loading...", this);
    m_sawsLabel->setStyleSheet("font-size: 9pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + "; padding-left: 10px;");
    m_sawsLabel->setMinimumWidth(140);
    m_sawsLabel->setWordWrap(false);
    m_sawsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    inventoryLayout->addWidget(m_sawsLabel);
    
    inventoryLayout->addStretch();
    
    m_currentInventoryTable = new QTableWidget(this);  // Keep for compatibility
    m_currentInventoryTable->hide();  // Hide the old table
    
    bottomLayout->addWidget(inventoryBox, 1);
    
    layout()->addItem(bottomLayout);
}

QFrame* DashboardWidget::createSeparator()
{
    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    sep->setStyleSheet("background-color: " + AdobeStyles::BORDER_GRAY + "; min-height: 1px; max-height: 1px; margin: 5px 0;");
    return sep;
}

QGroupBox* DashboardWidget::createGroupBox(const QString &title)
{
    auto *groupBox = new QGroupBox(title, this);
    groupBox->setStyleSheet(
        "QGroupBox {"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    border: 2px solid #0078d4;"
        "    border-radius: 5px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0 5px;"
        "    color: #0078d4;"
        "}"
    );
    return groupBox;
}

void DashboardWidget::loadStatistics()
{
    // Calculate date ranges
    QDate today = QDate::currentDate();
    QDate weekStart = today.addDays(-(today.dayOfWeek() - 1));  // Monday of this week
    QDate monthStart = QDate(today.year(), today.month(), 1);
    QDate yearStart = QDate(today.year(), 1, 1);
    
    // === TOTAL HOUSEHOLDS SERVED ===
    // Count distinct households that have received completed deliveries
    QSqlQuery householdQuery;
    if (householdQuery.exec("SELECT COUNT(DISTINCT household_id) FROM orders WHERE status = 'Completed'")) {
        if (householdQuery.next()) {
            int totalHouseholds = householdQuery.value(0).toInt();
            m_totalHouseholdsLabel->setText(QString::number(totalHouseholds));
        }
    } else {
        qDebug() << "Error loading households served:" << householdQuery.lastError().text();
        m_totalHouseholdsLabel->setText("0");
    }
    
    // === WOOD DELIVERED ===
    // Week
    QSqlQuery woodWeekQuery;
    woodWeekQuery.prepare("SELECT COALESCE(SUM(delivered_cords), 0) FROM orders WHERE status = 'Completed' AND delivery_date >= ?");
    woodWeekQuery.addBindValue(weekStart.toString(Qt::ISODate));
    if (woodWeekQuery.exec() && woodWeekQuery.next()) {
        double cords = woodWeekQuery.value(0).toDouble();
        m_woodDeliveredWeekLabel->setText(QString("This Week: <b>%1 cords</b>").arg(cords, 0, 'f', 1));
    }
    
    // Month
    QSqlQuery woodMonthQuery;
    woodMonthQuery.prepare("SELECT COALESCE(SUM(delivered_cords), 0) FROM orders WHERE status = 'Completed' AND delivery_date >= ?");
    woodMonthQuery.addBindValue(monthStart.toString(Qt::ISODate));
    if (woodMonthQuery.exec() && woodMonthQuery.next()) {
        double cords = woodMonthQuery.value(0).toDouble();
        m_woodDeliveredMonthLabel->setText(QString("This Month: <b>%1 cords</b>").arg(cords, 0, 'f', 1));
    }
    
    // Year
    QSqlQuery woodYearQuery;
    woodYearQuery.prepare("SELECT COALESCE(SUM(delivered_cords), 0) FROM orders WHERE status = 'Completed' AND delivery_date >= ?");
    woodYearQuery.addBindValue(yearStart.toString(Qt::ISODate));
    if (woodYearQuery.exec() && woodYearQuery.next()) {
        double cords = woodYearQuery.value(0).toDouble();
        m_woodDeliveredYearLabel->setText(QString("This Year: <b>%1 cords</b>").arg(cords, 0, 'f', 1));
    }
    
    // All Time
    QSqlQuery woodAllQuery;
    if (woodAllQuery.exec("SELECT COALESCE(SUM(delivered_cords), 0) FROM orders WHERE status = 'Completed'") && woodAllQuery.next()) {
        double cords = woodAllQuery.value(0).toDouble();
        m_woodDeliveredAllTimeLabel->setText(QString("All Time: <b>%1 cords</b>").arg(cords, 0, 'f', 1));
    }
    
    // === EXPENSES (based on amount_paid) ===
    // Week
    QSqlQuery expWeekQuery;
    expWeekQuery.prepare("SELECT COALESCE(SUM(amount_paid), 0) FROM orders WHERE status = 'Completed' AND delivery_date >= ?");
    expWeekQuery.addBindValue(weekStart.toString(Qt::ISODate));
    if (expWeekQuery.exec() && expWeekQuery.next()) {
        double amount = expWeekQuery.value(0).toDouble();
        m_expenseWeekLabel->setText(QString("This Week: <b>$%1</b>").arg(amount, 0, 'f', 2));
    }
    
    // Month
    QSqlQuery expMonthQuery;
    expMonthQuery.prepare("SELECT COALESCE(SUM(amount_paid), 0) FROM orders WHERE status = 'Completed' AND delivery_date >= ?");
    expMonthQuery.addBindValue(monthStart.toString(Qt::ISODate));
    if (expMonthQuery.exec() && expMonthQuery.next()) {
        double amount = expMonthQuery.value(0).toDouble();
        m_expenseMonthLabel->setText(QString("This Month: <b>$%1</b>").arg(amount, 0, 'f', 2));
    }
    
    // Year
    QSqlQuery expYearQuery;
    expYearQuery.prepare("SELECT COALESCE(SUM(amount_paid), 0) FROM orders WHERE status = 'Completed' AND delivery_date >= ?");
    expYearQuery.addBindValue(yearStart.toString(Qt::ISODate));
    if (expYearQuery.exec() && expYearQuery.next()) {
        double amount = expYearQuery.value(0).toDouble();
        m_expenseYearLabel->setText(QString("This Year: <b>$%1</b>").arg(amount, 0, 'f', 2));
    }
    
    // All Time
    QSqlQuery expAllQuery;
    if (expAllQuery.exec("SELECT COALESCE(SUM(amount_paid), 0) FROM orders WHERE status = 'Completed'") && expAllQuery.next()) {
        double amount = expAllQuery.value(0).toDouble();
        m_expenseAllTimeLabel->setText(QString("All Time: <b>$%1</b>").arg(amount, 0, 'f', 2));
    }
    
    qDebug() << "Statistics loaded successfully for leads/admins";
}

void DashboardWidget::loadUpcomingOrders()
{
    // Load actual orders from database
    QSqlQuery query;
    if (!query.exec("SELECT o.id, o.order_date, h.name, h.phone, o.requested_cords, o.status "
                   "FROM orders o "
                   "JOIN households h ON o.household_id = h.id "
                   "WHERE o.status IN ('Pending', 'Scheduled', 'In Progress') "
                   "ORDER BY o.delivery_date, o.order_date "
                   "LIMIT 10")) {
        qDebug() << "ERROR: Failed to load upcoming orders:" << query.lastError().text();
        m_upcomingOrdersTable->setRowCount(1);
        auto *messageItem = new QTableWidgetItem("Error loading orders");
        m_upcomingOrdersTable->setItem(0, 0, messageItem);
        m_upcomingOrdersTable->setSpan(0, 0, 1, 3);
        return;
    }
    
    m_upcomingOrdersTable->setRowCount(0);
    int row = 0;
    
    while (query.next()) {
        m_upcomingOrdersTable->insertRow(row);
        
        QString orderDate = query.value(1).toString();
        QString clientName = query.value(2).toString();
        QString contactNumber = query.value(3).toString();
        
        // Parse and format date
        QDate date = QDate::fromString(orderDate, Qt::ISODate);
        QString formattedDate = date.isValid() ? date.toString("MMM d, yyyy") : orderDate;
        
        m_upcomingOrdersTable->setItem(row, 0, new QTableWidgetItem(formattedDate));
        m_upcomingOrdersTable->setItem(row, 1, new QTableWidgetItem(clientName));
        m_upcomingOrdersTable->setItem(row, 2, new QTableWidgetItem(contactNumber.isEmpty() ? "N/A" : contactNumber));
        
        row++;
    }
    
    if (row == 0) {
        m_upcomingOrdersTable->setRowCount(1);
        auto *messageItem = new QTableWidgetItem("No upcoming orders scheduled");
        messageItem->setTextAlignment(Qt::AlignCenter);
        QFont font = messageItem->font();
        font.setItalic(true);
        messageItem->setFont(font);
        messageItem->setForeground(QBrush(Qt::gray));
        m_upcomingOrdersTable->setItem(0, 0, messageItem);
        m_upcomingOrdersTable->setSpan(0, 0, 1, 3);
    }
    
    qDebug() << "Loaded" << row << "upcoming orders";
}

void DashboardWidget::loadCurrentInventory()
{
    QSqlQuery query;
    if (!query.exec("SELECT species, form, volume_cords, status FROM inventory ORDER BY species")) {
        qDebug() << "ERROR: Failed to load inventory:" << query.lastError().text();
        return;
    }
    
    m_currentInventoryTable->setRowCount(0);
    int row = 0;
    
    while (query.next()) {
        m_currentInventoryTable->insertRow(row);
        m_currentInventoryTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        m_currentInventoryTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        m_currentInventoryTable->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        m_currentInventoryTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        row++;
    }
    
    if (row == 0) {
        m_currentInventoryTable->setRowCount(1);
        auto *messageItem = new QTableWidgetItem("No inventory items");
        messageItem->setTextAlignment(Qt::AlignCenter);
        QFont font = messageItem->font();
        font.setItalic(true);
        messageItem->setFont(font);
        messageItem->setForeground(QBrush(Qt::gray));
        m_currentInventoryTable->setItem(0, 0, messageItem);
        m_currentInventoryTable->setSpan(0, 0, 1, 4);
    }
    
    qDebug() << "Loaded" << row << "inventory items";
}

void DashboardWidget::loadEmergencies()
{
    // TODO: Query emergencies from database when implemented
    // For now, show placeholder or empty
    m_emergenciesText->setPlainText("No current emergencies");
    m_emergenciesText->setStyleSheet(
        "QTextEdit { background-color: #d4edda; border: 1px solid #28a745; padding: 5px; color: #155724; }"
    );
    
    qDebug() << "Loaded emergencies (placeholder)";
}

void DashboardWidget::loadLowInventory()
{
    // TODO: Query inventory below threshold when inventory system is fully implemented
    // For now, show placeholder
    m_lowInventoryText->setPlainText("All inventory levels adequate");
    m_lowInventoryText->setStyleSheet(
        "QTextEdit { background-color: #d4edda; border: 1px solid #28a745; padding: 5px; color: #155724; }"
    );
    
    qDebug() << "Loaded low inventory alerts (placeholder)";
}

void DashboardWidget::updateMonthlyCalendar()
{
    // TODO: Highlight dates with scheduled orders/deliveries
    // For now, just highlight today
    QDate today = QDate::currentDate();
    
    QTextCharFormat format;
    format.setBackground(QBrush(QColor(0, 120, 212, 100))); // Light blue
    format.setForeground(QBrush(Qt::white));
    format.setFontWeight(QFont::Bold);
    
    m_monthlyCalendar->setDateTextFormat(today, format);
    
    qDebug() << "Updated monthly calendar"; 
}

void DashboardWidget::refreshData()
{
    qDebug() << "Refreshing all dashboard data...";
    
    // Reload all data
    if (Authorization::isAdmin(m_userInfo.role) || Authorization::isLead(m_userInfo.role)) {
        loadStatistics();
    }
    loadUpcomingOrders();
    loadCurrentInventory();
    loadInventoryAtAGlance();
    loadEmergencies();
    loadLowInventory();
    updateMonthlyCalendar();
    checkInventoryAlerts();
    
    qDebug() << "Dashboard data refreshed!";
}

void DashboardWidget::loadInventoryAtAGlance()
{
    qDebug() << "Loading inventory at-a-glance...";
    
    // Query for Split Wood
    QSqlQuery splitQuery;
    splitQuery.prepare("SELECT COALESCE(SUM(quantity), 0) FROM inventory_items WHERE item_name LIKE '%Split%' OR item_name LIKE '%split%'");
    double splitCords = 0.0;
    if (splitQuery.exec() && splitQuery.next()) {
        splitCords = splitQuery.value(0).toDouble();
    } else {
        qDebug() << "Error loading split wood:" << splitQuery.lastError().text();
    }
    
    // Query for Rounds/Unsplit Wood
    QSqlQuery roundsQuery;
    roundsQuery.prepare("SELECT COALESCE(SUM(quantity), 0) FROM inventory_items WHERE item_name LIKE '%Round%' OR item_name LIKE '%Unsplit%'");
    double roundsCords = 0.0;
    if (roundsQuery.exec() && roundsQuery.next()) {
        roundsCords = roundsQuery.value(0).toDouble();
    } else {
        qDebug() << "Error loading rounds:" << roundsQuery.lastError().text();
    }
    
    // Query for Regular Gas
    QSqlQuery regularGasQuery;
    regularGasQuery.prepare("SELECT COALESCE(SUM(quantity), 0) FROM inventory_items WHERE (item_name LIKE '%Gasoline%' OR item_name LIKE '%gasoline%' OR item_name LIKE '%Unmixed Gas%') AND item_name NOT LIKE '%Mix%' AND item_name NOT LIKE '%2-Cycle%'");
    double regularGas = 0.0;
    if (regularGasQuery.exec() && regularGasQuery.next()) {
        regularGas = regularGasQuery.value(0).toDouble();
    } else {
        qDebug() << "Error loading regular gas:" << regularGasQuery.lastError().text();
    }
    
    // Query for Mixed/2-Cycle Gas
    QSqlQuery mixedGasQuery;
    mixedGasQuery.prepare("SELECT COALESCE(SUM(quantity), 0) FROM inventory_items WHERE item_name LIKE '%Mix%' OR item_name LIKE '%2-Cycle%' OR item_name LIKE '%Mixed Gas%'");
    double mixedGas = 0.0;
    if (mixedGasQuery.exec() && mixedGasQuery.next()) {
        mixedGas = mixedGasQuery.value(0).toDouble();
    } else {
        qDebug() << "Error loading mixed gas:" << mixedGasQuery.lastError().text();
    }
    
    // Query for operational Chainsaws
    QSqlQuery sawsQuery;
    sawsQuery.prepare("SELECT COALESCE(SUM(quantity), 0) FROM inventory_items WHERE item_name LIKE '%Chainsaw%' AND item_name NOT LIKE '%Chain%'");
    int saws = 0;
    if (sawsQuery.exec() && sawsQuery.next()) {
        saws = sawsQuery.value(0).toInt();
    } else {
        qDebug() << "Error loading chainsaws:" << sawsQuery.lastError().text();
    }
    
    // Compute tentative split wood after fulfilling all open orders
    double openRequested = 0.0;
    {
        QSqlQuery openQ;
        openQ.prepare("SELECT COALESCE(SUM(requested_cords - delivered_cords), 0) FROM orders WHERE status IN ('Pending','Scheduled','In Progress')");
        if (openQ.exec() && openQ.next()) {
            openRequested = openQ.value(0).toDouble();
        } else {
            qDebug() << "Error loading open order requests:" << openQ.lastError().text();
        }
    }
    double tentativeSplit = splitCords - openRequested;
    if (tentativeSplit < 0) tentativeSplit = 0.0;

    // Update labels
    if (m_splitWoodLabel) {
        m_splitWoodLabel->setText(
            QString("<span style='color:%1'>%2 cords</span> <span style='color:%3'>(true)</span>"
                    " &nbsp; | &nbsp; "
                    "<span style='color:%4'>%5</span> <span style='color:%3'>tentative</span>")
                .arg(AdobeStyles::SUCCESS_GREEN)
                .arg(splitCords, 0, 'f', 1)
                .arg(AdobeStyles::TEXT_SECONDARY)
                .arg(AdobeStyles::ADOBE_BLUE)
                .arg(tentativeSplit, 0, 'f', 1));
    }
    if (m_roundsWoodLabel) {
        m_roundsWoodLabel->setText(QString("%1 cords").arg(roundsCords, 0, 'f', 1));
    }
    if (m_regularGasLabel) {
        m_regularGasLabel->setText(QString("%1 gal").arg(regularGas, 0, 'f', 1));
    }
    if (m_mixedGasLabel) {
        m_mixedGasLabel->setText(QString("%1 gal").arg(mixedGas, 0, 'f', 1));
    }
    if (m_sawsLabel) {
        m_sawsLabel->setText(QString("%1 operational").arg(saws));
    }
    
    qDebug() << "Inventory loaded: Split=" << splitCords << ", Rounds=" << roundsCords 
             << ", RegGas=" << regularGas << ", MixGas=" << mixedGas << ", Saws=" << saws;
}

void DashboardWidget::checkInventoryAlerts()
{
    qDebug() << "Checking inventory alerts...";
    
    // Remove existing alerts widget if it exists
    if (m_inventoryAlertsWidget) {
        m_inventoryAlertsWidget->deleteLater();
        m_inventoryAlertsWidget = nullptr;
    }
    
    // Query for items that are low or critical
    QSqlQuery query;
    query.exec("SELECT item_name, quantity, unit, reorder_level, emergency_level "
               "FROM inventory_items "
               "WHERE (reorder_level > 0 AND quantity <= reorder_level) "
               "OR (emergency_level > 0 AND quantity <= emergency_level) "
               "ORDER BY quantity ASC");
    
    QStringList alerts;
    QStringList criticalAlerts;
    
    while (query.next()) {
        QString itemName = query.value(0).toString();
        double quantity = query.value(1).toDouble();
        QString unit = query.value(2).toString();
        double reorderLevel = query.value(3).toDouble();
        double emergencyLevel = query.value(4).toDouble();
        
        QString alertText = QString("%1: %2 %3 remaining").arg(itemName).arg(quantity).arg(unit);
        
        if (emergencyLevel > 0 && quantity <= emergencyLevel) {
            criticalAlerts << QString("🚨 CRITICAL: %1").arg(alertText);
        } else if (reorderLevel > 0 && quantity <= reorderLevel) {
            alerts << QString("⚠️ LOW: %1").arg(alertText);
        }
    }
    
    // Only create alerts widget if there are alerts
    if (!alerts.isEmpty() || !criticalAlerts.isEmpty()) {
        m_inventoryAlertsWidget = new QWidget(this);
        auto *alertLayout = new QVBoxLayout(m_inventoryAlertsWidget);
        
        // Style the alert widget
        m_inventoryAlertsWidget->setStyleSheet(
            "QWidget { "
            "  background-color: #ffebe0; "
            "  border: 2px solid #ff6b35; "
            "  border-radius: 8px; "
            "  padding: 10px; "
            "  margin: 5px; "
            "}"
        );
        
        // Add title
        auto *titleLabel = new QLabel("⚠️ INVENTORY ALERTS", m_inventoryAlertsWidget);
        titleLabel->setStyleSheet("font-weight: bold; color: #ff6b35; font-size: 12pt;");
        titleLabel->setAlignment(Qt::AlignCenter);
        alertLayout->addWidget(titleLabel);
        
        // Add critical alerts first
        for (const QString &alert : criticalAlerts) {
            auto *alertLabel = new QLabel(alert, m_inventoryAlertsWidget);
            alertLabel->setStyleSheet("color: #c1421e; font-weight: bold;");
            alertLayout->addWidget(alertLabel);
        }
        
        // Add regular alerts
        for (const QString &alert : alerts) {
            auto *alertLabel = new QLabel(alert, m_inventoryAlertsWidget);
            alertLabel->setStyleSheet("color: #d97732;");
            alertLayout->addWidget(alertLabel);
        }
        
        // Add the alerts widget to the main layout
        // Find the main layout and insert the alerts at the top
        QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
        if (mainLayout) {
            mainLayout->insertWidget(1, m_inventoryAlertsWidget); // Insert after header
        }
        
        qDebug() << "Created inventory alerts widget with" << (alerts.size() + criticalAlerts.size()) << "alerts";
    }
}

