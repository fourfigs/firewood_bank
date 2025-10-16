#include "VolunteerProfileWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

VolunteerProfileWidget::VolunteerProfileWidget(const QString &username, QWidget *parent)
    : QWidget(parent), m_username(username)
{
    m_householdId = getHouseholdId();
    setupUI();
    loadVolunteerProfile();
    loadVolunteerHours();
    loadCertifications();
    loadAvailableWorkDays();
}

int VolunteerProfileWidget::getHouseholdId()
{
    // Get the household ID associated with this username
    QSqlQuery query;
    query.prepare("SELECT id FROM households WHERE name = :username OR phone = :username OR email = :username");
    query.bindValue(":username", m_username);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    qDebug() << "Warning: Could not find household ID for username:" << m_username;
    return -1;
}

void VolunteerProfileWidget::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Title
    auto *titleLabel = new QLabel("<h2>📋 My Volunteer Profile</h2>", this);
    mainLayout->addWidget(titleLabel);
    
    // Top Section: Personal Information
    auto *profileBox = new QGroupBox("Personal Information", this);
    profileBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #0078d4; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *profileLayout = new QGridLayout(profileBox);
    
    profileLayout->addWidget(new QLabel("<b>Name:</b>"), 0, 0);
    m_nameLabel = new QLabel(this);
    profileLayout->addWidget(m_nameLabel, 0, 1);
    
    profileLayout->addWidget(new QLabel("<b>Phone:</b>"), 0, 2);
    m_phoneLabel = new QLabel(this);
    profileLayout->addWidget(m_phoneLabel, 0, 3);
    
    profileLayout->addWidget(new QLabel("<b>Physical Address:</b>"), 1, 0);
    m_addressLabel = new QLabel(this);
    m_addressLabel->setWordWrap(true);
    profileLayout->addWidget(m_addressLabel, 1, 1);
    
    profileLayout->addWidget(new QLabel("<b>Email:</b>"), 1, 2);
    m_emailLabel = new QLabel(this);
    profileLayout->addWidget(m_emailLabel, 1, 3);
    
    profileLayout->addWidget(new QLabel("<b>Mailing Address:</b>"), 2, 0);
    m_mailingAddressLabel = new QLabel(this);
    m_mailingAddressLabel->setWordWrap(true);
    profileLayout->addWidget(m_mailingAddressLabel, 2, 1, 1, 3);
    
    mainLayout->addWidget(profileBox);
    
    // Status Section
    auto *statusBox = new QGroupBox("Status & Requirements", this);
    statusBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #0078d4; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *statusLayout = new QHBoxLayout(statusBox);
    
    m_waiverCheck = new QCheckBox("Waiver Signed", this);
    m_waiverCheck->setEnabled(false);
    statusLayout->addWidget(m_waiverCheck);
    
    m_licenseCheck = new QCheckBox("Driver's License", this);
    m_licenseCheck->setEnabled(false);
    statusLayout->addWidget(m_licenseCheck);
    
    m_vehicleCheck = new QCheckBox("Working Vehicle", this);
    m_vehicleCheck->setEnabled(false);
    statusLayout->addWidget(m_vehicleCheck);
    
    statusLayout->addStretch();
    mainLayout->addWidget(statusBox);
    
    // Availability Section
    auto *availBox = new QGroupBox("My Availability", this);
    availBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #0078d4; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *availLayout = new QVBoxLayout(availBox);
    
    m_availabilityText = new QTextEdit(this);
    m_availabilityText->setMaximumHeight(60);
    m_availabilityText->setReadOnly(true);
    m_availabilityText->setPlaceholderText("(Contact office to update availability)");
    availLayout->addWidget(m_availabilityText);
    
    mainLayout->addWidget(availBox);
    
    // Middle Section: Two columns
    auto *middleLayout = new QHBoxLayout();
    
    // Left: Volunteer Hours Log
    auto *hoursBox = new QGroupBox("⏱️ My Volunteer Hours", this);
    hoursBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #28a745; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *hoursLayout = new QVBoxLayout(hoursBox);
    
    m_hoursTable = new QTableWidget(this);
    m_hoursTable->setColumnCount(3);
    m_hoursTable->setHorizontalHeaderLabels({"Date", "Hours", "Activity"});
    m_hoursTable->horizontalHeader()->setStretchLastSection(true);
    m_hoursTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_hoursTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_hoursTable->setAlternatingRowColors(true);
    m_hoursTable->verticalHeader()->setVisible(false);
    hoursLayout->addWidget(m_hoursTable);
    
    middleLayout->addWidget(hoursBox);
    
    // Right: Certifications
    auto *certBox = new QGroupBox("🏆 My Certifications", this);
    certBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #ffc107; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *certLayout = new QVBoxLayout(certBox);
    
    m_certificationsTable = new QTableWidget(this);
    m_certificationsTable->setColumnCount(3);
    m_certificationsTable->setHorizontalHeaderLabels({"Certification", "Issued", "Expires"});
    m_certificationsTable->horizontalHeader()->setStretchLastSection(true);
    m_certificationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_certificationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_certificationsTable->setAlternatingRowColors(true);
    m_certificationsTable->verticalHeader()->setVisible(false);
    certLayout->addWidget(m_certificationsTable);
    
    auto *certButtonLayout = new QHBoxLayout();
    m_addCertButton = new QPushButton("+ Add Certification", this);
    connect(m_addCertButton, &QPushButton::clicked, this, &VolunteerProfileWidget::addCertification);
    certButtonLayout->addWidget(m_addCertButton);
    certButtonLayout->addStretch();
    certLayout->addLayout(certButtonLayout);
    
    middleLayout->addWidget(certBox);
    
    mainLayout->addLayout(middleLayout);
    
    // Bottom Section: Available Work Days
    auto *workDaysBox = new QGroupBox("📅 Available Work Days", this);
    workDaysBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #dc3545; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *workDaysLayout = new QVBoxLayout(workDaysBox);
    
    m_workDaysTable = new QTableWidget(this);
    m_workDaysTable->setColumnCount(6);
    m_workDaysTable->setHorizontalHeaderLabels({"Date", "Time", "Activity", "Location", "Slots", "My Status"});
    m_workDaysTable->horizontalHeader()->setStretchLastSection(true);
    m_workDaysTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_workDaysTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_workDaysTable->setAlternatingRowColors(true);
    m_workDaysTable->verticalHeader()->setVisible(false);
    workDaysLayout->addWidget(m_workDaysTable);
    
    auto *workDayButtonLayout = new QHBoxLayout();
    m_signUpButton = new QPushButton("✅ Sign Up for Selected Work Day", this);
    connect(m_signUpButton, &QPushButton::clicked, this, &VolunteerProfileWidget::signUpForWorkDay);
    workDayButtonLayout->addWidget(m_signUpButton);
    
    m_cancelButton = new QPushButton("❌ Cancel My Sign-Up", this);
    connect(m_cancelButton, &QPushButton::clicked, this, &VolunteerProfileWidget::cancelSignUp);
    workDayButtonLayout->addWidget(m_cancelButton);
    
    m_refreshButton = new QPushButton("🔄 Refresh", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &VolunteerProfileWidget::refreshData);
    workDayButtonLayout->addWidget(m_refreshButton);
    
    workDayButtonLayout->addStretch();
    workDaysLayout->addLayout(workDayButtonLayout);
    
    mainLayout->addWidget(workDaysBox);
}

void VolunteerProfileWidget::loadVolunteerProfile()
{
    if (m_householdId < 0) {
        QMessageBox::warning(this, "Profile Not Found", 
                           "Could not find your volunteer profile. Please contact the office.");
        return;
    }
    
    QSqlQuery query;
    query.prepare("SELECT name, phone, address, mailing_address, email, availability, "
                 "waiver_signed, has_license, has_working_vehicle "
                 "FROM households WHERE id = :id");
    query.bindValue(":id", m_householdId);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "ERROR: Failed to load volunteer profile:" << query.lastError().text();
        return;
    }
    
    m_nameLabel->setText(query.value(0).toString());
    m_phoneLabel->setText(query.value(1).toString());
    m_addressLabel->setText(query.value(2).toString());
    m_mailingAddressLabel->setText(query.value(3).toString());
    m_emailLabel->setText(query.value(4).toString());
    m_availabilityText->setPlainText(query.value(5).toString());
    
    m_waiverCheck->setChecked(query.value(6).toInt() == 1);
    m_licenseCheck->setChecked(query.value(7).toInt() == 1);
    m_vehicleCheck->setChecked(query.value(8).toInt() == 1);
}

void VolunteerProfileWidget::loadVolunteerHours()
{
    if (m_householdId < 0) return;
    
    QSqlQuery query;
    query.prepare("SELECT date, hours, activity FROM volunteer_hours "
                 "WHERE household_id = :id ORDER BY date DESC LIMIT 20");
    query.bindValue(":id", m_householdId);
    
    m_hoursTable->setRowCount(0);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to load volunteer hours:" << query.lastError().text();
        return;
    }
    
    double totalHours = 0.0;
    int row = 0;
    
    while (query.next()) {
        m_hoursTable->insertRow(row);
        
        QString dateStr = query.value(0).toString();
        QDate date = QDate::fromString(dateStr, Qt::ISODate);
        m_hoursTable->setItem(row, 0, new QTableWidgetItem(date.toString("MM/dd/yyyy")));
        
        double hours = query.value(1).toDouble();
        totalHours += hours;
        m_hoursTable->setItem(row, 1, new QTableWidgetItem(QString::number(hours, 'f', 1)));
        
        m_hoursTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        row++;
    }
    
    // Add total row
    if (row > 0) {
        m_hoursTable->insertRow(row);
        auto *totalItem = new QTableWidgetItem("TOTAL");
        QFont font = totalItem->font();
        font.setBold(true);
        totalItem->setFont(font);
        m_hoursTable->setItem(row, 0, totalItem);
        
        auto *hoursItem = new QTableWidgetItem(QString::number(totalHours, 'f', 1));
        hoursItem->setFont(font);
        m_hoursTable->setItem(row, 1, hoursItem);
    }
}

void VolunteerProfileWidget::loadCertifications()
{
    if (m_householdId < 0) return;
    
    QSqlQuery query;
    query.prepare("SELECT certification_name, issue_date, expiration_date "
                 "FROM volunteer_certifications WHERE household_id = :id "
                 "ORDER BY expiration_date DESC");
    query.bindValue(":id", m_householdId);
    
    m_certificationsTable->setRowCount(0);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to load certifications:" << query.lastError().text();
        return;
    }
    
    int row = 0;
    while (query.next()) {
        m_certificationsTable->insertRow(row);
        
        m_certificationsTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        
        QString issueStr = query.value(1).toString();
        if (!issueStr.isEmpty()) {
            QDate issueDate = QDate::fromString(issueStr, Qt::ISODate);
            m_certificationsTable->setItem(row, 1, new QTableWidgetItem(issueDate.toString("MM/dd/yyyy")));
        }
        
        QString expireStr = query.value(2).toString();
        if (!expireStr.isEmpty()) {
            QDate expireDate = QDate::fromString(expireStr, Qt::ISODate);
            m_certificationsTable->setItem(row, 2, new QTableWidgetItem(expireDate.toString("MM/dd/yyyy")));
        }
        
        row++;
    }
}

void VolunteerProfileWidget::loadAvailableWorkDays()
{
    m_workDaysTable->setRowCount(0);
    
    // Load upcoming work days
    QSqlQuery query;
    query.prepare("SELECT ws.id, ws.work_date, ws.start_time, ws.end_time, ws.activity_type, "
                 "ws.location, ws.volunteer_slots, ws.slots_filled, "
                 "(SELECT COUNT(*) FROM work_schedule_signups WHERE schedule_id = ws.id AND household_id = :household_id) as signed_up "
                 "FROM work_schedule ws "
                 "WHERE ws.work_date >= date('now') "
                 "ORDER BY ws.work_date, ws.start_time");
    query.bindValue(":household_id", m_householdId);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to load work days:" << query.lastError().text();
        return;
    }
    
    int row = 0;
    while (query.next()) {
        m_workDaysTable->insertRow(row);
        
        int scheduleId = query.value(0).toInt();
        m_workDaysTable->setItem(row, 0, new QTableWidgetItem(QString::number(scheduleId)));
        m_workDaysTable->item(row, 0)->setData(Qt::UserRole, scheduleId); // Store ID in user data
        
        QString dateStr = query.value(1).toString();
        QDate date = QDate::fromString(dateStr, Qt::ISODate);
        m_workDaysTable->item(row, 0)->setText(date.toString("MM/dd/yyyy"));
        
        QString timeRange = QString("%1 - %2").arg(query.value(2).toString(), query.value(3).toString());
        m_workDaysTable->setItem(row, 1, new QTableWidgetItem(timeRange));
        
        m_workDaysTable->setItem(row, 2, new QTableWidgetItem(query.value(4).toString()));
        m_workDaysTable->setItem(row, 3, new QTableWidgetItem(query.value(5).toString()));
        
        int totalSlots = query.value(6).toInt();
        int filledSlots = query.value(7).toInt();
        int availableSlots = totalSlots - filledSlots;
        QString slotInfo = QString("%1 / %2").arg(filledSlots).arg(totalSlots);
        auto *slotsItem = new QTableWidgetItem(slotInfo);
        if (availableSlots <= 0) {
            slotsItem->setBackground(QBrush(QColor(255, 200, 200))); // Light red
        } else if (availableSlots <= 2) {
            slotsItem->setBackground(QBrush(QColor(255, 255, 200))); // Light yellow
        } else {
            slotsItem->setBackground(QBrush(QColor(200, 255, 200))); // Light green
        }
        m_workDaysTable->setItem(row, 4, slotsItem);
        
        bool signedUp = query.value(8).toInt() > 0;
        auto *statusItem = new QTableWidgetItem(signedUp ? "✓ Signed Up" : availableSlots > 0 ? "Available" : "Full");
        QFont font = statusItem->font();
        if (signedUp) {
            font.setBold(true);
            statusItem->setFont(font);
            statusItem->setForeground(QBrush(QColor(0, 128, 0)));
        }
        m_workDaysTable->setItem(row, 5, statusItem);
        
        row++;
    }
}

void VolunteerProfileWidget::signUpForWorkDay()
{
    if (m_workDaysTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a work day to sign up for.");
        return;
    }
    
    int row = m_workDaysTable->currentRow();
    int scheduleId = m_workDaysTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString status = m_workDaysTable->item(row, 5)->text();
    
    if (status == "✓ Signed Up") {
        QMessageBox::information(this, "Already Signed Up", 
                               "You are already signed up for this work day.");
        return;
    }
    
    if (status == "Full") {
        QMessageBox::warning(this, "Work Day Full", 
                           "This work day is already full. Please choose another day.");
        return;
    }
    
    QString workDay = m_workDaysTable->item(row, 0)->text();
    QString time = m_workDaysTable->item(row, 1)->text();
    QString activity = m_workDaysTable->item(row, 2)->text();
    
    auto response = QMessageBox::question(this, "Confirm Sign-Up",
                                         QString("Sign up for:\n\n%1\n%2\n%3\n\nAre you sure?")
                                         .arg(workDay, time, activity),
                                         QMessageBox::Yes | QMessageBox::No);
    
    if (response != QMessageBox::Yes) return;
    
    QSqlQuery query;
    query.prepare("INSERT INTO work_schedule_signups (schedule_id, household_id, status) "
                 "VALUES (:schedule_id, :household_id, 'Confirmed')");
    query.bindValue(":schedule_id", scheduleId);
    query.bindValue(":household_id", m_householdId);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", 
                            "Failed to sign up: " + query.lastError().text());
        return;
    }
    
    // Update slots_filled count
    query.prepare("UPDATE work_schedule SET slots_filled = slots_filled + 1 WHERE id = :id");
    query.bindValue(":id", scheduleId);
    query.exec();
    
    QMessageBox::information(this, "Success", "You have been signed up for this work day!");
    refreshData();
}

void VolunteerProfileWidget::cancelSignUp()
{
    if (m_workDaysTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a work day to cancel.");
        return;
    }
    
    int row = m_workDaysTable->currentRow();
    int scheduleId = m_workDaysTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString status = m_workDaysTable->item(row, 5)->text();
    
    if (status != "✓ Signed Up") {
        QMessageBox::information(this, "Not Signed Up", 
                               "You are not signed up for this work day.");
        return;
    }
    
    auto response = QMessageBox::question(this, "Confirm Cancellation",
                                         "Are you sure you want to cancel your sign-up?",
                                         QMessageBox::Yes | QMessageBox::No);
    
    if (response != QMessageBox::Yes) return;
    
    QSqlQuery query;
    query.prepare("DELETE FROM work_schedule_signups WHERE schedule_id = :schedule_id AND household_id = :household_id");
    query.bindValue(":schedule_id", scheduleId);
    query.bindValue(":household_id", m_householdId);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", 
                            "Failed to cancel sign-up: " + query.lastError().text());
        return;
    }
    
    // Update slots_filled count
    query.prepare("UPDATE work_schedule SET slots_filled = slots_filled - 1 WHERE id = :id");
    query.bindValue(":id", scheduleId);
    query.exec();
    
    QMessageBox::information(this, "Cancelled", "Your sign-up has been cancelled.");
    refreshData();
}

void VolunteerProfileWidget::addCertification()
{
    bool ok;
    QString certName = QInputDialog::getText(this, "Add Certification",
                                            "Certification Name:",
                                            QLineEdit::Normal, "", &ok);
    
    if (!ok || certName.trimmed().isEmpty()) return;
    
    // Create a simple dialog for dates
    QDialog dialog(this);
    dialog.setWindowTitle("Certification Dates");
    auto *layout = new QFormLayout(&dialog);
    
    auto *issueDate = new QDateEdit(QDate::currentDate(), &dialog);
    issueDate->setCalendarPopup(true);
    layout->addRow("Issue Date:", issueDate);
    
    auto *expireDate = new QDateEdit(QDate::currentDate().addYears(1), &dialog);
    expireDate->setCalendarPopup(true);
    layout->addRow("Expiration Date:", expireDate);
    
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttonBox);
    
    if (dialog.exec() != QDialog::Accepted) return;
    
    QSqlQuery query;
    query.prepare("INSERT INTO volunteer_certifications (household_id, certification_name, issue_date, expiration_date) "
                 "VALUES (:household_id, :name, :issue, :expire)");
    query.bindValue(":household_id", m_householdId);
    query.bindValue(":name", certName.trimmed());
    query.bindValue(":issue", issueDate->date().toString(Qt::ISODate));
    query.bindValue(":expire", expireDate->date().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", 
                            "Failed to add certification: " + query.lastError().text());
        return;
    }
    
    QMessageBox::information(this, "Success", "Certification added successfully!");
    loadCertifications();
}

void VolunteerProfileWidget::refreshData()
{
    loadVolunteerProfile();
    loadVolunteerHours();
    loadCertifications();
    loadAvailableWorkDays();
}

