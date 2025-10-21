#include "ClientDialog.h"
#include "WorkOrderDialog.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QDate>
#include <QDebug>
#include <QTableView>

ClientDialog::ClientDialog(int clientId, QWidget* parent)
  : QDialog(parent), m_clientId(clientId), m_isNewClient(clientId == -1)
{
  setupUI();

  if (!m_isNewClient) {
    loadClientData();
    loadVolunteerHours();
    loadOrderHistory(); // Load order history for existing clients
  }

  setWindowTitle(m_isNewClient ? "Add New Client" : "Edit Client");
  resize(900, 700);
}

void ClientDialog::setupUI()
{
  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(15);

  m_tabs = new QTabWidget(this);
  mainLayout->addWidget(m_tabs);

  createBasicInfoTab();
  createOrderHistoryTab(); // Changed order to make it more prominent
  createVolunteerTab();

  // Buttons
  auto* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();

  m_cancelButton = new QPushButton("Cancel", this);
  connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  buttonLayout->addWidget(m_cancelButton);

  m_saveButton = new QPushButton("Save", this);
  m_saveButton->setDefault(true);
  m_saveButton->setStyleSheet(
    "QPushButton { background-color: #0078d4; color: white; font-weight: bold; padding: 8px 20px; }"
    "QPushButton:hover { background-color: #106ebe; }"
  );
  connect(m_saveButton, &QPushButton::clicked, this, &ClientDialog::onSaveClicked);
  buttonLayout->addWidget(m_saveButton);

  mainLayout->addLayout(buttonLayout);
}

void ClientDialog::createBasicInfoTab()
{
  auto* basicTab = new QWidget();
  auto* layout = new QVBoxLayout(basicTab);
  layout->setSpacing(15);

  // Contact Information
  auto* contactGroup = new QGroupBox("Contact Information");
  auto* contactLayout = new QFormLayout(contactGroup);
  contactLayout->setSpacing(10);

  m_nameEdit = new QLineEdit();
  m_nameEdit->setPlaceholderText("Full name or household name");
  contactLayout->addRow("Name:*", m_nameEdit);

  m_phoneEdit = new QLineEdit();
  m_phoneEdit->setPlaceholderText("(555) 555-5555");
  contactLayout->addRow("Phone:", m_phoneEdit);

  m_emailEdit = new QLineEdit();
  m_emailEdit->setPlaceholderText("email@example.com");
  contactLayout->addRow("Email:", m_emailEdit);

  layout->addWidget(contactGroup);

  // Address Information
  auto* addressGroup = new QGroupBox("Address Information");
  auto* addressLayout = new QFormLayout(addressGroup);
  addressLayout->setSpacing(10);

  m_addressEdit = new QTextEdit();
  m_addressEdit->setMaximumHeight(80);
  m_addressEdit->setPlaceholderText("Physical address");
  addressLayout->addRow("Physical Address:", m_addressEdit);

  m_mailingAddressEdit = new QTextEdit();
  m_mailingAddressEdit->setMaximumHeight(80);
  m_mailingAddressEdit->setPlaceholderText("Leave blank if same as physical");
  addressLayout->addRow("Mailing Address:", m_mailingAddressEdit);

  m_gateCodeEdit = new QLineEdit();
  m_gateCodeEdit->setPlaceholderText("Access code or directions");
  m_gateCodeEdit->setEchoMode(QLineEdit::Password);
  addressLayout->addRow("Gate Code:", m_gateCodeEdit);

  layout->addWidget(addressGroup);

  // Additional Information
  auto* additionalGroup = new QGroupBox("Additional Information");
  auto* additionalLayout = new QFormLayout(additionalGroup);

  m_stoveSizeCombo = new QComboBox();
  m_stoveSizeCombo->addItems({ "N/A", "Small (< 18\")", "Medium (18-24\")", "Large (> 24\")" });
  additionalLayout->addRow("Stove Size:", m_stoveSizeCombo);

  m_notesEdit = new QTextEdit();
  m_notesEdit->setMaximumHeight(100);
  m_notesEdit->setPlaceholderText("General notes about the client");
  additionalLayout->addRow("Notes:", m_notesEdit);

  layout->addWidget(additionalGroup);
  layout->addStretch();

  m_tabs->addTab(basicTab, "📋 Basic Info");
}

void ClientDialog::createVolunteerTab()
{
  auto* volunteerTab = new QWidget();
  auto* layout = new QVBoxLayout(volunteerTab);
  layout->setSpacing(15);

  // Volunteer Status
  auto* statusGroup = new QGroupBox("Volunteer Status");
  auto* statusLayout = new QVBoxLayout(statusGroup);

  m_isVolunteerCheck = new QCheckBox("This client is a volunteer");
  m_isVolunteerCheck->setStyleSheet("font-weight: bold; font-size: 12px;");
  connect(m_isVolunteerCheck, &QCheckBox::stateChanged, this, &ClientDialog::onVolunteerCheckChanged);
  statusLayout->addWidget(m_isVolunteerCheck);

  auto* requirementsLayout = new QHBoxLayout();
  m_waiverSignedCheck = new QCheckBox("Waiver Signed");
  m_hasLicenseCheck = new QCheckBox("Has License");
  m_hasVehicleCheck = new QCheckBox("Has Working Vehicle");
  requirementsLayout->addWidget(m_waiverSignedCheck);
  requirementsLayout->addWidget(m_hasLicenseCheck);
  requirementsLayout->addWidget(m_hasVehicleCheck);
  requirementsLayout->addStretch();
  statusLayout->addLayout(requirementsLayout);

  layout->addWidget(statusGroup);

  // Work for Wood Program
  auto* workGroup = new QGroupBox("Work for Wood Program");
  auto* workLayout = new QFormLayout(workGroup);

  m_worksForWoodCheck = new QCheckBox("Participates in Work for Wood");
  m_worksForWoodCheck->setStyleSheet("font-weight: bold;");
  connect(m_worksForWoodCheck, &QCheckBox::stateChanged, this, &ClientDialog::onWorksForWoodChanged);
  workLayout->addRow("", m_worksForWoodCheck);

  m_woodCreditSpin = new QDoubleSpinBox();
  m_woodCreditSpin->setRange(0, 999999);
  m_woodCreditSpin->setDecimals(2);
  m_woodCreditSpin->setSuffix(" cords");
  m_woodCreditSpin->setEnabled(false);
  workLayout->addRow("Wood Credit Received:", m_woodCreditSpin);

  m_creditBalanceSpin = new QDoubleSpinBox();
  m_creditBalanceSpin->setRange(-999999, 999999);
  m_creditBalanceSpin->setDecimals(2);
  m_creditBalanceSpin->setSuffix(" cords");
  m_creditBalanceSpin->setEnabled(false);
  workLayout->addRow("Current Credit Balance:", m_creditBalanceSpin);

  layout->addWidget(workGroup);

  // Volunteer Hours Log
  auto* hoursGroup = new QGroupBox("Volunteer Hours Log");
  auto* hoursLayout = new QVBoxLayout(hoursGroup);

  auto* summaryLayout = new QHBoxLayout();
  m_totalHoursLabel = new QLabel("Total Hours: 0.0");
  m_totalHoursLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
  summaryLayout->addWidget(m_totalHoursLabel);

  summaryLayout->addSpacing(20);

  m_lastVolunteerDateLabel = new QLabel("Last Date: Never");
  m_lastVolunteerDateLabel->setStyleSheet("font-size: 13px;");
  summaryLayout->addWidget(m_lastVolunteerDateLabel);
  summaryLayout->addStretch();
  hoursLayout->addLayout(summaryLayout);

  m_volunteerHoursTable = new QTableWidget();
  m_volunteerHoursTable->setColumnCount(4);
  m_volunteerHoursTable->setHorizontalHeaderLabels({ "Date", "Hours", "Activity", "Notes" });
  m_volunteerHoursTable->horizontalHeader()->setStretchLastSection(true);
  m_volunteerHoursTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_volunteerHoursTable->setAlternatingRowColors(true);
  m_volunteerHoursTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  hoursLayout->addWidget(m_volunteerHoursTable);

  auto* hoursButtonLayout = new QHBoxLayout();
  auto* addHoursBtn = new QPushButton("Add Hours");
  connect(addHoursBtn, &QPushButton::clicked, this, &ClientDialog::onAddVolunteerHours);
  hoursButtonLayout->addWidget(addHoursBtn);

  auto* deleteHoursBtn = new QPushButton("Delete Selected");
  connect(deleteHoursBtn, &QPushButton::clicked, this, &ClientDialog::onDeleteVolunteerHours);
  hoursButtonLayout->addWidget(deleteHoursBtn);
  hoursButtonLayout->addStretch();

  hoursLayout->addLayout(hoursButtonLayout);
  layout->addWidget(hoursGroup);

  m_tabs->addTab(volunteerTab, "🤝 Volunteer Info");

  // Disable volunteer fields initially
  onVolunteerCheckChanged(Qt::Unchecked);
}

void ClientDialog::createOrderHistoryTab()
{
  auto* orderTab = new QWidget();
  auto* layout = new QVBoxLayout(orderTab);
  layout->setSpacing(15);

  auto* summaryLayout = new QHBoxLayout();
  m_orderCountLabel = new QLabel("Total Orders: 0");
  m_orderCountLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
  summaryLayout->addWidget(m_orderCountLabel);

  summaryLayout->addSpacing(20);

  m_lastOrderDateLabel = new QLabel("Last Order: Never");
  m_lastOrderDateLabel->setStyleSheet("font-size: 14px;");
  summaryLayout->addWidget(m_lastOrderDateLabel);
  summaryLayout->addStretch();

  layout->addLayout(summaryLayout);

  m_orderHistoryModel = new QSqlTableModel(this);
  m_orderHistoryModel->setTable("orders");

  m_orderHistoryView = new QTableView();
  m_orderHistoryView->setModel(m_orderHistoryModel);
  m_orderHistoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_orderHistoryView->setSelectionMode(QAbstractItemView::SingleSelection);
  m_orderHistoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_orderHistoryView->setAlternatingRowColors(true);
  m_orderHistoryView->setSortingEnabled(true);
  m_orderHistoryView->horizontalHeader()->setStretchLastSection(true);

  connect(m_orderHistoryView, &QTableView::doubleClicked, this, &ClientDialog::onOrderHistoryDoubleClicked);

  layout->addWidget(m_orderHistoryView);

  m_tabs->addTab(orderTab, "📦 Order History");
}

void ClientDialog::onVolunteerCheckChanged(int state)
{
  bool isVolunteer = (state == Qt::Checked);
  m_waiverSignedCheck->setEnabled(isVolunteer);
  m_hasLicenseCheck->setEnabled(isVolunteer);
  m_hasVehicleCheck->setEnabled(isVolunteer);
}

void ClientDialog::onWorksForWoodChanged(int state)
{
  bool worksForWood = (state == Qt::Checked);
  m_woodCreditSpin->setEnabled(worksForWood);
  m_creditBalanceSpin->setEnabled(worksForWood);
}

void ClientDialog::loadClientData()
{
  QSqlQuery query;
  query.prepare("SELECT name, phone, address, email, mailing_address, gate_code, notes, stove_fireplace_size, "
    "is_volunteer, waiver_signed, has_license, has_working_vehicle, participating_in_work_for_wood, "
    "wood_credit_received, credit_balance, last_volunteer_date, order_count, last_order_date "
    "FROM households WHERE id = :id");
  query.bindValue(":id", m_clientId);

  if (!query.exec() || !query.next()) {
    qDebug() << "ERROR: Failed to load client data:" << query.lastError().text();
    return;
  }

  m_nameEdit->setText(query.value(0).toString());
  m_phoneEdit->setText(query.value(1).toString());
  m_addressEdit->setPlainText(query.value(2).toString());
  m_emailEdit->setText(query.value(3).toString());
  m_mailingAddressEdit->setPlainText(query.value(4).toString());
  m_gateCodeEdit->setText(query.value(5).toString());
  m_notesEdit->setPlainText(query.value(6).toString());

  QString stoveSize = query.value(7).toString();
  int stoveIndex = m_stoveSizeCombo->findText(stoveSize);
  if (stoveIndex >= 0) m_stoveSizeCombo->setCurrentIndex(stoveIndex);

  m_isVolunteerCheck->setChecked(query.value(8).toInt() == 1);
  m_waiverSignedCheck->setChecked(query.value(9).toInt() == 1);
  m_hasLicenseCheck->setChecked(query.value(10).toInt() == 1);
  m_hasVehicleCheck->setChecked(query.value(11).toInt() == 1);
  m_worksForWoodCheck->setChecked(query.value(12).toInt() == 1);
  m_woodCreditSpin->setValue(query.value(13).toDouble());
  m_creditBalanceSpin->setValue(query.value(14).toDouble());

  m_orderCountLabel->setText(QString("Total Orders: %1").arg(query.value(16).toInt()));
  m_lastOrderDateLabel->setText(QString("Last Order: %1").arg(
    query.value(17).toString().isEmpty() ? "Never" : query.value(17).toString()));

  qDebug() << "Loaded client data for ID:" << m_clientId;
}

void ClientDialog::loadVolunteerHours()
{
  QSqlQuery query;
  query.prepare("SELECT date, hours, activity, notes FROM volunteer_hours "
    "WHERE household_id = :id ORDER BY date DESC");
  query.bindValue(":id", m_clientId);

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to load volunteer hours:" << query.lastError().text();
    return;
  }

  m_volunteerHoursTable->setRowCount(0);
  double totalHours = 0.0;
  QString lastDate;
  int row = 0;

  while (query.next()) {
    m_volunteerHoursTable->insertRow(row);
    m_volunteerHoursTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
    m_volunteerHoursTable->setItem(row, 1, new QTableWidgetItem(QString::number(query.value(1).toDouble(), 'f', 1)));
    m_volunteerHoursTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
    m_volunteerHoursTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));

    totalHours += query.value(1).toDouble();
    if (row == 0) lastDate = query.value(0).toString();
    row++;
  }

  m_totalHoursLabel->setText(QString("Total Hours: %1").arg(totalHours, 0, 'f', 1));
  m_lastVolunteerDateLabel->setText(QString("Last Date: %1").arg(lastDate.isEmpty() ? "Never" : lastDate));
}

void ClientDialog::loadOrderHistory() {
  if (m_isNewClient) return;

  m_orderHistoryModel->setFilter(QString("household_id = %1").arg(m_clientId));
  m_orderHistoryModel->select();

  m_orderHistoryModel->setHeaderData(m_orderHistoryModel->fieldIndex("order_date"), Qt::Horizontal, "Order Date");
  m_orderHistoryModel->setHeaderData(m_orderHistoryModel->fieldIndex("delivered_cords"), Qt::Horizontal, "Cords Delivered");
  m_orderHistoryModel->setHeaderData(m_orderHistoryModel->fieldIndex("status"), Qt::Horizontal, "Status");
  m_orderHistoryModel->setHeaderData(m_orderHistoryModel->fieldIndex("priority"), Qt::Horizontal, "Priority");

  for (int i = 0; i < m_orderHistoryModel->columnCount(); ++i) {
    QString fieldName = m_orderHistoryModel->headerData(i, Qt::Horizontal).toString();
    if (fieldName != "Order Date" && fieldName != "Cords Delivered" && fieldName != "Status" && fieldName != "Priority") {
      m_orderHistoryView->hideColumn(i);
    }
  }
}

void ClientDialog::onOrderHistoryDoubleClicked(const QModelIndex& index) {
  if (!index.isValid()) return;

  int orderId = m_orderHistoryModel->data(m_orderHistoryModel->index(index.row(), 0)).toInt();

  WorkOrderDialog dialog(orderId, this);
  dialog.exec();

  // Refresh history after potential edits
  loadOrderHistory();
}

void ClientDialog::onAddVolunteerHours()
{
  if (m_isNewClient) {
    QMessageBox::information(this, "Save First", "Please save the client first before adding volunteer hours.");
    return;
  }

  QMessageBox::information(this, "Coming Soon", "Volunteer hours entry dialog will be implemented next.");
}

void ClientDialog::onDeleteVolunteerHours()
{
  QMessageBox::information(this, "Coming Soon", "Delete volunteer hours will be implemented next.");
}

void ClientDialog::onSaveClicked()
{
  if (saveClientData()) {
    accept();
  }
}

bool ClientDialog::saveClientData()
{
  // Validate required fields
  if (m_nameEdit->text().trimmed().isEmpty()) {
    QMessageBox::warning(this, "Validation Error", "Name is required.");
    m_tabs->setCurrentIndex(0);
    m_nameEdit->setFocus();
    return false;
  }

  if (m_isNewClient) {
    QString name = m_nameEdit->text().trimmed();
    QString address = m_addressEdit->toPlainText().trimmed();

    QSqlQuery checkQuery;
    checkQuery.prepare(
      "SELECT id, name, address, phone FROM households "
      "WHERE LOWER(name) = LOWER(:name) OR "
      "(LOWER(address) = LOWER(:address) AND :address != '')"
    );
    checkQuery.bindValue(":name", name);
    checkQuery.bindValue(":address", address);

    if (checkQuery.exec() && checkQuery.next()) {
      int existingId = checkQuery.value(0).toInt();
      QString existingName = checkQuery.value(1).toString();
      QString existingAddress = checkQuery.value(2).toString();
      QString existingPhone = checkQuery.value(3).toString();

      QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Possible Duplicate Household",
        QString("A household with similar information already exists:\n\n"
          "Name: %1\n"
          "Address: %2\n"
          "Phone: %3\n\n"
          "Do you want to:\n"
          "• Click 'Yes' to edit the existing household instead\n"
          "• Click 'No' to add this as a new household anyway\n"
          "• Click 'Cancel' to go back and review")
        .arg(existingName)
        .arg(existingAddress.isEmpty() ? "(No address)" : existingAddress)
        .arg(existingPhone.isEmpty() ? "(No phone)" : existingPhone),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Yes
      );

      if (reply == QMessageBox::Yes) {
        m_clientId = existingId;
        m_isNewClient = false;
        loadClientData();
        loadVolunteerHours();
        loadOrderHistory();
        setWindowTitle("Edit Client");
        QMessageBox::information(this, "Editing Existing Client",
          "You are now editing the existing household. Make any changes and click Save.");
        return false;
      }
      else if (reply == QMessageBox::Cancel) {
        return false;
      }
    }
  }

  QSqlQuery query;

  if (m_isNewClient) {
    query.prepare("INSERT INTO households (name, phone, address, email, mailing_address, gate_code, notes, stove_fireplace_size, "
      "is_volunteer, waiver_signed, has_license, has_working_vehicle, participating_in_work_for_wood, "
      "wood_credit_received, credit_balance) "
      "VALUES (:name, :phone, :address, :email, :mailing_address, :gate_code, :notes, :stove_size, "
      ":is_volunteer, :waiver_signed, :has_license, :has_working_vehicle, :works_for_wood, "
      ":wood_credit_received, :credit_balance)");
  }
  else {
    query.prepare("UPDATE households SET name = :name, phone = :phone, address = :address, email = :email, "
      "mailing_address = :mailing_address, gate_code = :gate_code, notes = :notes, stove_fireplace_size = :stove_size, "
      "is_volunteer = :is_volunteer, waiver_signed = :waiver_signed, has_license = :has_license, "
      "has_working_vehicle = :has_working_vehicle, participating_in_work_for_wood = :works_for_wood, "
      "wood_credit_received = :wood_credit_received, credit_balance = :credit_balance "
      "WHERE id = :id");
    query.bindValue(":id", m_clientId);
  }

  query.bindValue(":name", m_nameEdit->text().trimmed());
  query.bindValue(":phone", m_phoneEdit->text().trimmed());
  query.bindValue(":address", m_addressEdit->toPlainText().trimmed());
  query.bindValue(":email", m_emailEdit->text().trimmed());
  query.bindValue(":mailing_address", m_mailingAddressEdit->toPlainText().trimmed());
  query.bindValue(":gate_code", m_gateCodeEdit->text());
  query.bindValue(":notes", m_notesEdit->toPlainText());
  query.bindValue(":stove_size", m_stoveSizeCombo->currentText());
  query.bindValue(":is_volunteer", m_isVolunteerCheck->isChecked() ? 1 : 0);
  query.bindValue(":waiver_signed", m_waiverSignedCheck->isChecked() ? 1 : 0);
  query.bindValue(":has_license", m_hasLicenseCheck->isChecked() ? 1 : 0);
  query.bindValue(":has_working_vehicle", m_hasVehicleCheck->isChecked() ? 1 : 0);
  query.bindValue(":works_for_wood", m_worksForWoodCheck->isChecked() ? 1 : 0);
  query.bindValue(":wood_credit_received", m_woodCreditSpin->value());
  query.bindValue(":credit_balance", m_creditBalanceSpin->value());

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to save client:" << query.lastError().text();
    QMessageBox::critical(this, "Database Error",
      "Failed to save client data: " + query.lastError().text());
    return false;
  }

  if (m_isNewClient) {
    m_clientId = query.lastInsertId().toInt();
    qDebug() << "Created new client with ID:" << m_clientId;
  }
  else {
    qDebug() << "Updated client ID:" << m_clientId;
  }

  return true;
}
