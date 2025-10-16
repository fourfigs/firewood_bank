#include "EquipmentMaintenanceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QInputDialog>

EquipmentMaintenanceDialog::EquipmentMaintenanceDialog(int equipmentId, QWidget *parent)
    : QDialog(parent), m_equipmentId(equipmentId)
{
    setWindowTitle(equipmentId < 0 ? "Add Equipment" : "Edit Equipment Maintenance");
    resize(650, 550);
    
    setupUI();
    
    if (m_equipmentId >= 0) {
        loadEquipment();
    }
}

void EquipmentMaintenanceDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Equipment Information Group
    auto *equipGroup = new QGroupBox("Equipment Information", this);
    auto *equipLayout = new QFormLayout(equipGroup);
    
    m_equipmentNameEdit = new QLineEdit(this);
    m_equipmentNameEdit->setPlaceholderText("e.g., Log Splitter, Chainsaw #1");
    equipLayout->addRow("Equipment Name:", m_equipmentNameEdit);
    
    mainLayout->addWidget(equipGroup);
    
    // Service Hours Group
    auto *hoursGroup = new QGroupBox("Service Hours", this);
    auto *hoursLayout = new QFormLayout(hoursGroup);
    
    m_currentHoursEdit = new QDoubleSpinBox(this);
    m_currentHoursEdit->setRange(0.0, 999999.0);
    m_currentHoursEdit->setSingleStep(1.0);
    m_currentHoursEdit->setDecimals(1);
    m_currentHoursEdit->setSuffix(" hours");
    connect(m_currentHoursEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &EquipmentMaintenanceDialog::updateServiceAlert);
    hoursLayout->addRow("Current Hours:", m_currentHoursEdit);
    
    m_nextServiceHoursEdit = new QDoubleSpinBox(this);
    m_nextServiceHoursEdit->setRange(0.0, 999999.0);
    m_nextServiceHoursEdit->setSingleStep(10.0);
    m_nextServiceHoursEdit->setDecimals(1);
    m_nextServiceHoursEdit->setSuffix(" hours");
    connect(m_nextServiceHoursEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &EquipmentMaintenanceDialog::updateServiceAlert);
    hoursLayout->addRow("Next Service At:", m_nextServiceHoursEdit);
    
    m_alertThresholdEdit = new QDoubleSpinBox(this);
    m_alertThresholdEdit->setRange(0.0, 100.0);
    m_alertThresholdEdit->setSingleStep(1.0);
    m_alertThresholdEdit->setDecimals(1);
    m_alertThresholdEdit->setSuffix(" hours");
    m_alertThresholdEdit->setValue(5.0);
    m_alertThresholdEdit->setToolTip("Alert when this many hours away from service");
    connect(m_alertThresholdEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &EquipmentMaintenanceDialog::updateServiceAlert);
    hoursLayout->addRow("Alert Threshold:", m_alertThresholdEdit);
    
    // Service alert label
    m_serviceAlertLabel = new QLabel(this);
    m_serviceAlertLabel->setWordWrap(true);
    m_serviceAlertLabel->setStyleSheet("QLabel { padding: 10px; border-radius: 5px; }");
    hoursLayout->addRow("", m_serviceAlertLabel);
    
    // Record service button
    auto *recordServiceButton = new QPushButton("✅ Record Service Completed", this);
    connect(recordServiceButton, &QPushButton::clicked, this, &EquipmentMaintenanceDialog::recordService);
    hoursLayout->addRow("", recordServiceButton);
    
    mainLayout->addWidget(hoursGroup);
    
    // Last Service Group
    auto *lastServiceGroup = new QGroupBox("Last Service Details", this);
    auto *lastServiceLayout = new QFormLayout(lastServiceGroup);
    
    m_lastServiceDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_lastServiceDateEdit->setCalendarPopup(true);
    lastServiceLayout->addRow("Last Service Date:", m_lastServiceDateEdit);
    
    m_lastServiceNotesEdit = new QTextEdit(this);
    m_lastServiceNotesEdit->setMaximumHeight(80);
    m_lastServiceNotesEdit->setPlaceholderText("What was done during the last service...");
    lastServiceLayout->addRow("Last Service Notes:", m_lastServiceNotesEdit);
    
    mainLayout->addWidget(lastServiceGroup);
    
    // General Notes Group
    auto *notesGroup = new QGroupBox("General Notes", this);
    auto *notesLayout = new QVBoxLayout(notesGroup);
    
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMaximumHeight(80);
    m_notesEdit->setPlaceholderText("General information about this equipment...");
    notesLayout->addWidget(m_notesEdit);
    
    mainLayout->addWidget(notesGroup);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *saveButton = new QPushButton("💾 Save", this);
    saveButton->setDefault(true);
    connect(saveButton, &QPushButton::clicked, this, &EquipmentMaintenanceDialog::saveEquipment);
    buttonLayout->addWidget(saveButton);
    
    auto *cancelButton = new QPushButton("❌ Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Initialize alert
    updateServiceAlert();
}

void EquipmentMaintenanceDialog::updateServiceAlert()
{
    double currentHours = m_currentHoursEdit->value();
    double nextService = m_nextServiceHoursEdit->value();
    double threshold = m_alertThresholdEdit->value();
    
    double hoursRemaining = nextService - currentHours;
    
    if (hoursRemaining <= 0) {
        m_serviceAlertLabel->setText("⚠️ SERVICE OVERDUE!");
        m_serviceAlertLabel->setStyleSheet("QLabel { background-color: #ffcccc; color: #cc0000; "
                                          "padding: 10px; border-radius: 5px; font-weight: bold; }");
    } else if (hoursRemaining <= threshold) {
        m_serviceAlertLabel->setText(QString("⚠️ Service needed soon! %1 hours remaining")
                                     .arg(hoursRemaining, 0, 'f', 1));
        m_serviceAlertLabel->setStyleSheet("QLabel { background-color: #ffffcc; color: #cc6600; "
                                          "padding: 10px; border-radius: 5px; font-weight: bold; }");
    } else {
        m_serviceAlertLabel->setText(QString("✓ Service in %1 hours")
                                     .arg(hoursRemaining, 0, 'f', 1));
        m_serviceAlertLabel->setStyleSheet("QLabel { background-color: #ccffcc; color: #006600; "
                                          "padding: 10px; border-radius: 5px; }");
    }
}

void EquipmentMaintenanceDialog::recordService()
{
    bool ok;
    double serviceInterval = QInputDialog::getDouble(this, "Record Service", 
                                                     "Service interval (hours until next service):",
                                                     50.0, 1.0, 1000.0, 1, &ok);
    
    if (!ok) return;
    
    // Set next service based on current hours + interval
    double currentHours = m_currentHoursEdit->value();
    m_nextServiceHoursEdit->setValue(currentHours + serviceInterval);
    
    // Update last service date to today
    m_lastServiceDateEdit->setDate(QDate::currentDate());
    
    // Prompt for service notes
    QString notes = QInputDialog::getText(this, "Record Service",
                                         "Service notes:",
                                         QLineEdit::Normal, 
                                         m_lastServiceNotesEdit->toPlainText(), &ok);
    if (ok) {
        m_lastServiceNotesEdit->setPlainText(notes);
    }
    
    updateServiceAlert();
    
    QMessageBox::information(this, "Service Recorded", 
                           QString("Service recorded. Next service at %1 hours.")
                           .arg(m_nextServiceHoursEdit->value(), 0, 'f', 1));
}

void EquipmentMaintenanceDialog::loadEquipment()
{
    QSqlQuery query;
    query.prepare("SELECT equipment_name, current_hours, next_service_hours, "
                 "last_service_date, last_service_notes, alert_threshold_hours, notes "
                 "FROM equipment_maintenance WHERE id = :id");
    query.bindValue(":id", m_equipmentId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load equipment: " + query.lastError().text());
        return;
    }
    
    m_equipmentNameEdit->setText(query.value(0).toString());
    m_currentHoursEdit->setValue(query.value(1).toDouble());
    m_nextServiceHoursEdit->setValue(query.value(2).toDouble());
    
    QString lastServiceDateStr = query.value(3).toString();
    if (!lastServiceDateStr.isEmpty()) {
        QDate lastServiceDate = QDate::fromString(lastServiceDateStr, Qt::ISODate);
        if (lastServiceDate.isValid()) {
            m_lastServiceDateEdit->setDate(lastServiceDate);
        }
    }
    
    m_lastServiceNotesEdit->setPlainText(query.value(4).toString());
    m_alertThresholdEdit->setValue(query.value(5).toDouble());
    m_notesEdit->setPlainText(query.value(6).toString());
    
    updateServiceAlert();
}

void EquipmentMaintenanceDialog::saveEquipment()
{
    // Validation
    if (m_equipmentNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter an equipment name.");
        m_equipmentNameEdit->setFocus();
        return;
    }
    
    QString equipmentName = m_equipmentNameEdit->text().trimmed();
    double currentHours = m_currentHoursEdit->value();
    double nextServiceHours = m_nextServiceHoursEdit->value();
    double alertThreshold = m_alertThresholdEdit->value();
    QString lastServiceDate = m_lastServiceDateEdit->date().toString(Qt::ISODate);
    QString lastServiceNotes = m_lastServiceNotesEdit->toPlainText();
    QString notes = m_notesEdit->toPlainText();
    
    QSqlQuery query;
    
    if (m_equipmentId < 0) {
        // Create new equipment
        query.prepare("INSERT INTO equipment_maintenance "
                     "(equipment_name, current_hours, next_service_hours, last_service_date, "
                     "last_service_notes, alert_threshold_hours, notes, updated_at) "
                     "VALUES (:name, :current, :next, :last_date, :last_notes, :threshold, :notes, :updated)");
    } else {
        // Update existing equipment
        query.prepare("UPDATE equipment_maintenance SET "
                     "equipment_name = :name, current_hours = :current, next_service_hours = :next, "
                     "last_service_date = :last_date, last_service_notes = :last_notes, "
                     "alert_threshold_hours = :threshold, notes = :notes, updated_at = :updated "
                     "WHERE id = :id");
        query.bindValue(":id", m_equipmentId);
    }
    
    query.bindValue(":name", equipmentName);
    query.bindValue(":current", currentHours);
    query.bindValue(":next", nextServiceHours);
    query.bindValue(":last_date", lastServiceDate);
    query.bindValue(":last_notes", lastServiceNotes);
    query.bindValue(":threshold", alertThreshold);
    query.bindValue(":notes", notes);
    query.bindValue(":updated", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", 
                            "Failed to save equipment: " + query.lastError().text());
        qDebug() << "SQL Error:" << query.lastError().text();
        return;
    }
    
    accept();
}

