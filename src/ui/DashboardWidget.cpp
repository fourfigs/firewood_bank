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
    loadEmergencies();
    loadLowInventory();
    updateMonthlyCalendar();
}

void DashboardWidget::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Show statistics for leads and admins only
    if (Authorization::isAdmin(m_userInfo.role) || Authorization::isLead(m_userInfo.role)) {
        createStatisticsSection();
    }
    
    createTopSection();
    createBottomSection();
}

void DashboardWidget::createStatisticsSection()
{
    // Statistics banner at the top for leads and admins
    auto *statsBox = createGroupBox("📊 Organization Statistics Dashboard");
    statsBox->setStyleSheet(
        "QGroupBox { "
        "   border: 3px solid " + FirewoodStyles::EMBER_ORANGE + "; "
        "   border-radius: 15px; "
        "   margin-top: 22px; "
        "   padding: 25px 20px 20px 20px; "
        "   font-weight: bold; "
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fff9f5, stop:1 #ffede0); "
        "} "
        "QGroupBox::title { "
        "   subcontrol-origin: margin; "
        "   subcontrol-position: top center; "
        "   padding: 10px 24px; "
        "   background: " + FirewoodStyles::GRADIENT_EMBER_TO_FLAME + "; "
        "   color: white; "
        "   border-radius: 8px; "
        "   font-size: 13pt; "
        "   font-weight: bold; "
        "}"
    );
    auto *statsLayout = new QGridLayout(statsBox);
    statsLayout->setSpacing(18);
    statsLayout->setContentsMargins(10, 15, 10, 10);
    statsLayout->setColumnStretch(0, 1);
    statsLayout->setColumnStretch(1, 1);
    statsLayout->setColumnStretch(2, 1);
    statsLayout->setColumnStretch(3, 1);
    
        // === HOUSEHOLD COUNT ===
    auto *householdsLabel = new QLabel("<b style='font-size: 13pt; color: " + FirewoodStyles::EMBER_ORANGE + ";'>👥 HOUSEHOLDS SERVED</b>", this);
    statsLayout->addWidget(householdsLabel, 0, 0);
    
    m_totalHouseholdsLabel = new QLabel("Loading...", this);
    m_totalHouseholdsLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: " + FirewoodStyles::FOREST_GREEN + "; padding: 10px;");
    m_totalHouseholdsLabel->setAlignment(Qt::AlignCenter);
    statsLayout->addWidget(m_totalHouseholdsLabel, 1, 0);
    
    // === WOOD DELIVERED ===
    auto *woodLabel = new QLabel("<b style='font-size: 13pt; color: " + FirewoodStyles::EMBER_ORANGE + ";'>🪵 WOOD DELIVERED</b>", this);
    statsLayout->addWidget(woodLabel, 0, 1);
    
    auto *woodStatsLayout = new QVBoxLayout();
    woodStatsLayout->setSpacing(5);
    
    m_woodDeliveredWeekLabel = new QLabel("This Week: <b>0.0 cords</b>", this);
    m_woodDeliveredWeekLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    woodStatsLayout->addWidget(m_woodDeliveredWeekLabel);
    
    m_woodDeliveredMonthLabel = new QLabel("This Month: <b>0.0 cords</b>", this);
    m_woodDeliveredMonthLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    woodStatsLayout->addWidget(m_woodDeliveredMonthLabel);
    
    m_woodDeliveredYearLabel = new QLabel("This Year: <b>0.0 cords</b>", this);
    m_woodDeliveredYearLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    woodStatsLayout->addWidget(m_woodDeliveredYearLabel);
    
    m_woodDeliveredAllTimeLabel = new QLabel("All Time: <b>0.0 cords</b>", this);
    m_woodDeliveredAllTimeLabel->setStyleSheet("font-size: 13pt; font-weight: bold; color: " + FirewoodStyles::SUCCESS_GREEN + "; padding: 3px;");
    woodStatsLayout->addWidget(m_woodDeliveredAllTimeLabel);
    
    statsLayout->addLayout(woodStatsLayout, 1, 1);
    
    // === EXPENSES ===
    auto *expensesLabel = new QLabel("<b style='font-size: 13pt; color: " + FirewoodStyles::EMBER_ORANGE + ";'>💰 EXPENSES</b>", this);
    statsLayout->addWidget(expensesLabel, 0, 2);
    
    auto *expenseStatsLayout = new QVBoxLayout();
    expenseStatsLayout->setSpacing(5);
    
    m_expenseWeekLabel = new QLabel("This Week: <b>$0.00</b>", this);
    m_expenseWeekLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    expenseStatsLayout->addWidget(m_expenseWeekLabel);
    
    m_expenseMonthLabel = new QLabel("This Month: <b>$0.00</b>", this);
    m_expenseMonthLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    expenseStatsLayout->addWidget(m_expenseMonthLabel);
    
    m_expenseYearLabel = new QLabel("This Year: <b>$0.00</b>", this);
    m_expenseYearLabel->setStyleSheet("font-size: 11pt; padding: 3px;");
    expenseStatsLayout->addWidget(m_expenseYearLabel);
    
    m_expenseAllTimeLabel = new QLabel("All Time: <b>$0.00</b>", this);
    m_expenseAllTimeLabel->setStyleSheet("font-size: 13pt; font-weight: bold; color: " + FirewoodStyles::WARNING_AMBER + "; padding: 3px;");
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
        "  border: 2px solid " + FirewoodStyles::WARM_EARTH + "; "
        "  border-radius: 6px; "
        "  padding: 3px; "
        "} "
        "QCalendarWidget QWidget { "
        "  alternate-background-color: " + FirewoodStyles::CREAM + "; "
        "  background-color: white; "
        "} "
        "QCalendarWidget QAbstractItemView { "
        "  selection-background-color: " + FirewoodStyles::EMBER_ORANGE + "; "
        "  selection-color: white; "
        "  font-size: 9pt; "
        "  border-radius: 3px; "
        "} "
        "QCalendarWidget QAbstractItemView:enabled { "
        "  color: " + FirewoodStyles::CHARCOAL + "; "
        "} "
        "QCalendarWidget QToolButton { "
        "  color: white; "
        "  background-color: " + FirewoodStyles::FOREST_GREEN + "; "
        "  border-radius: 3px; "
        "  padding: 3px; "
        "  font-weight: bold; "
        "  font-size: 9pt; "
        "  min-width: 20px; "
        "  margin: 1px; "
        "} "
        "QCalendarWidget QToolButton:hover { "
        "  background-color: " + FirewoodStyles::EMBER_ORANGE + "; "
        "  color: white; "
        "} "
        "QCalendarWidget QToolButton:pressed { "
        "  background-color: " + FirewoodStyles::FLAME_RED + "; "
        "} "
        "QCalendarWidget QMenu { "
        "  background-color: white; "
        "  border: 1px solid " + FirewoodStyles::WARM_EARTH + "; "
        "} "
        "QCalendarWidget QSpinBox { "
        "  font-size: 9pt; "
        "  padding: 2px; "
        "} "
        "QCalendarWidget QWidget#qt_calendar_navigationbar { "
        "  background-color: " + FirewoodStyles::FOREST_GREEN + "; "
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
    ordersBox->setStyleSheet(FirewoodStyles::GROUP_BOX_FIRE);
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
    m_upcomingOrdersTable->setStyleSheet(FirewoodStyles::TABLE_VIEW);
    m_upcomingOrdersTable->setShowGrid(false);
    ordersScroll->setWidget(m_upcomingOrdersTable);
    ordersLayout->addWidget(ordersScroll);
    
    bottomLayout->addWidget(ordersBox, 1);
    
    // ==================== CENTER: 2-Week Calendar ====================
    auto *twoWeekBox = createGroupBox("📅 Next 2 Weeks");
    twoWeekBox->setStyleSheet(FirewoodStyles::GROUP_BOX);
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
        QString bgColor = (i % 2 == 0) ? "white" : FirewoodStyles::CREAM;
        
        // Day name label
        auto *dayLabel = new QLabel(date.toString("dddd"), this);
        dayLabel->setStyleSheet(
            "font-size: 12pt; "
            "font-weight: bold; "
            "color: " + FirewoodStyles::FOREST_GREEN + "; "
            "padding: 8px 10px 2px 10px; "
            "background: " + bgColor + ";"
        );
        calendarContentLayout->addWidget(dayLabel);
        
        // Full date label
        auto *dateLabel = new QLabel(date.toString("MMMM d, yyyy"), this);
        dateLabel->setStyleSheet(
            "font-size: 10pt; "
            "color: " + FirewoodStyles::CHARCOAL + "; "
            "padding: 2px 10px 8px 10px; "
            "background: " + bgColor + ";"
        );
        calendarContentLayout->addWidget(dateLabel);
        
        // Separator (only between rows, not after last row)
        if (i < 13) {
            auto *separator = new QFrame(this);
            separator->setFrameShape(QFrame::HLine);
            separator->setFrameShadow(QFrame::Sunken);
            separator->setStyleSheet("background-color: " + FirewoodStyles::WARM_EARTH + "; min-height: 1px; max-height: 1px;");
            calendarContentLayout->addWidget(separator);
        }
    }
    
    calendarContentLayout->addStretch();
    calendarScroll->setWidget(calendarWidget);
    twoWeekLayout->addWidget(calendarScroll);
    
    bottomLayout->addWidget(twoWeekBox, 1);
    
    // ==================== RIGHT: Visual Inventory Display ====================
    auto *inventoryBox = createGroupBox("📦 Inventory At-A-Glance");
    inventoryBox->setStyleSheet(FirewoodStyles::GROUP_BOX_SUCCESS);
    auto *inventoryLayout = new QVBoxLayout(inventoryBox);
    inventoryLayout->setSpacing(8);
    inventoryLayout->setContentsMargins(12, 12, 12, 12);
    
    // Wood Section
    auto *woodLabel = new QLabel("<b style='font-size: 10pt; color: " + FirewoodStyles::EMBER_ORANGE + ";'>🪵 WOOD</b>", this);
    inventoryLayout->addWidget(woodLabel);
    
    auto *woodGrid = new QGridLayout();
    woodGrid->setSpacing(6);
    woodGrid->setContentsMargins(0, 2, 0, 2);
    
    // Wood - Split
    auto *splitLabel = new QLabel("<b>Split:</b>", this);
    splitLabel->setStyleSheet("font-size: 9pt;");
    auto *splitValue = new QLabel("12.5 cords", this);
    splitValue->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::SUCCESS_GREEN + ";");
    woodGrid->addWidget(splitLabel, 0, 0);
    woodGrid->addWidget(splitValue, 0, 1);
    
    // Wood - Not Split
    auto *unsplitLabel = new QLabel("<b>Rounds:</b>", this);
    unsplitLabel->setStyleSheet("font-size: 9pt;");
    auto *unsplitValue = new QLabel("8.0 cords", this);
    unsplitValue->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::WARNING_AMBER + ";");
    woodGrid->addWidget(unsplitLabel, 1, 0);
    woodGrid->addWidget(unsplitValue, 1, 1);
    
    inventoryLayout->addLayout(woodGrid);
    inventoryLayout->addWidget(createSeparator());
    
    // Gas Section
    auto *gasLabel = new QLabel("<b style='font-size: 10pt; color: " + FirewoodStyles::EMBER_ORANGE + ";'>⛽ GAS</b>", this);
    inventoryLayout->addWidget(gasLabel);
    
    auto *gasGrid = new QGridLayout();
    gasGrid->setSpacing(6);
    gasGrid->setContentsMargins(0, 2, 0, 2);
    
    auto *regularGasLabel = new QLabel("<b>Regular:</b>", this);
    regularGasLabel->setStyleSheet("font-size: 9pt;");
    auto *regularGasValue = new QLabel("5 gal", this);
    regularGasValue->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::SUCCESS_GREEN + ";");
    gasGrid->addWidget(regularGasLabel, 0, 0);
    gasGrid->addWidget(regularGasValue, 0, 1);
    
    auto *mixedGasLabel = new QLabel("<b>Mixed:</b>", this);
    mixedGasLabel->setStyleSheet("font-size: 9pt;");
    auto *mixedGasValue = new QLabel("3 gal", this);
    mixedGasValue->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::WARNING_AMBER + ";");
    gasGrid->addWidget(mixedGasLabel, 1, 0);
    gasGrid->addWidget(mixedGasValue, 1, 1);
    
    inventoryLayout->addLayout(gasGrid);
    inventoryLayout->addWidget(createSeparator());
    
    // Saws Section
    auto *sawsLabel = new QLabel("<b style='font-size: 10pt; color: " + FirewoodStyles::EMBER_ORANGE + ";'>🪚 SAWS</b>", this);
    inventoryLayout->addWidget(sawsLabel);
    
    auto *sawsValue = new QLabel("6 operational", this);
    sawsValue->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::SUCCESS_GREEN + "; padding-left: 15px;");
    inventoryLayout->addWidget(sawsValue);
    
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
    sep->setStyleSheet("background-color: " + FirewoodStyles::ASH_GRAY + "; min-height: 1px; max-height: 1px; margin: 5px 0;");
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
    
    // === TOTAL HOUSEHOLDS ===
    QSqlQuery householdQuery;
    if (householdQuery.exec("SELECT COUNT(*) FROM households WHERE active = 1")) {
        if (householdQuery.next()) {
            int totalHouseholds = householdQuery.value(0).toInt();
            m_totalHouseholdsLabel->setText(QString::number(totalHouseholds));
        }
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
    if (!query.exec("SELECT o.id, o.order_date, h.name, h.phone_number, o.requested_cords, o.status "
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

