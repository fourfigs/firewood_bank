#include "WorkOrderDialog.h"
#include "ClientDialog.h"
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

WorkOrderDialog::WorkOrderDialog(int orderId, QWidget* parent)
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
  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);

  auto* orgHeader = new QLabel(this);
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

  auto* orderHeaderLayout = new QHBoxLayout();
  QString orderNumText = m_orderId >= 0 ? QString("Order #%1").arg(m_orderId) : "New Order";
  auto* orderNumLabel = new QLabel("<b>" + orderNumText + "</b>", this);
  orderNumLabel->setStyleSheet("font-size: 11pt; color: " + FirewoodStyles::FOREST_GREEN + ";");
  auto* dateLabel = new QLabel("<b>Date:</b> " + QDate::currentDate().toString("MMMM dd, yyyy"), this);
  dateLabel->setStyleSheet("font-size: 11pt; color: " + FirewoodStyles::CHARCOAL + ";");
  orderHeaderLayout->addWidget(orderNumLabel);
  orderHeaderLayout->addStretch();
  orderHeaderLayout->addWidget(dateLabel);
  mainLayout->addLayout(orderHeaderLayout);

  auto* orderGroup = new QGroupBox("Order Information", this);
  orderGroup->setProperty("class", "fire");
  orderGroup->setStyleSheet(FirewoodStyles::GROUP_BOX_FIRE);
  auto* orderLayout = new QFormLayout(orderGroup);

  auto* clientLayout = new QHBoxLayout();
  m_clientCombo = new QComboBox(this);
  clientLayout->addWidget(m_clientCombo, 1);
  m_newClientButton = new QPushButton("➕ New Client...", this);
  connect(m_newClientButton, &QPushButton::clicked, this, &WorkOrderDialog::onNewClientClicked);
  clientLayout->addWidget(m_newClientButton);

  loadClients();
  orderLayout->addRow("Client:", clientLayout);

  m_orderDateEdit = new QDateEdit(QDate::currentDate(), this);
  m_orderDateEdit->setCalendarPopup(true);
  orderLayout->addRow("Order Date:", m_orderDateEdit);

  m_requestedCordsEdit = new QDoubleSpinBox(this);
  m_requestedCordsEdit->setRange(0.0, 999.0);
  m_requestedCordsEdit->setSingleStep(0.5);
  m_requestedCordsEdit->setSuffix(" cords");
  orderLayout->addRow("Requested Cords:", m_requestedCordsEdit);

  m_statusCombo = new QComboBox(this);
  m_statusCombo->addItems({ "Pending", "Scheduled", "In Progress", "Completed", "Cancelled" });
  connect(m_statusCombo, &QComboBox::currentTextChanged, this, &WorkOrderDialog::onStatusChanged);
  orderLayout->addRow("Status:", m_statusCombo);

  m_priorityCombo = new QComboBox(this);
  m_priorityCombo->addItems({ "Low", "Normal", "High", "Emergency" });
  m_priorityCombo->setCurrentText("Normal");
  orderLayout->addRow("Priority:", m_priorityCombo);

  mainLayout->addWidget(orderGroup);

  auto* deliveryGroup = new QGroupBox("🚚 Delivery Information", this);
  deliveryGroup->setStyleSheet(FirewoodStyles::GROUP_BOX);
  auto* deliveryLayout = new QFormLayout(deliveryGroup);

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
  m_endMileageEdit->setEnabled(false);
  deliveryLayout->addRow("Ending Odometer:", m_endMileageEdit);

  mainLayout->addWidget(deliveryGroup);

  auto* paymentGroup = new QGroupBox("💳 Payment Information", this);
  paymentGroup->setStyleSheet(FirewoodStyles::GROUP_BOX_SUCCESS);
  auto* paymentLayout = new QFormLayout(paymentGroup);

  m_paymentMethodCombo = new QComboBox(this);
  m_paymentMethodCombo->addItems({ "Cash", "Check", "Credit Card", "Work-for-Wood", "Voucher", "Free" });
  paymentLayout->addRow("Payment Method:", m_paymentMethodCombo);

  m_amountPaidEdit = new QDoubleSpinBox(this);
  m_amountPaidEdit->setRange(0.0, 99999.0);
  m_amountPaidEdit->setPrefix("$");
  m_amountPaidEdit->setSingleStep(1.0);
  paymentLayout->addRow("Amount Paid:", m_amountPaidEdit);

  mainLayout->addWidget(paymentGroup);

  auto* notesGroup = new QGroupBox("📝 Additional Notes", this);
  notesGroup->setStyleSheet(FirewoodStyles::GROUP_BOX);
  auto* notesLayout = new QVBoxLayout(notesGroup);

  m_notesEdit = new QTextEdit(this);
  m_notesEdit->setMaximumHeight(80);
  notesLayout->addWidget(m_notesEdit);

  mainLayout->addWidget(notesGroup);

  auto* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();

  auto* saveButton = new QPushButton("💾 Save Order", this);
  saveButton->setProperty("class", "primary");
  saveButton->setStyleSheet(FirewoodStyles::PRIMARY_BUTTON);
  saveButton->setDefault(true);
  connect(saveButton, &QPushButton::clicked, this, &WorkOrderDialog::saveOrder);
  buttonLayout->addWidget(saveButton);

  auto* cancelButton = new QPushButton("❌ Cancel", this);
  cancelButton->setProperty("class", "cancel");
  cancelButton->setStyleSheet(FirewoodStyles::CANCEL_BUTTON);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  buttonLayout->addWidget(cancelButton);

  mainLayout->addLayout(buttonLayout);
}

void WorkOrderDialog::onNewClientClicked()
{
  ClientDialog dialog(-1, this);
  if (dialog.exec() == QDialog::Accepted) {
    // Reload the client list
    int newClientId = dialog.getClientId();
    QString newClientName = dialog.getClientName();
    qDebug() << "New client created with ID:" << newClientId << "Name:" << newClientName;

    loadClients();

    // Find and select the new client
    int index = m_clientCombo->findData(newClientId);
    if (index != -1) {
      m_clientCombo->setCurrentIndex(index);
    }
    else {
      qDebug() << "Could not find new client in combobox";
    }
  }
}

void WorkOrderDialog::loadOrder()
{
    if (m_orderId <= 0) {
        // New order - set defaults
        m_orderDateEdit->setDate(QDate::currentDate());
        m_requestedCordsEdit->setValue(0);
        m_deliveredCordsEdit->setValue(0);
        m_statusCombo->setCurrentText("New");
        m_priorityCombo->setCurrentText("Normal");
        m_deliveryDateEdit->setDate(QDate::currentDate());
        m_paymentMethodCombo->setCurrentText("Cash");
        m_amountPaidEdit->setValue(0);
        m_assignedDriverEdit->clear();
        m_deliveryAddressEdit->clear();
        m_deliveryNotesEdit->clear();
        m_notesEdit->clear();
        return;
    }

    // Load existing order
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT * FROM orders WHERE id = :id");
    query.bindValue(":id", m_orderId);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to load order:" << query.lastError().text();
        return;
    }
    
    if (query.next()) {
        m_orderDateEdit->setDate(query.value("order_date").toDate());
        m_requestedCordsEdit->setValue(query.value("requested_amount").toDouble());
        m_deliveredCordsEdit->setValue(query.value("delivered_amount").toDouble());
        m_statusCombo->setCurrentText(query.value("status").toString());
        m_priorityCombo->setCurrentText(query.value("priority").toString());
        m_deliveryDateEdit->setDate(query.value("delivery_date").toDate());
        m_paymentMethodCombo->setCurrentText(query.value("payment_method").toString());
        m_amountPaidEdit->setValue(query.value("amount").toDouble());
        m_assignedDriverEdit->setText(query.value("driver").toString());
        m_deliveryAddressEdit->setText(query.value("delivery_address").toString());
        m_deliveryNotesEdit->setText(query.value("delivery_notes").toString());
        m_notesEdit->setText(query.value("notes").toString());
        
        // Set client
        int clientId = query.value("household_id").toInt();
        int index = m_clientCombo->findData(clientId);
        if (index != -1) {
            m_clientCombo->setCurrentIndex(index);
        }
    }
}

void WorkOrderDialog::loadClients()
{
    m_clientCombo->clear();
    
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT id, full_name FROM users WHERE user_type IN ('client', 'volunteer') ORDER BY full_name");
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to load clients:" << query.lastError().text();
        return;
    }
    
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        m_clientCombo->addItem(name, id);
    }
}

void WorkOrderDialog::saveOrder()
{
    if (m_clientCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Form Error", "Please select a client.");
        return;
    }
    
    int clientId = m_clientCombo->currentData().toInt();
    QDate orderDate = m_orderDateEdit->date();
    double requestedAmount = m_requestedCordsEdit->value();
    double deliveredAmount = m_deliveredCordsEdit->value();
    QString status = m_statusCombo->currentText();
    QString priority = m_priorityCombo->currentText();
    QDate deliveryDate = m_deliveryDateEdit->date();
    QString paymentMethod = m_paymentMethodCombo->currentText();
    double amount = m_amountPaidEdit->value();
    QString driver = m_assignedDriverEdit->text();
    QString deliveryAddress = m_deliveryAddressEdit->text();
    QString deliveryNotes = m_deliveryNotesEdit->toPlainText();
    QString notes = m_notesEdit->toPlainText();
    
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    
    if (m_orderId <= 0) {
        // Insert new order
        query.prepare("INSERT INTO orders (household_id, order_date, requested_amount, delivered_amount, status, priority, delivery_date, payment_method, amount, driver, delivery_address, delivery_notes, notes, created_by, created_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.bindValue(0, clientId);
        query.bindValue(1, orderDate);
        query.bindValue(2, requestedAmount);
        query.bindValue(3, deliveredAmount);
        query.bindValue(4, status);
        query.bindValue(5, priority);
        query.bindValue(6, deliveryDate);
        query.bindValue(7, paymentMethod);
        query.bindValue(8, amount);
        query.bindValue(9, driver);
        query.bindValue(10, deliveryAddress);
        query.bindValue(11, deliveryNotes);
        query.bindValue(12, notes);
        query.bindValue(13, "System"); // TODO: Get actual user
        query.bindValue(14, QDateTime::currentDateTime());
    } else {
        // Update existing order
        query.prepare("UPDATE orders SET household_id = ?, order_date = ?, requested_amount = ?, delivered_amount = ?, status = ?, priority = ?, delivery_date = ?, payment_method = ?, amount = ?, driver = ?, delivery_address = ?, delivery_notes = ?, notes = ?, updated_at = ? WHERE id = ?");
        query.bindValue(0, clientId);
        query.bindValue(1, orderDate);
        query.bindValue(2, requestedAmount);
        query.bindValue(3, deliveredAmount);
        query.bindValue(4, status);
        query.bindValue(5, priority);
        query.bindValue(6, deliveryDate);
        query.bindValue(7, paymentMethod);
        query.bindValue(8, amount);
        query.bindValue(9, driver);
        query.bindValue(10, deliveryAddress);
        query.bindValue(11, deliveryNotes);
        query.bindValue(12, notes);
        query.bindValue(13, QDateTime::currentDateTime());
        query.bindValue(14, m_orderId);
    }
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to save order:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to save order: " + query.lastError().text());
        return;
    }
    
    if (m_orderId <= 0) {
        m_orderId = query.lastInsertId().toInt();
        qDebug() << "Order created with ID:" << m_orderId;
    } else {
        qDebug() << "Order updated with ID:" << m_orderId;
    }
    
    accept();
}

void WorkOrderDialog::onStatusChanged(const QString &status)
{
    // Enable/disable fields based on status
    bool isCompleted = (status == "Completed");
    bool isDelivered = (status == "Delivered" || status == "Completed");
    
    m_deliveredCordsEdit->setEnabled(isDelivered);
    m_deliveryDateEdit->setEnabled(isDelivered);
    m_assignedDriverEdit->setEnabled(isDelivered);
    m_deliveryAddressEdit->setEnabled(isDelivered);
    m_deliveryNotesEdit->setEnabled(isDelivered);
    m_paymentMethodCombo->setEnabled(isCompleted);
    m_amountPaidEdit->setEnabled(isCompleted);
}

// ... rest of WorkOrderDialog.cpp ...
