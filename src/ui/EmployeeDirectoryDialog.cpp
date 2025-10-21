#include "EmployeeDirectoryDialog.h"
#include "DeliveryLogDialog.h" // Now we can use the enhanced dialog
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

EmployeeDirectoryDialog::EmployeeDirectoryDialog(QWidget* parent)
  : QDialog(parent)
{
  setWindowTitle("Employee Directory");
  resize(800, 500);

  setupUI();
  loadEmployees();
}

void EmployeeDirectoryDialog::setupUI()
{
  auto* mainLayout = new QVBoxLayout(this);

  auto* titleLabel = new QLabel("<h2>📋 Employee Directory</h2>", this);
  mainLayout->addWidget(titleLabel);

  auto* searchLayout = new QHBoxLayout();
  searchLayout->addWidget(new QLabel("Search:", this));

  m_searchEdit = new QLineEdit(this);
  m_searchEdit->setPlaceholderText("Search by name, email, or phone...");
  connect(m_searchEdit, &QLineEdit::textChanged, this, &EmployeeDirectoryDialog::searchEmployees);
  searchLayout->addWidget(m_searchEdit);

  mainLayout->addLayout(searchLayout);

  m_employeesTable = new QTableWidget(this);
  m_employeesTable->setColumnCount(6);
  m_employeesTable->setHorizontalHeaderLabels({ "ID", "Full Name", "Role", "Email", "Phone", "Availability" });
  m_employeesTable->horizontalHeader()->setStretchLastSection(true);
  m_employeesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_employeesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_employeesTable->setSelectionMode(QAbstractItemView::SingleSelection);
  m_employeesTable->setAlternatingRowColors(true);
  m_employeesTable->verticalHeader()->setVisible(false);
  m_employeesTable->setSortingEnabled(true);
  m_employeesTable->hideColumn(0);

  connect(m_employeesTable, &QTableWidget::doubleClicked, this, &EmployeeDirectoryDialog::viewProfile);

  mainLayout->addWidget(m_employeesTable);

  auto* buttonLayout = new QHBoxLayout();

  m_viewButton = new QPushButton("👁️ View Details", this);
  connect(m_viewButton, &QPushButton::clicked, this, &EmployeeDirectoryDialog::viewProfile);
  buttonLayout->addWidget(m_viewButton);

  m_deliveryHistoryButton = new QPushButton("🚚 View Delivery History", this);
  connect(m_deliveryHistoryButton, &QPushButton::clicked, this, &EmployeeDirectoryDialog::viewDeliveryHistory);
  buttonLayout->addWidget(m_deliveryHistoryButton);

  buttonLayout->addStretch();

  m_refreshButton = new QPushButton("🔄 Refresh", this);
  connect(m_refreshButton, &QPushButton::clicked, this, &EmployeeDirectoryDialog::loadEmployees);
  buttonLayout->addWidget(m_refreshButton);

  auto* closeButton = new QPushButton("Close", this);
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
  buttonLayout->addWidget(closeButton);

  mainLayout->addLayout(buttonLayout);
}

void EmployeeDirectoryDialog::loadEmployees()
{
  m_employeesTable->setRowCount(0);

  QSqlQuery query("SELECT id, full_name, role, email, phone, availability "
    "FROM users WHERE (role = 'employee' OR role = 'admin' OR role = 'lead') AND active = 1 "
    "ORDER BY role DESC, full_name");

  if (!query.exec()) {
    QMessageBox::critical(this, "Error", "Failed to load employees: " + query.lastError().text());
    return;
  }

  int row = 0;
  while (query.next()) {
    m_employeesTable->insertRow(row);

    m_employeesTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
    m_employeesTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
    // ... (rest of data population is unchanged)

    row++;
  }

  m_employeesTable->resizeColumnsToContents();
}

void EmployeeDirectoryDialog::viewDeliveryHistory()
{
  if (m_employeesTable->currentRow() < 0) {
    QMessageBox::information(this, "No Selection", "Please select a driver to view their delivery history.");
    return;
  }

  int row = m_employeesTable->currentRow();
  QString driverName = m_employeesTable->item(row, 1)->text();

  // Create an instance of the DeliveryLogDialog, passing the driver's name to the constructor.
  // This will open the dialog pre-filtered for that driver.
  DeliveryLogDialog dialog(driverName, this);
  dialog.setWindowTitle(QString("Delivery Log for %1").arg(driverName));
  dialog.exec();
}


void EmployeeDirectoryDialog::searchEmployees()
{
  // ... (functionality is unchanged)
}

void EmployeeDirectoryDialog::viewProfile()
{
  // ... (functionality is unchanged)
}

