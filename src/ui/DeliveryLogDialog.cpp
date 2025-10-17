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

DeliveryLogDialog::DeliveryLogDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadDeliveryLog();
}

void DeliveryLogDialog::setupUI()
{
    setWindowTitle("🚚 Delivery Log - Mileage & Wood Tracking");
    resize(1200, 700);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Header with organization gradient
    auto *headerLabel = new QLabel("🚚 <b>DELIVERY LOG</b> 🪵", this);
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 " + FirewoodStyles::EMBER_ORANGE + ", "
        "stop:1 " + FirewoodStyles::FLAME_RED + "); "
        "color: white; "
        "font-size: 18pt; "
        "font-weight: bold; "
        "padding: 15px; "
        "border-radius: 8px; "
        "margin-bottom: 10px;"
    );
    mainLayout->addWidget(headerLabel);
    
    // Filters Group
    auto *filtersGroup = new QGroupBox("📅 Filters", this);
    filtersGroup->setStyleSheet(FirewoodStyles::GROUP_BOX);
    auto *filtersLayout = new QHBoxLayout(filtersGroup);
    
    auto *dateFilterLayout = new QFormLayout();
    m_startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1), this);
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
    dateFilterLayout->addRow("From:", m_startDateEdit);
    
    m_endDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
    dateFilterLayout->addRow("To:", m_endDateEdit);
    
    filtersLayout->addLayout(dateFilterLayout);
    
    auto *driverLayout = new QFormLayout();
    m_driverFilterCombo = new QComboBox(this);
    m_driverFilterCombo->setStyleSheet(FirewoodStyles::COMBO_BOX);
    m_driverFilterCombo->addItem("All Drivers", "");
    
    // Load unique drivers
    QSqlQuery driverQuery("SELECT DISTINCT driver FROM delivery_log ORDER BY driver");
    while (driverQuery.next()) {
        QString driver = driverQuery.value(0).toString();
        m_driverFilterCombo->addItem(driver, driver);
    }
    
    driverLayout->addRow("Driver:", m_driverFilterCombo);
    filtersLayout->addLayout(driverLayout);
    
    auto *applyButton = new QPushButton("🔍 Apply Filters", this);
    applyButton->setStyleSheet(FirewoodStyles::PRIMARY_BUTTON);
    applyButton->setMinimumHeight(40);
    connect(applyButton, &QPushButton::clicked, this, &DeliveryLogDialog::applyFilters);
    filtersLayout->addWidget(applyButton);
    
    auto *exportButton = new QPushButton("📊 Export to CSV", this);
    exportButton->setStyleSheet(FirewoodStyles::SECONDARY_BUTTON);
    exportButton->setMinimumHeight(40);
    connect(exportButton, &QPushButton::clicked, this, &DeliveryLogDialog::exportToCsv);
    filtersLayout->addWidget(exportButton);
    
    mainLayout->addWidget(filtersGroup);
    
    // Statistics Summary
    auto *statsGroup = new QGroupBox("📊 Summary Statistics", this);
    statsGroup->setStyleSheet(FirewoodStyles::GROUP_BOX_SUCCESS);
    auto *statsLayout = new QHBoxLayout(statsGroup);
    
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
    
    // Delivery Log Table
    m_logTable = new QTableWidget(this);
    m_logTable->setColumnCount(9);
    m_logTable->setHorizontalHeaderLabels({
        "ID", "Driver", "Date", "Time", "Start Mi", "End Mi", "Miles", "Cords", "Client"
    });
    m_logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_logTable->setAlternatingRowColors(true);
    m_logTable->setStyleSheet(FirewoodStyles::TABLE_VIEW);
    m_logTable->horizontalHeader()->setStretchLastSection(true);
    m_logTable->verticalHeader()->setVisible(false);
    
    // Set column widths
    m_logTable->setColumnWidth(0, 50);   // ID
    m_logTable->setColumnWidth(1, 120);  // Driver
    m_logTable->setColumnWidth(2, 100);  // Date
    m_logTable->setColumnWidth(3, 80);   // Time
    m_logTable->setColumnWidth(4, 80);   // Start Mi
    m_logTable->setColumnWidth(5, 80);   // End Mi
    m_logTable->setColumnWidth(6, 70);   // Miles
    m_logTable->setColumnWidth(7, 70);   // Cords
    
    mainLayout->addWidget(m_logTable);
    
    // Close Button
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *closeButton = new QPushButton("✅ Close", this);
    closeButton->setStyleSheet(FirewoodStyles::CANCEL_BUTTON);
    closeButton->setMinimumHeight(40);
    closeButton->setMinimumWidth(120);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void DeliveryLogDialog::loadDeliveryLog()
{
    QSqlQuery query;
    if (!query.exec("SELECT id, driver, delivery_date, delivery_time, "
                   "start_mileage, end_mileage, total_miles, delivered_cords, client_name "
                   "FROM delivery_log "
                   "ORDER BY delivery_date DESC, id DESC")) {
        QMessageBox::critical(this, "Database Error",
                            "Failed to load delivery log: " + query.lastError().text());
        qDebug() << "ERROR loading delivery log:" << query.lastError().text();
        return;
    }
    
    m_logTable->setRowCount(0);
    int row = 0;
    double totalMiles = 0.0;
    double totalCords = 0.0;
    
    while (query.next()) {
        m_logTable->insertRow(row);
        
        int id = query.value(0).toInt();
        QString driver = query.value(1).toString();
        QString deliveryDate = query.value(2).toString();
        QString deliveryTime = query.value(3).toString();
        double startMileage = query.value(4).toDouble();
        double endMileage = query.value(5).toDouble();
        double miles = query.value(6).toDouble();
        double cords = query.value(7).toDouble();
        QString clientName = query.value(8).toString();
        
        // Parse and format date
        QDate date = QDate::fromString(deliveryDate, Qt::ISODate);
        QString formattedDate = date.isValid() ? date.toString("MMM d, yyyy") : deliveryDate;
        
        m_logTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        m_logTable->setItem(row, 1, new QTableWidgetItem(driver));
        m_logTable->setItem(row, 2, new QTableWidgetItem(formattedDate));
        m_logTable->setItem(row, 3, new QTableWidgetItem(deliveryTime.isEmpty() ? "-" : deliveryTime));
        m_logTable->setItem(row, 4, new QTableWidgetItem(QString::number(startMileage, 'f', 1)));
        m_logTable->setItem(row, 5, new QTableWidgetItem(QString::number(endMileage, 'f', 1)));
        m_logTable->setItem(row, 6, new QTableWidgetItem(QString::number(miles, 'f', 1)));
        m_logTable->setItem(row, 7, new QTableWidgetItem(QString::number(cords, 'f', 2)));
        m_logTable->setItem(row, 8, new QTableWidgetItem(clientName));
        
        totalMiles += miles;
        totalCords += cords;
        row++;
    }
    
    // Update summary statistics
    m_deliveryCountLabel->setText(QString("<b>Deliveries:</b> %1").arg(row));
    m_totalMilesLabel->setText(QString("<b>Total Miles:</b> %1").arg(totalMiles, 0, 'f', 1));
    m_totalCordsLabel->setText(QString("<b>Total Cords:</b> %1").arg(totalCords, 0, 'f', 2));
    
    qDebug() << "Loaded" << row << "delivery log entries";
}

void DeliveryLogDialog::applyFilters()
{
    QString driverFilter = m_driverFilterCombo->currentData().toString();
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    QSqlQuery query;
    query.prepare("SELECT id, driver, delivery_date, delivery_time, "
                 "start_mileage, end_mileage, total_miles, delivered_cords, client_name "
                 "FROM delivery_log "
                 "WHERE delivery_date BETWEEN :start_date AND :end_date "
                 + (driverFilter.isEmpty() ? QString("") : QString("AND driver = :driver ")) +
                 "ORDER BY delivery_date DESC, id DESC");
    
    query.bindValue(":start_date", startDate.toString(Qt::ISODate));
    query.bindValue(":end_date", endDate.toString(Qt::ISODate));
    if (!driverFilter.isEmpty()) {
        query.bindValue(":driver", driverFilter);
    }
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error",
                            "Failed to filter delivery log: " + query.lastError().text());
        return;
    }
    
    m_logTable->setRowCount(0);
    int row = 0;
    double totalMiles = 0.0;
    double totalCords = 0.0;
    
    while (query.next()) {
        m_logTable->insertRow(row);
        
        int id = query.value(0).toInt();
        QString driver = query.value(1).toString();
        QString deliveryDate = query.value(2).toString();
        QString deliveryTime = query.value(3).toString();
        double startMileage = query.value(4).toDouble();
        double endMileage = query.value(5).toDouble();
        double miles = query.value(6).toDouble();
        double cords = query.value(7).toDouble();
        QString clientName = query.value(8).toString();
        
        QDate date = QDate::fromString(deliveryDate, Qt::ISODate);
        QString formattedDate = date.isValid() ? date.toString("MMM d, yyyy") : deliveryDate;
        
        m_logTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        m_logTable->setItem(row, 1, new QTableWidgetItem(driver));
        m_logTable->setItem(row, 2, new QTableWidgetItem(formattedDate));
        m_logTable->setItem(row, 3, new QTableWidgetItem(deliveryTime.isEmpty() ? "-" : deliveryTime));
        m_logTable->setItem(row, 4, new QTableWidgetItem(QString::number(startMileage, 'f', 1)));
        m_logTable->setItem(row, 5, new QTableWidgetItem(QString::number(endMileage, 'f', 1)));
        m_logTable->setItem(row, 6, new QTableWidgetItem(QString::number(miles, 'f', 1)));
        m_logTable->setItem(row, 7, new QTableWidgetItem(QString::number(cords, 'f', 2)));
        m_logTable->setItem(row, 8, new QTableWidgetItem(clientName));
        
        totalMiles += miles;
        totalCords += cords;
        row++;
    }
    
    m_deliveryCountLabel->setText(QString("<b>Deliveries:</b> %1").arg(row));
    m_totalMilesLabel->setText(QString("<b>Total Miles:</b> %1").arg(totalMiles, 0, 'f', 1));
    m_totalCordsLabel->setText(QString("<b>Total Cords:</b> %1").arg(totalCords, 0, 'f', 2));
    
    qDebug() << "Filtered to" << row << "deliveries";
}

void DeliveryLogDialog::exportToCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Delivery Log",
                                                   "delivery_log.csv",
                                                   "CSV Files (*.csv);;All Files (*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error",
                            "Failed to create file: " + file.errorString());
        return;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "ID,Driver,Date,Time,Start Mileage,End Mileage,Total Miles,Cords Delivered,Client\n";
    
    // Write data
    for (int row = 0; row < m_logTable->rowCount(); ++row) {
        for (int col = 0; col < m_logTable->columnCount(); ++col) {
            QString text = m_logTable->item(row, col)->text();
            // Escape commas and quotes
            if (text.contains(',') || text.contains('"')) {
                text = "\"" + text.replace("\"", "\"\"") + "\"";
            }
            out << text;
            if (col < m_logTable->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n";
    }
    
    file.close();
    
    QMessageBox::information(this, "Export Successful",
                           QString("Delivery log exported to:\n%1\n\n%2 deliveries exported.")
                           .arg(fileName)
                           .arg(m_logTable->rowCount()));
    
    qDebug() << "Exported" << m_logTable->rowCount() << "deliveries to" << fileName;
}

