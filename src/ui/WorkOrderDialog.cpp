#include "WorkOrderDialog.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDateTime>

WorkOrderDialog::WorkOrderDialog(int orderId, QWidget *parent)
    : QDialog(parent), m_orderId(orderId)
{
    setWindowTitle(orderId < 0 ? "New Work Order" : "Edit Work Order");
    resize(700, 600);
    
    setupUI();
    
    if (m_orderId >= 0) {
        loadOrder();
    }
}

void WorkOrderDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    
    // ==================== ORGANIZATION HEADER ====================
    auto *orgHeader = new QLabel(this);
    orgHeader->setText(
        "<div style='text-align: center;'>"
        "<div style='font-size: 16pt; font-weight: bold; margin-bottom: 4px;'>🌲 Northern Mendocino Ecosystem Recovery Alliance 🔥</div>"
        "<div style='font-size: 12pt; margin-bottom: 2px;'>Community Firewood Bank</div>"
        "<div style='font-size: 9pt;'>📧 info@nm-era.org  |  📞 (707) 555-WOOD</div>"
        "</div>"
    );
    orgHeader->setAlignment(Qt::AlignCenter);
    orgHeader->setStyleSheet(
        "background: " + FirewoodStyles::GRADIENT_EMBER_TO_FLAME + "; "
        "color: white; "
        "border-radius: 8px; "
        "padding: 16px; "
        "margin-bottom: 10px;"
    );
    mainLayout->addWidget(orgHeader);
    
    // Order Number and Date Header
    auto *orderHeaderLayout = new QHBoxLayout();
    QString orderNumText = m_orderId >= 0 ? QString("Order #%1").arg(m_orderId) : "New Order";
    auto *orderNumLabel = new QLabel("<b>" + orderNumText + "</b>", this);
    orderNumLabel->setStyleSheet("font-size: 11pt; color: " + FirewoodStyles::FOREST_GREEN + ";");
    auto *dateLabel = new QLabel("<b>Date:</b> " + QDate::currentDate().toString("MMMM dd, yyyy"), this);
    dateLabel->setStyleSheet("font-size: 11pt; color: " + FirewoodStyles::CHARCOAL + ";");
    orderHeaderLayout->addWidget(orderNumLabel);
    orderHeaderLayout->addStretch();
    orderHeaderLayout->addWidget(dateLabel);
    mainLayout->addLayout(orderHeaderLayout);
    
    // Order Information Group
    auto *orderGroup = new QGroupBox("Order Information", this);
    orderGroup->setProperty("class", "fire");
    orderGroup->setStyleSheet(FirewoodStyles::GROUP_BOX_FIRE);
    auto *orderLayout = new QFormLayout(orderGroup);
    
    m_clientCombo = new QComboBox(this);
    loadClients();
    orderLayout->addRow("Client:", m_clientCombo);
    
    m_orderDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_orderDateEdit->setCalendarPopup(true);
    orderLayout->addRow("Order Date:", m_orderDateEdit);
    
    m_requestedCordsEdit = new QDoubleSpinBox(this);
    m_requestedCordsEdit->setRange(0.0, 999.0);
    m_requestedCordsEdit->setSingleStep(0.5);
    m_requestedCordsEdit->setSuffix(" cords");
    orderLayout->addRow("Requested Cords:", m_requestedCordsEdit);
    
    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItems({"Pending", "Scheduled", "In Progress", "Completed", "Cancelled"});
    connect(m_statusCombo, &QComboBox::currentTextChanged, this, &WorkOrderDialog::onStatusChanged);
    orderLayout->addRow("Status:", m_statusCombo);
    
    m_priorityCombo = new QComboBox(this);
    m_priorityCombo->addItems({"Low", "Normal", "High", "Emergency"});
    m_priorityCombo->setCurrentText("Normal");
    orderLayout->addRow("Priority:", m_priorityCombo);
    
    mainLayout->addWidget(orderGroup);
    
    // Delivery Information Group
    auto *deliveryGroup = new QGroupBox("🚚 Delivery Information", this);
    deliveryGroup->setStyleSheet(FirewoodStyles::GROUP_BOX);
    auto *deliveryLayout = new QFormLayout(deliveryGroup);
    
    m_deliveryDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_deliveryDateEdit->setCalendarPopup(true);
    deliveryLayout->addRow("Delivery Date:", m_deliveryDateEdit);
    
    m_deliveredCordsEdit = new QDoubleSpinBox(this);
    m_deliveredCordsEdit->setRange(0.0, 999.0);
    m_deliveredCordsEdit->setSingleStep(0.5);
    m_deliveredCordsEdit->setSuffix(" cords");
    deliveryLayout->addRow("Delivered Cords:", m_deliveredCordsEdit);
    
    m_deliveryAddressEdit = new QLineEdit(this);
    deliveryLayout->addRow("Delivery Address:", m_deliveryAddressEdit);
    
    m_deliveryNotesEdit = new QTextEdit(this);
    m_deliveryNotesEdit->setMaximumHeight(60);
    deliveryLayout->addRow("Delivery Notes:", m_deliveryNotesEdit);
    
    m_assignedDriverEdit = new QLineEdit(this);
    deliveryLayout->addRow("Assigned Driver:", m_assignedDriverEdit);
    
    // Delivery tracking fields (NEW!)
    m_deliveryTimeEdit = new QLineEdit(this);
    m_deliveryTimeEdit->setPlaceholderText("e.g., 9:00 AM");
    deliveryLayout->addRow("Departure Time:", m_deliveryTimeEdit);
    
    m_startMileageEdit = new QDoubleSpinBox(this);
    m_startMileageEdit->setRange(0.0, 999999.0);
    m_startMileageEdit->setDecimals(1);
    m_startMileageEdit->setSuffix(" mi");
    m_startMileageEdit->setPrefix("Start: ");
    deliveryLayout->addRow("Starting Odometer:", m_startMileageEdit);
    
    m_endMileageEdit = new QDoubleSpinBox(this);
    m_endMileageEdit->setRange(0.0, 999999.0);
    m_endMileageEdit->setDecimals(1);
    m_endMileageEdit->setSuffix(" mi");
    m_endMileageEdit->setPrefix("End: ");
    m_endMileageEdit->setEnabled(false);  // Only enabled when completing delivery
    deliveryLayout->addRow("Ending Odometer:", m_endMileageEdit);
    
    mainLayout->addWidget(deliveryGroup);
    
    // Payment Information Group
    auto *paymentGroup = new QGroupBox("💳 Payment Information", this);
    paymentGroup->setStyleSheet(FirewoodStyles::GROUP_BOX_SUCCESS);
    auto *paymentLayout = new QFormLayout(paymentGroup);
    
    m_paymentMethodCombo = new QComboBox(this);
    m_paymentMethodCombo->addItems({"Cash", "Check", "Credit Card", "Work-for-Wood", "Voucher", "Free"});
    paymentLayout->addRow("Payment Method:", m_paymentMethodCombo);
    
    m_amountPaidEdit = new QDoubleSpinBox(this);
    m_amountPaidEdit->setRange(0.0, 99999.0);
    m_amountPaidEdit->setPrefix("$");
    m_amountPaidEdit->setSingleStep(1.0);
    paymentLayout->addRow("Amount Paid:", m_amountPaidEdit);
    
    mainLayout->addWidget(paymentGroup);
    
    // Notes
    auto *notesGroup = new QGroupBox("📝 Additional Notes", this);
    notesGroup->setStyleSheet(FirewoodStyles::GROUP_BOX);
    auto *notesLayout = new QVBoxLayout(notesGroup);
    
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMaximumHeight(80);
    notesLayout->addWidget(m_notesEdit);
    
    mainLayout->addWidget(notesGroup);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *saveButton = new QPushButton("💾 Save Order", this);
    saveButton->setProperty("class", "primary");
    saveButton->setStyleSheet(FirewoodStyles::PRIMARY_BUTTON);
    saveButton->setDefault(true);
    connect(saveButton, &QPushButton::clicked, this, &WorkOrderDialog::saveOrder);
    buttonLayout->addWidget(saveButton);
    
    auto *cancelButton = new QPushButton("❌ Cancel", this);
    cancelButton->setProperty("class", "cancel");
    cancelButton->setStyleSheet(FirewoodStyles::CANCEL_BUTTON);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void WorkOrderDialog::onStatusChanged(const QString &status)
{
    // Enable end mileage field only when completing delivery
    if (status == "Completed") {
        m_endMileageEdit->setEnabled(true);
        m_endMileageEdit->setStyleSheet("background-color: " + FirewoodStyles::GOLDEN_GLOW + ";");
        
        // Auto-calculate delivered cords if not set (1/3 cord per delivery)
        if (m_deliveredCordsEdit->value() == 0.0) {
            m_deliveredCordsEdit->setValue(0.33);
        }
    } else {
        m_endMileageEdit->setEnabled(false);
        m_endMileageEdit->setStyleSheet("");
    }
    
    // Show visual indicator for in-progress deliveries
    if (status == "In Progress") {
        m_startMileageEdit->setStyleSheet("background-color: " + FirewoodStyles::GOLDEN_GLOW + ";");
        m_deliveryTimeEdit->setStyleSheet("background-color: " + FirewoodStyles::GOLDEN_GLOW + ";");
    } else {
        m_startMileageEdit->setStyleSheet("");
        m_deliveryTimeEdit->setStyleSheet("");
    }
}

void WorkOrderDialog::loadClients()
{
    QSqlQuery query("SELECT id, name FROM households ORDER BY name");
    
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        m_clientCombo->addItem(name, id);
    }
    
    if (m_clientCombo->count() == 0) {
        QMessageBox::warning(this, "No Clients", 
                           "No clients found in database. Please add a client first.");
    }
}

void WorkOrderDialog::loadOrder()
{
    QSqlQuery query;
    query.prepare("SELECT household_id, order_date, requested_cords, delivered_cords, "
                 "status, priority, delivery_date, delivery_address, delivery_notes, "
                 "assigned_driver, payment_method, amount_paid, notes, "
                 "delivery_time, start_mileage, end_mileage "
                 "FROM orders WHERE id = :id");
    query.bindValue(":id", m_orderId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load order: " + query.lastError().text());
        return;
    }
    
    // Set household
    int householdId = query.value(0).toInt();
    int index = m_clientCombo->findData(householdId);
    if (index >= 0) {
        m_clientCombo->setCurrentIndex(index);
    }
    
    // Set order date
    QString orderDateStr = query.value(1).toString();
    QDate orderDate = QDate::fromString(orderDateStr, Qt::ISODate);
    if (orderDate.isValid()) {
        m_orderDateEdit->setDate(orderDate);
    }
    
    // Set cords
    m_requestedCordsEdit->setValue(query.value(2).toDouble());
    m_deliveredCordsEdit->setValue(query.value(3).toDouble());
    
    // Set status and priority
    m_statusCombo->setCurrentText(query.value(4).toString());
    m_priorityCombo->setCurrentText(query.value(5).toString());
    
    // Set delivery date
    QString deliveryDateStr = query.value(6).toString();
    if (!deliveryDateStr.isEmpty()) {
        QDate deliveryDate = QDate::fromString(deliveryDateStr, Qt::ISODate);
        if (deliveryDate.isValid()) {
            m_deliveryDateEdit->setDate(deliveryDate);
        }
    }
    
    // Set delivery details
    m_deliveryAddressEdit->setText(query.value(7).toString());
    m_deliveryNotesEdit->setPlainText(query.value(8).toString());
    m_assignedDriverEdit->setText(query.value(9).toString());
    
    // Set payment details
    m_paymentMethodCombo->setCurrentText(query.value(10).toString());
    m_amountPaidEdit->setValue(query.value(11).toDouble());
    
    // Set notes
    m_notesEdit->setPlainText(query.value(12).toString());
    
    // Set delivery tracking fields (NEW!)
    m_deliveryTimeEdit->setText(query.value(13).toString());
    m_startMileageEdit->setValue(query.value(14).toDouble());
    m_endMileageEdit->setValue(query.value(15).toDouble());
}

void WorkOrderDialog::saveOrder()
{
    // Validation
    if (m_clientCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a client.");
        m_clientCombo->setFocus();
        return;
    }
    
    if (m_requestedCordsEdit->value() <= 0.0) {
        QMessageBox::warning(this, "Validation Error", "Requested cords must be greater than 0.");
        m_requestedCordsEdit->setFocus();
        return;
    }
    
    int householdId = m_clientCombo->currentData().toInt();
    QString newStatus = m_statusCombo->currentText();
    
    // Check if order is being marked as completed
    bool isBeingCompleted = false;
    if (m_orderId >= 0 && newStatus == "Completed") {
        // Check if it wasn't completed before
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT status FROM orders WHERE id = ?");
        checkQuery.addBindValue(m_orderId);
        if (checkQuery.exec() && checkQuery.next()) {
            QString oldStatus = checkQuery.value(0).toString();
            isBeingCompleted = (oldStatus != "Completed");
        }
    }
    
    QSqlQuery query;
    
    if (m_orderId < 0) {
        // Create new order
        query.prepare("INSERT INTO orders (household_id, order_date, requested_cords, delivered_cords, "
                     "status, priority, delivery_date, delivery_address, delivery_notes, "
                     "assigned_driver, payment_method, amount_paid, notes, "
                     "delivery_time, start_mileage, end_mileage, created_at, updated_at) "
                     "VALUES (:household_id, :order_date, :requested_cords, :delivered_cords, "
                     ":status, :priority, :delivery_date, :delivery_address, :delivery_notes, "
                     ":assigned_driver, :payment_method, :amount_paid, :notes, "
                     ":delivery_time, :start_mileage, :end_mileage, :created_at, :updated_at)");
        
        query.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));
    } else {
        // Update existing order
        query.prepare("UPDATE orders SET household_id = :household_id, order_date = :order_date, "
                     "requested_cords = :requested_cords, delivered_cords = :delivered_cords, "
                     "status = :status, priority = :priority, delivery_date = :delivery_date, "
                     "delivery_address = :delivery_address, delivery_notes = :delivery_notes, "
                     "assigned_driver = :assigned_driver, payment_method = :payment_method, "
                     "amount_paid = :amount_paid, notes = :notes, "
                     "delivery_time = :delivery_time, start_mileage = :start_mileage, end_mileage = :end_mileage, "
                     "completed_date = :completed_date, updated_at = :updated_at "
                     "WHERE id = :id");
        
        query.bindValue(":id", m_orderId);
        query.bindValue(":completed_date", (newStatus == "Completed") ? QDate::currentDate().toString(Qt::ISODate) : QVariant());
    }
    
    // Bind common values
    query.bindValue(":household_id", householdId);
    query.bindValue(":order_date", m_orderDateEdit->date().toString(Qt::ISODate));
    query.bindValue(":requested_cords", m_requestedCordsEdit->value());
    query.bindValue(":delivered_cords", m_deliveredCordsEdit->value());
    query.bindValue(":status", newStatus);
    query.bindValue(":priority", m_priorityCombo->currentText());
    query.bindValue(":delivery_date", m_deliveryDateEdit->date().toString(Qt::ISODate));
    query.bindValue(":delivery_address", m_deliveryAddressEdit->text());
    query.bindValue(":delivery_notes", m_deliveryNotesEdit->toPlainText());
    query.bindValue(":assigned_driver", m_assignedDriverEdit->text());
    query.bindValue(":payment_method", m_paymentMethodCombo->currentText());
    query.bindValue(":amount_paid", m_amountPaidEdit->value());
    query.bindValue(":notes", m_notesEdit->toPlainText());
    query.bindValue(":delivery_time", m_deliveryTimeEdit->text());
    query.bindValue(":start_mileage", m_startMileageEdit->value());
    query.bindValue(":end_mileage", m_endMileageEdit->value());
    query.bindValue(":updated_at", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", 
                            "Failed to save order: " + query.lastError().text());
        qDebug() << "SQL Error:" << query.lastError().text();
        return;
    }
    
    // Get the order ID for new orders
    int orderId = m_orderId;
    if (orderId < 0) {
        orderId = query.lastInsertId().toInt();
    }
    
    // === AUTO-INVENTORY UPDATE & DELIVERY LOG (when completing order) ===
    if (isBeingCompleted || (m_orderId < 0 && newStatus == "Completed")) {
        double deliveredCords = m_deliveredCordsEdit->value();
        
        // 1. Update inventory - subtract delivered cords from split wood
        if (deliveredCords > 0) {
            QSqlQuery invUpdate;
            // Find "Split" wood and subtract cords
            invUpdate.prepare("UPDATE inventory SET volume_cords = volume_cords - :delivered "
                            "WHERE form = 'Split' AND volume_cords >= :delivered");
            invUpdate.bindValue(":delivered", deliveredCords);
            
            if (!invUpdate.exec() || invUpdate.numRowsAffected() == 0) {
                qDebug() << "Warning: Failed to auto-update inventory:" << invUpdate.lastError().text();
                QMessageBox::warning(this, "Inventory Update", 
                                   QString("Order saved, but inventory was not auto-updated.\n"
                                          "Please manually adjust inventory by %.2f cords.").arg(deliveredCords));
            } else {
                qDebug() << "✅ Auto-updated inventory: -" << deliveredCords << "cords of split wood";
            }
        }
        
        // 2. Log to delivery_log table
        QString clientName = m_clientCombo->currentText();
        QString deliveryAddress = m_deliveryAddressEdit->text();
        QString driver = m_assignedDriverEdit->text();
        QString deliveryTime = m_deliveryTimeEdit->text();
        double startMileage = m_startMileageEdit->value();
        double endMileage = m_endMileageEdit->value();
        
        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO delivery_log (order_id, driver, delivery_date, delivery_time, "
                        "start_mileage, end_mileage, delivered_cords, client_name, client_address) "
                        "VALUES (:order_id, :driver, :delivery_date, :delivery_time, "
                        ":start_mileage, :end_mileage, :delivered_cords, :client_name, :client_address)");
        logQuery.bindValue(":order_id", orderId);
        logQuery.bindValue(":driver", driver.isEmpty() ? "Unknown" : driver);
        logQuery.bindValue(":delivery_date", QDate::currentDate().toString(Qt::ISODate));
        logQuery.bindValue(":delivery_time", deliveryTime);
        logQuery.bindValue(":start_mileage", startMileage);
        logQuery.bindValue(":end_mileage", endMileage);
        logQuery.bindValue(":delivered_cords", deliveredCords);
        logQuery.bindValue(":client_name", clientName);
        logQuery.bindValue(":client_address", deliveryAddress);
        
        if (!logQuery.exec()) {
            qDebug() << "Warning: Failed to log delivery:" << logQuery.lastError().text();
        } else {
            qDebug() << "✅ Delivery logged successfully!";
        }
    }
    
    // Update household order count and last order date
    QSqlQuery updateHousehold;
    updateHousehold.prepare("UPDATE households SET "
                           "order_count = (SELECT COUNT(*) FROM orders WHERE household_id = :household_id), "
                           "last_order_date = :last_order_date "
                           "WHERE id = :household_id");
    updateHousehold.bindValue(":household_id", householdId);
    updateHousehold.bindValue(":last_order_date", m_orderDateEdit->date().toString(Qt::ISODate));
    
    if (!updateHousehold.exec()) {
        qDebug() << "Warning: Failed to update household order count:" << updateHousehold.lastError().text();
    }
    
    accept();
}
