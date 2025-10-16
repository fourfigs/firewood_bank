#include "EmployeeDirectoryDialog.h"
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

EmployeeDirectoryDialog::EmployeeDirectoryDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Employee Directory");
    resize(800, 500);
    
    setupUI();
    loadEmployees();
}

void EmployeeDirectoryDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("<h2>📋 Employee Directory</h2>", this);
    mainLayout->addWidget(titleLabel);
    
    // Search bar
    auto *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("Search:", this));
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search by name, email, or phone...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &EmployeeDirectoryDialog::searchEmployees);
    searchLayout->addWidget(m_searchEdit);
    
    mainLayout->addLayout(searchLayout);
    
    // Employees table
    m_employeesTable = new QTableWidget(this);
    m_employeesTable->setColumnCount(6);
    m_employeesTable->setHorizontalHeaderLabels({
        "ID", "Full Name", "Role", "Email", "Phone", "Availability"
    });
    m_employeesTable->horizontalHeader()->setStretchLastSection(true);
    m_employeesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_employeesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_employeesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_employeesTable->setAlternatingRowColors(true);
    m_employeesTable->verticalHeader()->setVisible(false);
    m_employeesTable->hideColumn(0); // Hide ID column
    
    connect(m_employeesTable, &QTableWidget::doubleClicked, this, &EmployeeDirectoryDialog::viewProfile);
    
    mainLayout->addWidget(m_employeesTable);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    
    m_viewButton = new QPushButton("👁️ View Details", this);
    connect(m_viewButton, &QPushButton::clicked, this, &EmployeeDirectoryDialog::viewProfile);
    buttonLayout->addWidget(m_viewButton);
    
    buttonLayout->addStretch();
    
    m_refreshButton = new QPushButton("🔄 Refresh", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &EmployeeDirectoryDialog::loadEmployees);
    buttonLayout->addWidget(m_refreshButton);
    
    auto *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void EmployeeDirectoryDialog::loadEmployees()
{
    m_employeesTable->setRowCount(0);
    
    // Load all employees and admins (active users only)
    QSqlQuery query("SELECT id, full_name, role, email, phone, availability "
                   "FROM users WHERE (role = 'employee' OR role = 'admin') AND active = 1 "
                   "ORDER BY role DESC, full_name");
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to load employees: " + query.lastError().text());
        return;
    }
    
    int row = 0;
    while (query.next()) {
        m_employeesTable->insertRow(row);
        
        // ID (hidden)
        m_employeesTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        
        // Full Name
        m_employeesTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        
        // Role
        QString role = query.value(2).toString();
        auto *roleItem = new QTableWidgetItem(role);
        if (role == "admin") {
            roleItem->setForeground(QBrush(QColor(220, 53, 69))); // Red
            QFont font = roleItem->font();
            font.setBold(true);
            roleItem->setFont(font);
        }
        m_employeesTable->setItem(row, 2, roleItem);
        
        // Email
        QString email = query.value(3).toString();
        m_employeesTable->setItem(row, 3, new QTableWidgetItem(email.isEmpty() ? "-" : email));
        
        // Phone
        QString phone = query.value(4).toString();
        m_employeesTable->setItem(row, 4, new QTableWidgetItem(phone.isEmpty() ? "-" : phone));
        
        // Availability
        QString availability = query.value(5).toString();
        m_employeesTable->setItem(row, 5, new QTableWidgetItem(availability.isEmpty() ? "-" : availability));
        
        row++;
    }
    
    m_employeesTable->resizeColumnsToContents();
}

void EmployeeDirectoryDialog::searchEmployees()
{
    QString searchText = m_searchEdit->text().toLower();
    
    for (int row = 0; row < m_employeesTable->rowCount(); ++row) {
        bool match = false;
        
        // Search in name, email, phone, and availability columns
        for (int col = 1; col <= 5; ++col) {
            QString cellText = m_employeesTable->item(row, col)->text().toLower();
            if (cellText.contains(searchText)) {
                match = true;
                break;
            }
        }
        
        m_employeesTable->setRowHidden(row, !match);
    }
}

void EmployeeDirectoryDialog::viewProfile()
{
    if (m_employeesTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select an employee to view.");
        return;
    }
    
    int row = m_employeesTable->currentRow();
    int userId = m_employeesTable->item(row, 0)->text().toInt();
    
    QSqlQuery query;
    query.prepare("SELECT username, full_name, role, email, phone, availability "
                 "FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load profile: " + query.lastError().text());
        return;
    }
    
    QString username = query.value(0).toString();
    QString fullName = query.value(1).toString();
    QString role = query.value(2).toString();
    QString email = query.value(3).toString();
    QString phone = query.value(4).toString();
    QString availability = query.value(5).toString();
    
    // Create profile dialog
    QDialog profileDialog(this);
    profileDialog.setWindowTitle("Employee Profile - " + fullName);
    profileDialog.resize(450, 300);
    
    auto *layout = new QVBoxLayout(&profileDialog);
    
    auto *profileBox = new QGroupBox("Contact Information", &profileDialog);
    auto *profileLayout = new QFormLayout(profileBox);
    
    profileLayout->addRow("<b>Username:</b>", new QLabel(username, &profileDialog));
    profileLayout->addRow("<b>Full Name:</b>", new QLabel(fullName, &profileDialog));
    profileLayout->addRow("<b>Role:</b>", new QLabel(role, &profileDialog));
    profileLayout->addRow("<b>Email:</b>", new QLabel(email.isEmpty() ? "Not set" : email, &profileDialog));
    profileLayout->addRow("<b>Phone:</b>", new QLabel(phone.isEmpty() ? "Not set" : phone, &profileDialog));
    
    auto *availLabel = new QLabel(availability.isEmpty() ? "Not set" : availability, &profileDialog);
    availLabel->setWordWrap(true);
    profileLayout->addRow("<b>Availability:</b>", availLabel);
    
    layout->addWidget(profileBox);
    
    auto *closeButton = new QPushButton("Close", &profileDialog);
    connect(closeButton, &QPushButton::clicked, &profileDialog, &QDialog::accept);
    layout->addWidget(closeButton);
    
    profileDialog.exec();
}

