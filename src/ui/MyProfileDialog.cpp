#include "MyProfileDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

MyProfileDialog::MyProfileDialog(const QString &username, const QString &role, QWidget *parent)
    : QDialog(parent), m_username(username), m_role(role), m_userId(-1)
{
    setWindowTitle("My Profile");
    resize(600, 500);
    
    setupUI();
    loadProfile();
}

void MyProfileDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("<h2>👤 My Profile</h2>", this);
    mainLayout->addWidget(titleLabel);
    
    // Current Information (Read-Only)
    auto *currentBox = new QGroupBox("Current Information", this);
    currentBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #0078d4; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *currentLayout = new QFormLayout(currentBox);
    
    m_usernameLabel = new QLabel(this);
    currentLayout->addRow("<b>Username:</b>", m_usernameLabel);
    
    m_fullNameLabel = new QLabel(this);
    currentLayout->addRow("<b>Full Name:</b>", m_fullNameLabel);
    
    m_roleLabel = new QLabel(this);
    currentLayout->addRow("<b>Role:</b>", m_roleLabel);
    
    m_emailLabel = new QLabel(this);
    currentLayout->addRow("<b>Email:</b>", m_emailLabel);
    
    m_phoneLabel = new QLabel(this);
    currentLayout->addRow("<b>Phone:</b>", m_phoneLabel);
    
    m_availabilityLabel = new QLabel(this);
    m_availabilityLabel->setWordWrap(true);
    currentLayout->addRow("<b>Availability:</b>", m_availabilityLabel);
    
    m_statusLabel = new QLabel(this);
    currentLayout->addRow("<b>Status:</b>", m_statusLabel);
    
    mainLayout->addWidget(currentBox);
    
    // Request Changes Section
    auto *changeBox = new QGroupBox("Request Profile Changes", this);
    changeBox->setStyleSheet(
        "QGroupBox { font-weight: bold; border: 2px solid #28a745; "
        "border-radius: 5px; margin-top: 10px; padding-top: 10px; }"
    );
    auto *changeLayout = new QFormLayout(changeBox);
    
    auto *infoLabel = new QLabel("<i>Submit changes for admin approval</i>", this);
    infoLabel->setStyleSheet("color: #666; font-size: 11px;");
    changeLayout->addRow("", infoLabel);
    
    m_newEmailEdit = new QLineEdit(this);
    m_newEmailEdit->setPlaceholderText("Enter new email address");
    changeLayout->addRow("New Email:", m_newEmailEdit);
    
    m_newPhoneEdit = new QLineEdit(this);
    m_newPhoneEdit->setPlaceholderText("Enter new phone number");
    changeLayout->addRow("New Phone:", m_newPhoneEdit);
    
    m_newAvailabilityEdit = new QTextEdit(this);
    m_newAvailabilityEdit->setMaximumHeight(60);
    m_newAvailabilityEdit->setPlaceholderText("Enter your availability (e.g., Weekdays 9AM-5PM)");
    changeLayout->addRow("New Availability:", m_newAvailabilityEdit);
    
    mainLayout->addWidget(changeBox);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    
    m_submitButton = new QPushButton("📝 Submit Change Request", this);
    m_submitButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; padding: 8px 16px; }");
    connect(m_submitButton, &QPushButton::clicked, this, &MyProfileDialog::requestChange);
    buttonLayout->addWidget(m_submitButton);
    
    buttonLayout->addStretch();
    
    auto *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void MyProfileDialog::loadProfile()
{
    QSqlQuery query;
    query.prepare("SELECT id, username, full_name, role, email, phone, availability, active "
                 "FROM users WHERE username = :username");
    query.bindValue(":username", m_username);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load profile: " + query.lastError().text());
        return;
    }
    
    m_userId = query.value(0).toInt();
    m_usernameLabel->setText(query.value(1).toString());
    m_fullNameLabel->setText(query.value(2).toString());
    m_roleLabel->setText(query.value(3).toString());
    
    QString email = query.value(4).toString();
    m_emailLabel->setText(email.isEmpty() ? "<i>Not set</i>" : email);
    m_newEmailEdit->setText(email);
    
    QString phone = query.value(5).toString();
    m_phoneLabel->setText(phone.isEmpty() ? "<i>Not set</i>" : phone);
    m_newPhoneEdit->setText(phone);
    
    QString availability = query.value(6).toString();
    m_availabilityLabel->setText(availability.isEmpty() ? "<i>Not set</i>" : availability);
    m_newAvailabilityEdit->setPlainText(availability);
    
    bool active = query.value(7).toInt() == 1;
    m_statusLabel->setText(active ? "✓ Active" : "✗ Inactive");
}

void MyProfileDialog::requestChange()
{
    bool hasChanges = false;
    
    // Check email change
    QString currentEmail = m_emailLabel->text();
    if (currentEmail == "<i>Not set</i>") currentEmail = "";
    QString newEmail = m_newEmailEdit->text().trimmed();
    
    if (newEmail != currentEmail && !newEmail.isEmpty()) {
        submitChangeRequest("email", currentEmail, newEmail);
        hasChanges = true;
    }
    
    // Check phone change
    QString currentPhone = m_phoneLabel->text();
    if (currentPhone == "<i>Not set</i>") currentPhone = "";
    QString newPhone = m_newPhoneEdit->text().trimmed();
    
    if (newPhone != currentPhone && !newPhone.isEmpty()) {
        submitChangeRequest("phone", currentPhone, newPhone);
        hasChanges = true;
    }
    
    // Check availability change
    QString currentAvail = m_availabilityLabel->text();
    if (currentAvail == "<i>Not set</i>") currentAvail = "";
    QString newAvail = m_newAvailabilityEdit->toPlainText().trimmed();
    
    if (newAvail != currentAvail && !newAvail.isEmpty()) {
        submitChangeRequest("availability", currentAvail, newAvail);
        hasChanges = true;
    }
    
    if (hasChanges) {
        QMessageBox::information(this, "Success", 
                               "Your change request has been submitted to the administrator for approval.");
        accept();
    } else {
        QMessageBox::information(this, "No Changes", 
                               "No changes detected. Please modify at least one field before submitting.");
    }
}

void MyProfileDialog::submitChangeRequest(const QString &fieldName, const QString &oldValue, const QString &newValue)
{
    QSqlQuery query;
    query.prepare("INSERT INTO profile_change_requests "
                 "(user_id, requested_by, field_name, old_value, new_value, status) "
                 "VALUES (:user_id, :requested_by, :field_name, :old_value, :new_value, 'Pending')");
    
    query.bindValue(":user_id", m_userId);
    query.bindValue(":requested_by", m_username);
    query.bindValue(":field_name", fieldName);
    query.bindValue(":old_value", oldValue);
    query.bindValue(":new_value", newValue);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to submit change request:" << query.lastError().text();
        QMessageBox::warning(this, "Warning", 
                           "Failed to submit change for " + fieldName + ": " + query.lastError().text());
    }
}

