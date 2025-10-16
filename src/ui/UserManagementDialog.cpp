#include "UserManagementDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QDateTime>

UserManagementDialog::UserManagementDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("User Management");
    resize(900, 600);
    
    setupUI();
    loadUsers();
}

void UserManagementDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("<h2>👥 User Management</h2>", this);
    mainLayout->addWidget(titleLabel);
    
    // Search bar
    auto *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("Search:", this));
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search by username, full name, or email...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &UserManagementDialog::searchUsers);
    searchLayout->addWidget(m_searchEdit);
    
    mainLayout->addLayout(searchLayout);
    
    // Users table
    m_usersTable = new QTableWidget(this);
    m_usersTable->setColumnCount(7);
    m_usersTable->setHorizontalHeaderLabels({
        "ID", "Username", "Full Name", "Role", "Email", "Status", "Last Login"
    });
    m_usersTable->horizontalHeader()->setStretchLastSection(true);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setAlternatingRowColors(true);
    m_usersTable->verticalHeader()->setVisible(false);
    m_usersTable->hideColumn(0); // Hide ID column
    
    connect(m_usersTable, &QTableWidget::doubleClicked, this, &UserManagementDialog::viewUserProfile);
    
    mainLayout->addWidget(m_usersTable);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    
    m_viewButton = new QPushButton("👁️ View Profile", this);
    connect(m_viewButton, &QPushButton::clicked, this, &UserManagementDialog::viewUserProfile);
    buttonLayout->addWidget(m_viewButton);
    
    m_editButton = new QPushButton("✏️ Edit User", this);
    connect(m_editButton, &QPushButton::clicked, this, &UserManagementDialog::editUser);
    buttonLayout->addWidget(m_editButton);
    
    m_addButton = new QPushButton("➕ Add User", this);
    connect(m_addButton, &QPushButton::clicked, this, &UserManagementDialog::addUser);
    buttonLayout->addWidget(m_addButton);
    
    m_removeButton = new QPushButton("🗑️ Remove User", this);
    m_removeButton->setStyleSheet("QPushButton { background-color: #dc3545; color: white; }");
    connect(m_removeButton, &QPushButton::clicked, this, &UserManagementDialog::removeUser);
    buttonLayout->addWidget(m_removeButton);
    
    buttonLayout->addStretch();
    
    m_refreshButton = new QPushButton("🔄 Refresh", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &UserManagementDialog::loadUsers);
    buttonLayout->addWidget(m_refreshButton);
    
    auto *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void UserManagementDialog::loadUsers()
{
    m_usersTable->setRowCount(0);
    
    QSqlQuery query("SELECT id, username, full_name, role, email, active, last_login "
                   "FROM users ORDER BY role, username");
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to load users: " + query.lastError().text());
        return;
    }
    
    int row = 0;
    while (query.next()) {
        m_usersTable->insertRow(row);
        
        // ID (hidden)
        m_usersTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        
        // Username
        m_usersTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        
        // Full Name
        m_usersTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        
        // Role
        QString role = query.value(3).toString();
        auto *roleItem = new QTableWidgetItem(role);
        if (role == "admin") {
            roleItem->setForeground(QBrush(QColor(220, 53, 69))); // Red
            QFont font = roleItem->font();
            font.setBold(true);
            roleItem->setFont(font);
        } else if (role == "employee") {
            roleItem->setForeground(QBrush(QColor(0, 123, 255))); // Blue
        }
        m_usersTable->setItem(row, 3, roleItem);
        
        // Email
        m_usersTable->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        
        // Status
        bool active = query.value(5).toInt() == 1;
        auto *statusItem = new QTableWidgetItem(active ? "✓ Active" : "✗ Inactive");
        if (!active) {
            statusItem->setForeground(QBrush(QColor(128, 128, 128))); // Gray
        }
        m_usersTable->setItem(row, 5, statusItem);
        
        // Last Login
        QString lastLogin = query.value(6).toString();
        if (!lastLogin.isEmpty()) {
            QDateTime dt = QDateTime::fromString(lastLogin, Qt::ISODate);
            m_usersTable->setItem(row, 6, new QTableWidgetItem(dt.toString("MM/dd/yyyy hh:mm AP")));
        } else {
            m_usersTable->setItem(row, 6, new QTableWidgetItem("Never"));
        }
        
        row++;
    }
    
    m_usersTable->resizeColumnsToContents();
}

void UserManagementDialog::viewUserProfile()
{
    if (m_usersTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a user to view.");
        return;
    }
    
    int row = m_usersTable->currentRow();
    int userId = m_usersTable->item(row, 0)->text().toInt();
    
    QSqlQuery query;
    query.prepare("SELECT username, full_name, role, email, active, created_at, last_login "
                 "FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load user profile: " + query.lastError().text());
        return;
    }
    
    QString username = query.value(0).toString();
    QString fullName = query.value(1).toString();
    QString role = query.value(2).toString();
    QString email = query.value(3).toString();
    bool active = query.value(4).toInt() == 1;
    QString createdAt = query.value(5).toString();
    QString lastLogin = query.value(6).toString();
    
    // Format dates
    QDateTime created = QDateTime::fromString(createdAt, Qt::ISODate);
    QString createdStr = created.toString("MM/dd/yyyy hh:mm AP");
    
    QString lastLoginStr = "Never";
    if (!lastLogin.isEmpty()) {
        QDateTime lastDt = QDateTime::fromString(lastLogin, Qt::ISODate);
        lastLoginStr = lastDt.toString("MM/dd/yyyy hh:mm AP");
    }
    
    // Create profile dialog
    QDialog profileDialog(this);
    profileDialog.setWindowTitle("User Profile - " + username);
    profileDialog.resize(500, 400);
    
    auto *layout = new QVBoxLayout(&profileDialog);
    
    auto *profileBox = new QGroupBox("Profile Information", &profileDialog);
    auto *profileLayout = new QFormLayout(profileBox);
    
    profileLayout->addRow("<b>Username:</b>", new QLabel(username, &profileDialog));
    profileLayout->addRow("<b>Full Name:</b>", new QLabel(fullName, &profileDialog));
    profileLayout->addRow("<b>Role:</b>", new QLabel(role, &profileDialog));
    profileLayout->addRow("<b>Email:</b>", new QLabel(email, &profileDialog));
    profileLayout->addRow("<b>Status:</b>", new QLabel(active ? "Active" : "Inactive", &profileDialog));
    profileLayout->addRow("<b>Created:</b>", new QLabel(createdStr, &profileDialog));
    profileLayout->addRow("<b>Last Login:</b>", new QLabel(lastLoginStr, &profileDialog));
    
    layout->addWidget(profileBox);
    
    // If user has a household record (is a volunteer), show that too
    QSqlQuery householdQuery;
    householdQuery.prepare("SELECT name, phone, email, address FROM households WHERE name = :username");
    householdQuery.bindValue(":username", username);
    
    if (householdQuery.exec() && householdQuery.next()) {
        auto *volunteerBox = new QGroupBox("Volunteer Information", &profileDialog);
        auto *volunteerLayout = new QFormLayout(volunteerBox);
        
        volunteerLayout->addRow("<b>Name:</b>", new QLabel(householdQuery.value(0).toString(), &profileDialog));
        volunteerLayout->addRow("<b>Phone:</b>", new QLabel(householdQuery.value(1).toString(), &profileDialog));
        volunteerLayout->addRow("<b>Email:</b>", new QLabel(householdQuery.value(2).toString(), &profileDialog));
        volunteerLayout->addRow("<b>Address:</b>", new QLabel(householdQuery.value(3).toString(), &profileDialog));
        
        layout->addWidget(volunteerBox);
    }
    
    auto *buttonBox = new QPushButton("Close", &profileDialog);
    connect(buttonBox, &QPushButton::clicked, &profileDialog, &QDialog::accept);
    layout->addWidget(buttonBox);
    
    profileDialog.exec();
}

void UserManagementDialog::editUser()
{
    if (m_usersTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a user to edit.");
        return;
    }
    
    int row = m_usersTable->currentRow();
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString currentUsername = m_usersTable->item(row, 1)->text();
    
    QSqlQuery query;
    query.prepare("SELECT username, full_name, role, email, active FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load user: " + query.lastError().text());
        return;
    }
    
    QString username = query.value(0).toString();
    QString fullName = query.value(1).toString();
    QString role = query.value(2).toString();
    QString email = query.value(3).toString();
    bool active = query.value(4).toInt() == 1;
    
    // Create edit dialog
    QDialog editDialog(this);
    editDialog.setWindowTitle("Edit User - " + username);
    editDialog.resize(400, 300);
    
    auto *layout = new QFormLayout(&editDialog);
    
    auto *usernameEdit = new QLineEdit(username, &editDialog);
    layout->addRow("Username:", usernameEdit);
    
    auto *fullNameEdit = new QLineEdit(fullName, &editDialog);
    layout->addRow("Full Name:", fullNameEdit);
    
    auto *roleCombo = new QComboBox(&editDialog);
    roleCombo->addItems({"admin", "employee", "volunteer"});
    roleCombo->setCurrentText(role);
    layout->addRow("Role:", roleCombo);
    
    auto *emailEdit = new QLineEdit(email, &editDialog);
    layout->addRow("Email:", emailEdit);
    
    auto *activeCheck = new QCheckBox("Active", &editDialog);
    activeCheck->setChecked(active);
    layout->addRow("Status:", activeCheck);
    
    auto *passwordLabel = new QLabel("<i>Leave blank to keep current password</i>", &editDialog);
    layout->addRow("", passwordLabel);
    
    auto *passwordEdit = new QLineEdit(&editDialog);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("New password (optional)");
    layout->addRow("New Password:", passwordEdit);
    
    auto *buttonBox = new QHBoxLayout();
    auto *saveButton = new QPushButton("💾 Save", &editDialog);
    auto *cancelButton = new QPushButton("Cancel", &editDialog);
    
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString newUsername = usernameEdit->text().trimmed();
        QString newFullName = fullNameEdit->text().trimmed();
        QString newRole = roleCombo->currentText();
        QString newEmail = emailEdit->text().trimmed();
        bool newActive = activeCheck->isChecked();
        QString newPassword = passwordEdit->text();
        
        if (newUsername.isEmpty()) {
            QMessageBox::warning(&editDialog, "Validation Error", "Username cannot be empty.");
            return;
        }
        
        QSqlQuery updateQuery;
        QString sql = "UPDATE users SET username = :username, full_name = :full_name, "
                     "role = :role, email = :email, active = :active";
        
        if (!newPassword.isEmpty()) {
            sql += ", password_hash = :password_hash";
        }
        
        sql += " WHERE id = :id";
        
        updateQuery.prepare(sql);
        updateQuery.bindValue(":username", newUsername);
        updateQuery.bindValue(":full_name", newFullName);
        updateQuery.bindValue(":role", newRole);
        updateQuery.bindValue(":email", newEmail);
        updateQuery.bindValue(":active", newActive ? 1 : 0);
        updateQuery.bindValue(":id", userId);
        
        if (!newPassword.isEmpty()) {
            QByteArray passwordHash = QCryptographicHash::hash(
                newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
            updateQuery.bindValue(":password_hash", QString::fromLatin1(passwordHash));
        }
        
        if (!updateQuery.exec()) {
            QMessageBox::critical(&editDialog, "Error", 
                                "Failed to update user: " + updateQuery.lastError().text());
            return;
        }
        
        QMessageBox::information(&editDialog, "Success", "User updated successfully!");
        editDialog.accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, &editDialog, &QDialog::reject);
    
    buttonBox->addWidget(saveButton);
    buttonBox->addWidget(cancelButton);
    layout->addRow(buttonBox);
    
    if (editDialog.exec() == QDialog::Accepted) {
        loadUsers();
    }
}

void UserManagementDialog::addUser()
{
    // Create add user dialog
    QDialog addDialog(this);
    addDialog.setWindowTitle("Add New User");
    addDialog.resize(400, 300);
    
    auto *layout = new QFormLayout(&addDialog);
    
    auto *usernameEdit = new QLineEdit(&addDialog);
    layout->addRow("Username:", usernameEdit);
    
    auto *fullNameEdit = new QLineEdit(&addDialog);
    layout->addRow("Full Name:", fullNameEdit);
    
    auto *roleCombo = new QComboBox(&addDialog);
    roleCombo->addItems({"volunteer", "employee", "admin"});
    layout->addRow("Role:", roleCombo);
    
    auto *emailEdit = new QLineEdit(&addDialog);
    layout->addRow("Email:", emailEdit);
    
    auto *passwordEdit = new QLineEdit(&addDialog);
    passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addRow("Password:", passwordEdit);
    
    auto *buttonBox = new QHBoxLayout();
    auto *saveButton = new QPushButton("💾 Create User", &addDialog);
    auto *cancelButton = new QPushButton("Cancel", &addDialog);
    
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString username = usernameEdit->text().trimmed();
        QString fullName = fullNameEdit->text().trimmed();
        QString role = roleCombo->currentText();
        QString email = emailEdit->text().trimmed();
        QString password = passwordEdit->text();
        
        if (username.isEmpty()) {
            QMessageBox::warning(&addDialog, "Validation Error", "Username cannot be empty.");
            return;
        }
        
        if (password.isEmpty()) {
            QMessageBox::warning(&addDialog, "Validation Error", "Password cannot be empty.");
            return;
        }
        
        // Hash password
        QByteArray passwordHash = QCryptographicHash::hash(
            password.toUtf8(), QCryptographicHash::Sha256).toHex();
        
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (username, password_hash, role, full_name, email, active) "
                          "VALUES (:username, :password_hash, :role, :full_name, :email, 1)");
        insertQuery.bindValue(":username", username);
        insertQuery.bindValue(":password_hash", QString::fromLatin1(passwordHash));
        insertQuery.bindValue(":role", role);
        insertQuery.bindValue(":full_name", fullName);
        insertQuery.bindValue(":email", email);
        
        if (!insertQuery.exec()) {
            QMessageBox::critical(&addDialog, "Error", 
                                "Failed to create user: " + insertQuery.lastError().text());
            return;
        }
        
        QMessageBox::information(&addDialog, "Success", 
                               QString("User '%1' created successfully!").arg(username));
        addDialog.accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, &addDialog, &QDialog::reject);
    
    buttonBox->addWidget(saveButton);
    buttonBox->addWidget(cancelButton);
    layout->addRow(buttonBox);
    
    if (addDialog.exec() == QDialog::Accepted) {
        loadUsers();
    }
}

void UserManagementDialog::removeUser()
{
    if (m_usersTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a user to remove.");
        return;
    }
    
    int row = m_usersTable->currentRow();
    int userId = m_usersTable->item(row, 0)->text().toInt();
    QString username = m_usersTable->item(row, 1)->text();
    
    auto response = QMessageBox::question(this, "Confirm Removal",
                                         QString("Are you sure you want to remove user '%1'?\n\n"
                                                "This will deactivate the account but not delete the user record.")
                                         .arg(username),
                                         QMessageBox::Yes | QMessageBox::No);
    
    if (response != QMessageBox::Yes) return;
    
    // Deactivate instead of delete to preserve data integrity
    QSqlQuery query;
    query.prepare("UPDATE users SET active = 0 WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", 
                            "Failed to remove user: " + query.lastError().text());
        return;
    }
    
    QMessageBox::information(this, "Success", 
                           QString("User '%1' has been deactivated.").arg(username));
    loadUsers();
}

void UserManagementDialog::searchUsers()
{
    QString searchText = m_searchEdit->text().toLower();
    
    for (int row = 0; row < m_usersTable->rowCount(); ++row) {
        bool match = false;
        
        // Search in username, full name, and email columns
        for (int col = 1; col <= 4; ++col) {
            QString cellText = m_usersTable->item(row, col)->text().toLower();
            if (cellText.contains(searchText)) {
                match = true;
                break;
            }
        }
        
        m_usersTable->setRowHidden(row, !match);
    }
}

