#include "ProfileChangeRequestDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

ProfileChangeRequestDialog::ProfileChangeRequestDialog(const QString &adminUsername, QWidget *parent)
    : QDialog(parent), m_adminUsername(adminUsername)
{
    setWindowTitle("Profile Change Requests");
    resize(900, 600);
    
    setupUI();
    loadRequests();
}

void ProfileChangeRequestDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Title
    auto *titleLabel = new QLabel("<h2>📝 Profile Change Requests</h2>", this);
    mainLayout->addWidget(titleLabel);
    
    // Filter bar
    auto *filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Status Filter:", this));
    
    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItems({"Pending", "Approved", "Rejected", "All"});
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ProfileChangeRequestDialog::filterByStatus);
    filterLayout->addWidget(m_statusFilter);
    
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);
    
    // Requests table
    m_requestsTable = new QTableWidget(this);
    m_requestsTable->setColumnCount(8);
    m_requestsTable->setHorizontalHeaderLabels({
        "ID", "User", "Field", "Old Value", "New Value", "Requested", "Status", "Reviewed By"
    });
    m_requestsTable->horizontalHeader()->setStretchLastSection(true);
    m_requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_requestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_requestsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_requestsTable->setAlternatingRowColors(true);
    m_requestsTable->verticalHeader()->setVisible(false);
    m_requestsTable->hideColumn(0); // Hide ID column
    
    mainLayout->addWidget(m_requestsTable);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    
    m_approveButton = new QPushButton("✅ Approve Request", this);
    m_approveButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; padding: 8px 16px; }");
    connect(m_approveButton, &QPushButton::clicked, this, &ProfileChangeRequestDialog::approveRequest);
    buttonLayout->addWidget(m_approveButton);
    
    m_rejectButton = new QPushButton("❌ Reject Request", this);
    m_rejectButton->setStyleSheet("QPushButton { background-color: #dc3545; color: white; padding: 8px 16px; }");
    connect(m_rejectButton, &QPushButton::clicked, this, &ProfileChangeRequestDialog::rejectRequest);
    buttonLayout->addWidget(m_rejectButton);
    
    buttonLayout->addStretch();
    
    m_refreshButton = new QPushButton("🔄 Refresh", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &ProfileChangeRequestDialog::loadRequests);
    buttonLayout->addWidget(m_refreshButton);
    
    auto *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void ProfileChangeRequestDialog::loadRequests()
{
    m_requestsTable->setRowCount(0);
    
    QString sql = "SELECT pr.id, pr.requested_by, pr.field_name, pr.old_value, pr.new_value, "
                  "pr.request_date, pr.status, pr.reviewed_by "
                  "FROM profile_change_requests pr ";
    
    if (m_currentFilter != "All") {
        sql += "WHERE pr.status = :status ";
    }
    
    sql += "ORDER BY pr.request_date DESC";
    
    QSqlQuery query;
    query.prepare(sql);
    
    if (m_currentFilter != "All") {
        query.bindValue(":status", m_currentFilter);
    }
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to load requests: " + query.lastError().text());
        return;
    }
    
    int row = 0;
    while (query.next()) {
        m_requestsTable->insertRow(row);
        
        // ID (hidden)
        m_requestsTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        
        // Requested By
        m_requestsTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        
        // Field Name
        QString fieldName = query.value(2).toString();
        auto *fieldItem = new QTableWidgetItem(fieldName);
        QFont font = fieldItem->font();
        font.setBold(true);
        fieldItem->setFont(font);
        m_requestsTable->setItem(row, 2, fieldItem);
        
        // Old Value
        QString oldValue = query.value(3).toString();
        m_requestsTable->setItem(row, 3, new QTableWidgetItem(oldValue.isEmpty() ? "(empty)" : oldValue));
        
        // New Value
        QString newValue = query.value(4).toString();
        auto *newValueItem = new QTableWidgetItem(newValue);
        newValueItem->setForeground(QBrush(QColor(0, 128, 0)));
        m_requestsTable->setItem(row, 4, newValueItem);
        
        // Request Date
        QString requestDate = query.value(5).toString();
        QDateTime dt = QDateTime::fromString(requestDate, Qt::ISODate);
        m_requestsTable->setItem(row, 5, new QTableWidgetItem(dt.toString("MM/dd/yy hh:mm")));
        
        // Status
        QString status = query.value(6).toString();
        auto *statusItem = new QTableWidgetItem(status);
        if (status == "Pending") {
            statusItem->setForeground(QBrush(QColor(255, 165, 0))); // Orange
            statusItem->setFont(font);
        } else if (status == "Approved") {
            statusItem->setForeground(QBrush(QColor(0, 128, 0))); // Green
        } else if (status == "Rejected") {
            statusItem->setForeground(QBrush(QColor(220, 53, 69))); // Red
        }
        m_requestsTable->setItem(row, 6, statusItem);
        
        // Reviewed By
        QString reviewedBy = query.value(7).toString();
        m_requestsTable->setItem(row, 7, new QTableWidgetItem(reviewedBy.isEmpty() ? "-" : reviewedBy));
        
        row++;
    }
    
    m_requestsTable->resizeColumnsToContents();
}

void ProfileChangeRequestDialog::approveRequest()
{
    if (m_requestsTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a request to approve.");
        return;
    }
    
    int row = m_requestsTable->currentRow();
    int requestId = m_requestsTable->item(row, 0)->text().toInt();
    QString status = m_requestsTable->item(row, 6)->text();
    
    if (status != "Pending") {
        QMessageBox::information(this, "Already Processed", 
                               "This request has already been " + status.toLower() + ".");
        return;
    }
    
    auto response = QMessageBox::question(this, "Confirm Approval",
                                         "Are you sure you want to approve this change request?",
                                         QMessageBox::Yes | QMessageBox::No);
    
    if (response == QMessageBox::Yes) {
        processRequest(requestId, "Approved");
    }
}

void ProfileChangeRequestDialog::rejectRequest()
{
    if (m_requestsTable->currentRow() < 0) {
        QMessageBox::information(this, "No Selection", "Please select a request to reject.");
        return;
    }
    
    int row = m_requestsTable->currentRow();
    int requestId = m_requestsTable->item(row, 0)->text().toInt();
    QString status = m_requestsTable->item(row, 6)->text();
    
    if (status != "Pending") {
        QMessageBox::information(this, "Already Processed", 
                               "This request has already been " + status.toLower() + ".");
        return;
    }
    
    auto response = QMessageBox::question(this, "Confirm Rejection",
                                         "Are you sure you want to reject this change request?",
                                         QMessageBox::Yes | QMessageBox::No);
    
    if (response == QMessageBox::Yes) {
        processRequest(requestId, "Rejected");
    }
}

void ProfileChangeRequestDialog::processRequest(int requestId, const QString &action)
{
    // Get request details
    QSqlQuery getRequest;
    getRequest.prepare("SELECT user_id, field_name, new_value FROM profile_change_requests WHERE id = :id");
    getRequest.bindValue(":id", requestId);
    
    if (!getRequest.exec() || !getRequest.next()) {
        QMessageBox::critical(this, "Error", "Failed to load request details: " + getRequest.lastError().text());
        return;
    }
    
    int userId = getRequest.value(0).toInt();
    QString fieldName = getRequest.value(1).toString();
    QString newValue = getRequest.value(2).toString();
    
    // If approving, update the user's profile
    if (action == "Approved") {
        QSqlQuery updateUser;
        QString sql = QString("UPDATE users SET %1 = :new_value WHERE id = :user_id").arg(fieldName);
        updateUser.prepare(sql);
        updateUser.bindValue(":new_value", newValue);
        updateUser.bindValue(":user_id", userId);
        
        if (!updateUser.exec()) {
            QMessageBox::critical(this, "Error", 
                                "Failed to update user profile: " + updateUser.lastError().text());
            return;
        }
    }
    
    // Update request status
    QSqlQuery updateRequest;
    updateRequest.prepare("UPDATE profile_change_requests SET status = :status, "
                         "reviewed_by = :reviewed_by, reviewed_date = :reviewed_date "
                         "WHERE id = :id");
    updateRequest.bindValue(":status", action);
    updateRequest.bindValue(":reviewed_by", m_adminUsername);
    updateRequest.bindValue(":reviewed_date", QDateTime::currentDateTime().toString(Qt::ISODate));
    updateRequest.bindValue(":id", requestId);
    
    if (!updateRequest.exec()) {
        QMessageBox::critical(this, "Error", 
                            "Failed to update request: " + updateRequest.lastError().text());
        return;
    }
    
    QMessageBox::information(this, "Success", 
                           QString("Request has been %1.").arg(action.toLower()));
    loadRequests();
}

void ProfileChangeRequestDialog::filterByStatus(int index)
{
    m_currentFilter = m_statusFilter->itemText(index);
    loadRequests();
}

