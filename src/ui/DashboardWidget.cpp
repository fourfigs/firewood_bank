#include "DashboardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DashboardWidget::DashboardWidget(const UserInfo &userInfo, QWidget *parent)
    : QWidget(parent), m_userInfo(userInfo)
{
    setupUI();
    
    // Load data
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
    
    createTopSection();
    createBottomSection();
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
    
    // RIGHT: Monthly Calendar
    auto *calendarBox = createGroupBox("Monthly Overview");
    auto *calendarLayout = new QVBoxLayout(calendarBox);
    
    m_monthlyCalendar = new QCalendarWidget(this);
    m_monthlyCalendar->setGridVisible(true);
    m_monthlyCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    m_monthlyCalendar->setMaximumHeight(220);
    calendarLayout->addWidget(m_monthlyCalendar);
    
    topLayout->addWidget(calendarBox, 1);
    
    layout()->addItem(topLayout);
}

void DashboardWidget::createBottomSection()
{
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(15);
    
    // LEFT: Upcoming Orders
    auto *ordersBox = createGroupBox("📦 Upcoming Orders");
    auto *ordersLayout = new QVBoxLayout(ordersBox);
    
    m_upcomingOrdersTable = new QTableWidget(this);
    m_upcomingOrdersTable->setColumnCount(4);
    m_upcomingOrdersTable->setHorizontalHeaderLabels({"Date", "Household", "Cords", "Status"});
    m_upcomingOrdersTable->horizontalHeader()->setStretchLastSection(true);
    m_upcomingOrdersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_upcomingOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_upcomingOrdersTable->setAlternatingRowColors(true);
    m_upcomingOrdersTable->verticalHeader()->setVisible(false);
    ordersLayout->addWidget(m_upcomingOrdersTable);
    
    bottomLayout->addWidget(ordersBox, 1);
    
    // CENTER: 2-Week Calendar
    auto *twoWeekBox = createGroupBox("📅 Next 2 Weeks");
    auto *twoWeekLayout = new QVBoxLayout(twoWeekBox);
    
    m_twoWeekCalendar = new QCalendarWidget(this);
    m_twoWeekCalendar->setGridVisible(true);
    m_twoWeekCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    
    // Set date range to show next 2 weeks
    QDate today = QDate::currentDate();
    m_twoWeekCalendar->setSelectedDate(today);
    m_twoWeekCalendar->setMinimumDate(today);
    m_twoWeekCalendar->setMaximumDate(today.addDays(14));
    
    twoWeekLayout->addWidget(m_twoWeekCalendar);
    
    bottomLayout->addWidget(twoWeekBox, 1);
    
    // RIGHT: Current Inventory
    auto *inventoryBox = createGroupBox("📊 Current Inventory");
    auto *inventoryLayout = new QVBoxLayout(inventoryBox);
    
    m_currentInventoryTable = new QTableWidget(this);
    m_currentInventoryTable->setColumnCount(4);
    m_currentInventoryTable->setHorizontalHeaderLabels({"Species", "Form", "Cords", "Status"});
    m_currentInventoryTable->horizontalHeader()->setStretchLastSection(true);
    m_currentInventoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_currentInventoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_currentInventoryTable->setAlternatingRowColors(true);
    m_currentInventoryTable->verticalHeader()->setVisible(false);
    inventoryLayout->addWidget(m_currentInventoryTable);
    
    bottomLayout->addWidget(inventoryBox, 1);
    
    layout()->addItem(bottomLayout);
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

void DashboardWidget::loadUpcomingOrders()
{
    // TODO: Query real orders from database when orders system is implemented
    // For now, show placeholder message
    m_upcomingOrdersTable->setRowCount(1);
    
    auto *messageItem = new QTableWidgetItem("No upcoming orders scheduled");
    messageItem->setTextAlignment(Qt::AlignCenter);
    QFont font = messageItem->font();
    font.setItalic(true);
    messageItem->setFont(font);
    messageItem->setForeground(QBrush(Qt::gray));
    
    m_upcomingOrdersTable->setItem(0, 0, messageItem);
    m_upcomingOrdersTable->setSpan(0, 0, 1, 4);
    
    qDebug() << "Loaded upcoming orders (placeholder)";
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

