#include "ExpenseDialog.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ExpenseDialog::ExpenseDialog(QWidget *parent)
    : QDialog(parent), m_isEditMode(false)
{
    setupUI();
    loadCategories();
    setWindowTitle("Add New Expense");
}

ExpenseDialog::ExpenseDialog(int expenseId, QWidget *parent)
    : QDialog(parent), m_expenseId(expenseId), m_isEditMode(true)
{
    setupUI();
    loadCategories();
    loadExpenseData(expenseId);
    setWindowTitle("Edit Expense");
}

void ExpenseDialog::setupUI()
{
    setModal(true);
    setFixedSize(500, 600);
    setStyleSheet(FirewoodStyles::MAIN_WINDOW);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Header
    auto *headerLabel = new QLabel("💸 Expense Entry", this);
    headerLabel->setStyleSheet(
        "font-size: 16pt; font-weight: bold; color: white; "
        "background: " + FirewoodStyles::GRADIENT_EMBER_TO_FLAME + "; "
        "padding: 15px; border-radius: 8px; text-align: center;"
    );
    headerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(headerLabel);

    // Form group
    auto *formGroup = new QGroupBox("Expense Details", this);
    auto *formLayout = new QFormLayout(formGroup);
    formLayout->setSpacing(15);

    // Date
    m_dateEdit = new QDateEdit(QDate::currentDate(), formGroup);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
    formLayout->addRow("Date:", m_dateEdit);

    // Category
    m_categoryCombo = new QComboBox(formGroup);
    m_categoryCombo->setStyleSheet(FirewoodStyles::COMBO_BOX);
    formLayout->addRow("Category:", m_categoryCombo);

    // Amount
    m_amountSpinBox = new QDoubleSpinBox(formGroup);
    m_amountSpinBox->setRange(0.01, 999999.99);
    m_amountSpinBox->setDecimals(2);
    m_amountSpinBox->setPrefix("$");
    m_amountSpinBox->setStyleSheet(FirewoodStyles::LINE_EDIT);
    formLayout->addRow("Amount:", m_amountSpinBox);

    // Description
    m_descriptionEdit = new QLineEdit(formGroup);
    m_descriptionEdit->setPlaceholderText("Brief description of the expense...");
    m_descriptionEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
    formLayout->addRow("Description:", m_descriptionEdit);

    // Vendor
    m_vendorEdit = new QLineEdit(formGroup);
    m_vendorEdit->setPlaceholderText("Store, company, or person paid...");
    m_vendorEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
    formLayout->addRow("Vendor:", m_vendorEdit);

    // Receipt file
    auto *receiptLayout = new QHBoxLayout();
    m_receiptPathEdit = new QLineEdit(formGroup);
    m_receiptPathEdit->setPlaceholderText("No receipt selected...");
    m_receiptPathEdit->setReadOnly(true);
    m_receiptPathEdit->setStyleSheet(FirewoodStyles::LINE_EDIT);
    
    m_selectReceiptBtn = new QPushButton("📁 Browse", formGroup);
    m_selectReceiptBtn->setStyleSheet(FirewoodStyles::PRIMARY_BUTTON);
    connect(m_selectReceiptBtn, &QPushButton::clicked, this, &ExpenseDialog::selectReceiptFile);
    
    receiptLayout->addWidget(m_receiptPathEdit);
    receiptLayout->addWidget(m_selectReceiptBtn);
    formLayout->addRow("Receipt:", receiptLayout);

    // Payment method
    m_paymentMethodCombo = new QComboBox(formGroup);
    m_paymentMethodCombo->addItems({"cash", "check", "card", "bank_transfer", "other"});
    m_paymentMethodCombo->setStyleSheet(FirewoodStyles::COMBO_BOX);
    formLayout->addRow("Payment Method:", m_paymentMethodCombo);

    // Notes
    m_notesEdit = new QTextEdit(formGroup);
    m_notesEdit->setPlaceholderText("Additional notes or details...");
    m_notesEdit->setMaximumHeight(80);
    m_notesEdit->setStyleSheet(FirewoodStyles::TEXT_EDIT);
    formLayout->addRow("Notes:", m_notesEdit);

    mainLayout->addWidget(formGroup);

    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelBtn = new QPushButton("❌ Cancel", this);
    m_cancelBtn->setStyleSheet(FirewoodStyles::CANCEL_BUTTON);
    m_cancelBtn->setMinimumHeight(40);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(m_cancelBtn);

    m_saveBtn = new QPushButton("💾 Save Expense", this);
    m_saveBtn->setStyleSheet(FirewoodStyles::PRIMARY_BUTTON);
    m_saveBtn->setMinimumHeight(40);
    m_saveBtn->setDefault(true);
    connect(m_saveBtn, &QPushButton::clicked, this, &ExpenseDialog::accept);
    buttonLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(buttonLayout);

    // Set initial focus
    m_categoryCombo->setFocus();
}

void ExpenseDialog::loadCategories()
{
    QSqlQuery query;
    query.exec("SELECT category_name FROM budget_categories WHERE category_type = 'expense' AND active = 1 ORDER BY category_name");
    
    while (query.next()) {
        QString category = query.value(0).toString();
        m_categoryCombo->addItem(category.toUpper(), category);
    }
    
    if (m_categoryCombo->count() == 0) {
        // Add default categories if none exist
        QStringList defaults = {"fuel", "maintenance", "wood_purchase", "admin", "utilities", "insurance", "equipment", "supplies", "other"};
        for (const QString &cat : defaults) {
            m_categoryCombo->addItem(cat.toUpper(), cat);
        }
    }
}

void ExpenseDialog::loadExpenseData(int expenseId)
{
    QSqlQuery query;
    query.prepare("SELECT date, category, amount, description, vendor, receipt_path, payment_method "
                  "FROM expenses WHERE id = :id");
    query.bindValue(":id", expenseId);
    
    if (query.exec() && query.next()) {
        m_dateEdit->setDate(QDate::fromString(query.value(0).toString(), Qt::ISODate));
        
        QString category = query.value(1).toString();
        int categoryIndex = m_categoryCombo->findData(category);
        if (categoryIndex >= 0) {
            m_categoryCombo->setCurrentIndex(categoryIndex);
        }
        
        m_amountSpinBox->setValue(query.value(2).toDouble());
        m_descriptionEdit->setText(query.value(3).toString());
        m_vendorEdit->setText(query.value(4).toString());
        m_receiptPathEdit->setText(query.value(5).toString());
        
        QString paymentMethod = query.value(6).toString();
        int paymentIndex = m_paymentMethodCombo->findText(paymentMethod);
        if (paymentIndex >= 0) {
            m_paymentMethodCombo->setCurrentIndex(paymentIndex);
        }
    } else {
        QMessageBox::warning(this, "Error", "Could not load expense data: " + query.lastError().text());
    }
}

void ExpenseDialog::setUsername(const QString &username)
{
    m_username = username;
}

void ExpenseDialog::selectReceiptFile()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString receiptsPath = documentsPath + "/Firewood Bank/Receipts";
    
    // Create receipts directory if it doesn't exist
    QDir().mkpath(receiptsPath);
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Receipt File",
        receiptsPath,
        "Images (*.png *.jpg *.jpeg *.gif *.bmp);;PDF Files (*.pdf);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        // Copy file to receipts directory with organized naming
        QFileInfo fileInfo(fileName);
        QString newFileName = QString("receipt_%1_%2.%3")
                             .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                             .arg(QTime::currentTime().toString("HHmmss"))
                             .arg(fileInfo.suffix());
        
        QString newFilePath = receiptsPath + "/" + newFileName;
        
        if (QFile::copy(fileName, newFilePath)) {
            m_receiptPathEdit->setText(newFilePath);
        } else {
            // If copy fails, just use the original path
            m_receiptPathEdit->setText(fileName);
        }
    }
}

bool ExpenseDialog::validateInput()
{
    if (m_categoryCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a category.");
        m_categoryCombo->setFocus();
        return false;
    }
    
    if (m_amountSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid amount greater than $0.00.");
        m_amountSpinBox->setFocus();
        return false;
    }
    
    if (m_descriptionEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a description for this expense.");
        m_descriptionEdit->setFocus();
        return false;
    }
    
    return true;
}

void ExpenseDialog::accept()
{
    if (!validateInput()) {
        return;
    }
    
    saveExpense();
    QDialog::accept();
}

void ExpenseDialog::saveExpense()
{
    QSqlQuery query;
    
    if (m_isEditMode) {
        // Update existing expense
        query.prepare("UPDATE expenses SET "
                     "date = :date, category = :category, amount = :amount, "
                     "description = :description, vendor = :vendor, receipt_path = :receipt, "
                     "payment_method = :payment, updated_at = CURRENT_TIMESTAMP "
                     "WHERE id = :id");
        query.bindValue(":id", m_expenseId);
    } else {
        // Insert new expense
        query.prepare("INSERT INTO expenses (date, category, amount, description, vendor, "
                     "receipt_path, payment_method, created_by) "
                     "VALUES (:date, :category, :amount, :description, :vendor, "
                     ":receipt, :payment, :created_by)");
        query.bindValue(":created_by", m_username);
    }
    
    query.bindValue(":date", m_dateEdit->date().toString(Qt::ISODate));
    query.bindValue(":category", m_categoryCombo->currentData().toString());
    query.bindValue(":amount", m_amountSpinBox->value());
    query.bindValue(":description", m_descriptionEdit->text().trimmed());
    query.bindValue(":vendor", m_vendorEdit->text().trimmed());
    query.bindValue(":receipt", m_receiptPathEdit->text().trimmed());
    query.bindValue(":payment", m_paymentMethodCombo->currentText());
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", 
                             "Failed to save expense: " + query.lastError().text());
        return;
    }
    
    QString action = m_isEditMode ? "updated" : "created";
    qDebug() << "Expense" << action << "successfully:" << m_descriptionEdit->text();
}
