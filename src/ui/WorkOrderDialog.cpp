#include "WorkOrderDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
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
    
    // Order Information Group
    auto *orderGroup = new QGroupBox("Order Information", this);
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
    orderLayout->addRow("Status:", m_statusCombo);
    
    m_priorityCombo = new QComboBox(this);
    m_priorityCombo->addItems({"Low", "Normal", "High", "Emergency"});
    m_priorityCombo->setCurrentText("Normal");
    orderLayout->addRow("Priority:", m_priorityCombo);
    
    mainLayout->addWidget(orderGroup);
    
    // Delivery Information Group
    auto *deliveryGroup = new QGroupBox("Delivery Information", this);
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
    
    mainLayout->addWidget(deliveryGroup);
    
    // Payment Information Group
    auto *paymentGroup = new QGroupBox("Payment Information", this);
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
    auto *notesGroup = new QGroupBox("Additional Notes", this);
    auto *notesLayout = new QVBoxLayout(notesGroup);
    
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMaximumHeight(80);
    notesLayout->addWidget(m_notesEdit);
    
    mainLayout->addWidget(notesGroup);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *saveButton = new QPushButton("💾 Save", this);
    saveButton->setDefault(true);
    connect(saveButton, &QPushButton::clicked, this, &WorkOrderDialog::saveOrder);
    buttonLayout->addWidget(saveButton);
    
    auto *cancelButton = new QPushButton("❌ Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
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
                 "assigned_driver, payment_method, amount_paid, notes "
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
    
    QSqlQuery query;
    
    if (m_orderId < 0) {
        // Create new order
        query.prepare("INSERT INTO orders (household_id, order_date, requested_cords, delivered_cords, "
                     "status, priority, delivery_date, delivery_address, delivery_notes, "
                     "assigned_driver, payment_method, amount_paid, notes, created_at, updated_at) "
                     "VALUES (:household_id, :order_date, :requested_cords, :delivered_cords, "
                     ":status, :priority, :delivery_date, :delivery_address, :delivery_notes, "
                     ":assigned_driver, :payment_method, :amount_paid, :notes, :created_at, :updated_at)");
        
        query.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));
    } else {
        // Update existing order
        query.prepare("UPDATE orders SET household_id = :household_id, order_date = :order_date, "
                     "requested_cords = :requested_cords, delivered_cords = :delivered_cords, "
                     "status = :status, priority = :priority, delivery_date = :delivery_date, "
                     "delivery_address = :delivery_address, delivery_notes = :delivery_notes, "
                     "assigned_driver = :assigned_driver, payment_method = :payment_method, "
                     "amount_paid = :amount_paid, notes = :notes, updated_at = :updated_at "
                     "WHERE id = :id");
        
        query.bindValue(":id", m_orderId);
    }
    
    // Bind common values
    query.bindValue(":household_id", householdId);
    query.bindValue(":order_date", m_orderDateEdit->date().toString(Qt::ISODate));
    query.bindValue(":requested_cords", m_requestedCordsEdit->value());
    query.bindValue(":delivered_cords", m_deliveredCordsEdit->value());
    query.bindValue(":status", m_statusCombo->currentText());
    query.bindValue(":priority", m_priorityCombo->currentText());
    query.bindValue(":delivery_date", m_deliveryDateEdit->date().toString(Qt::ISODate));
    query.bindValue(":delivery_address", m_deliveryAddressEdit->text());
    query.bindValue(":delivery_notes", m_deliveryNotesEdit->toPlainText());
    query.bindValue(":assigned_driver", m_assignedDriverEdit->text());
    query.bindValue(":payment_method", m_paymentMethodCombo->currentText());
    query.bindValue(":amount_paid", m_amountPaidEdit->value());
    query.bindValue(":notes", m_notesEdit->toPlainText());
    query.bindValue(":updated_at", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", 
                            "Failed to save order: " + query.lastError().text());
        qDebug() << "SQL Error:" << query.lastError().text();
        return;
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
