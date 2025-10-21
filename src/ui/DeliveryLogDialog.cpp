#include "DeliveryLogDialog.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDate>
#include <QDebug>

DeliveryLogDialog::DeliveryLogDialog(const QString& driverFilter, QWidget* parent)
  : QDialog(parent), m_initialDriverFilter(driverFilter)
{
  setupUI();
  // Apply initial filters upon construction.
  applyFilters();
}

void DeliveryLogDialog::setupUI()
{
  setWindowTitle("🚚 Delivery Log - Mileage & Wood Tracking");
  resize(1200, 700);

  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(20, 20, 20, 20);

  auto* headerLabel = new QLabel("🚚 <b>DELIVERY LOG</b> 🪵", this);
  headerLabel->setAlignment(Qt::AlignCenter);
  headerLabel->setStyleSheet(
    "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
    "stop:0 " + FirewoodStyles::EMBER_ORANGE + ", "
    "stop:1 " + FirewoodStyles::FLAME_RED + "); "
    "color: white; font-size: 18pt; font-weight: bold; padding: 15px; "
    "border-radius: 8px; margin-bottom: 10px;"
  );
  mainLayout->addWidget(headerLabel);

  auto* filtersGroup = new QGroupBox("📅 Filters", this);
  filtersGroup->setStyleSheet(FirewoodStyles::GROUP_BOX);
  auto* filtersLayout = new QHBoxLayout(filtersGroup);

  auto* dateFilterLayout = new QFormLayout();
  m_startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1), this);
  m_startDateEdit->setCalendarPopup(true);
  m_startDateEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
  dateFilterLayout->addRow("From:", m_startDateEdit);

  m_endDateEdit = new QDateEdit(QDate::currentDate(), this);
  m_endDateEdit->setCalendarPopup(true);
  m_endDateEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
  dateFilterLayout->addRow("To:", m_endDateEdit);

  filtersLayout->addLayout(dateFilterLayout);

  auto* driverLayout = new QFormLayout();
  m_driverFilterCombo = new QComboBox(this);
  m_driverFilterCombo->setStyleSheet(FirewoodStyles::COMBO_BOX);
  populateDriverFilter();
  driverLayout->addRow("Driver:", m_driverFilterCombo);
  filtersLayout->addLayout(driverLayout);

  // If an initial driver filter was provided, apply it and disable the combo box.
  if (!m_initialDriverFilter.isEmpty()) {
    int index = m_driverFilterCombo->findText(m_initialDriverFilter);
    if (index != -1) {
      m_driverFilterCombo->setCurrentIndex(index);
    }
    m_driverFilterCombo->setEnabled(false);
  }

  auto* applyButton = new QPushButton("🔍 Apply Filters", this);
  applyButton->setStyleSheet(FirewoodStyles::PRIMARY_BUTTON);
  applyButton->setMinimumHeight(40);
  connect(applyButton, &QPushButton::clicked, this, &DeliveryLogDialog::applyFilters);
  filtersLayout->addWidget(applyButton);

  auto* exportButton = new QPushButton("📊 Export to CSV", this);
  exportButton->setStyleSheet(FirewoodStyles::SECONDARY_BUTTON);
  exportButton->setMinimumHeight(40);
  connect(exportButton, &QPushButton::clicked, this, &DeliveryLogDialog::exportToCsv);
  filtersLayout->addWidget(exportButton);

  mainLayout->addWidget(filtersGroup);

  auto* statsGroup = new QGroupBox("📊 Summary Statistics", this);
  statsGroup->setStyleSheet(FirewoodStyles::GROUP_BOX_SUCCESS);
  auto* statsLayout = new QHBoxLayout(statsGroup);

  m_deliveryCountLabel = new QLabel("<b>Deliveries:</b> 0", this);
  m_deliveryCountLabel->setStyleSheet("font-size: 13pt; padding: 5px;");
  statsLayout->addWidget(m_deliveryCountLabel);

  m_totalMilesLabel = new QLabel("<b>Total Miles:</b> 0.0", this);
  m_totalMilesLabel->setStyleSheet("font-size: 13pt; padding: 5px;");
  statsLayout->addWidget(m_totalMilesLabel);

  m_totalCordsLabel = new QLabel("<b>Total Cords:</b> 0.0", this);
  m_totalCordsLabel->setStyleSheet("font-size: 13pt; padding: 5px;");
  statsLayout->addWidget(m_totalCordsLabel);

  mainLayout->addWidget(statsGroup);

  m_logTable = new QTableWidget(this);
  m_logTable->setColumnCount(9);
  m_logTable->setHorizontalHeaderLabels({ "ID", "Driver", "Date", "Time", "Start Mi", "End Mi", "Miles", "Cords", "Client" });
  m_logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_logTable->setAlternatingRowColors(true);
  m_logTable->setStyleSheet(FirewoodStyles::TABLE_VIEW);
  m_logTable->horizontalHeader()->setStretchLastSection(true);
  m_logTable->verticalHeader()->setVisible(false);
  m_logTable->setSortingEnabled(true);

  mainLayout->addWidget(m_logTable);

  auto* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();

  auto* closeButton = new QPushButton("✅ Close", this);
  closeButton->setStyleSheet(FirewoodStyles::CANCEL_BUTTON);
  closeButton->setMinimumHeight(40);
  closeButton->setMinimumWidth(120);
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
  buttonLayout->addWidget(closeButton);

  mainLayout->addLayout(buttonLayout);
}

void DeliveryLogDialog::populateDriverFilter()
{
  m_driverFilterCombo->clear();
  m_driverFilterCombo->addItem("All Drivers", "");
  QSqlQuery driverQuery("SELECT DISTINCT driver FROM delivery_log WHERE driver IS NOT NULL AND driver != '' ORDER BY driver");
  while (driverQuery.next()) {
    QString driver = driverQuery.value(0).toString();
    m_driverFilterCombo->addItem(driver, driver);
  }
}

void DeliveryLogDialog::applyFilters()
{
  QString driverFilter = m_driverFilterCombo->currentData().toString();
  QDate startDate = m_startDateEdit->date();
  QDate endDate = m_endDateEdit->date();

  QSqlQuery query;
  QString queryString = "SELECT id, driver, delivery_date, delivery_time, "
    "start_mileage, end_mileage, total_miles, delivered_cords, client_name "
    "FROM delivery_log WHERE delivery_date BETWEEN :start_date AND :end_date ";

  if (!driverFilter.isEmpty()) {
    queryString += "AND driver = :driver ";
  }

  queryString += "ORDER BY delivery_date DESC, id DESC";

  query.prepare(queryString);

  query.bindValue(":start_date", startDate.toString(Qt::ISODate));
  query.bindValue(":end_date", endDate.toString(Qt::ISODate));
  if (!driverFilter.isEmpty()) {
    query.bindValue(":driver", driverFilter);
  }

  if (!query.exec()) {
    QMessageBox::critical(this, "Database Error", "Failed to filter delivery log: " + query.lastError().text());
    return;
  }

  m_logTable->setRowCount(0);
  int row = 0;
  double totalMiles = 0.0;
  double totalCords = 0.0;

  while (query.next()) {
    m_logTable->insertRow(row);
    // ... (data population is the same as before, so it is omitted for brevity)
    row++;
  }

  m_deliveryCountLabel->setText(QString("<b>Deliveries:</b> %1").arg(row));
  m_totalMilesLabel->setText(QString("<b>Total Miles:</b> %1").arg(totalMiles, 0, 'f', 1));
  m_totalCordsLabel->setText(QString("<b>Total Cords:</b> %1").arg(totalCords, 0, 'f', 2));

  qDebug() << "Filtered to" << row << "deliveries";
}

void DeliveryLogDialog::exportToCsv()
{
  // ... (export logic is unchanged, so it is omitted for brevity)
}
