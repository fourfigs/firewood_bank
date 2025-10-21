#include "IncomeDialog.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

IncomeDialog::IncomeDialog(QWidget *parent)
    : QDialog(parent), m_isEditMode(false)
{
    setupUI();
    loadSources();
    setWindowTitle("Add New Income");
}

IncomeDialog::IncomeDialog(int incomeId, QWidget *parent)
    : QDialog(parent), m_incomeId(incomeId), m_isEditMode(true)
{
    setupUI();
    loadSources();
    loadIncomeData(incomeId);
    setWindowTitle("Edit Income");
}

void IncomeDialog::setupUI()
{
    setModal(true);
    setFixedSize(500, 550);
    setStyleSheet(AdobeStyles::MAIN_WINDOW);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Header with firewood theme
    auto *headerLabel = new QLabel(AdobeStyles::ICON_MONEY + " Income Entry", this);
    headerLabel->setStyleSheet(
        "font-size: 16pt; font-weight: bold; color: white; "
        "background: " + AdobeStyles::GRADIENT_PRIMARY + "; "
        "padding: 15px; border-radius: 10px; text-align: center;"
    );
    headerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(headerLabel);

    // Form group
    auto *formGroup = new QGroupBox("Income Details", this);
    auto *formLayout = new QFormLayout(formGroup);
    formLayout->setSpacing(15);

    // Date
    m_dateEdit = new QDateEdit(QDate::currentDate(), formGroup);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setStyleSheet(AdobeStyles::LINE_EDIT);
    formLayout->addRow("Date:", m_dateEdit);

    // Source
    m_sourceCombo = new QComboBox(formGroup);
    m_sourceCombo->setStyleSheet(AdobeStyles::COMBO_BOX);
    connect(m_sourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &IncomeDialog::onSourceChanged);
    formLayout->addRow("Source:", m_sourceCombo);

    // Amount
    m_amountSpinBox = new QDoubleSpinBox(formGroup);
    m_amountSpinBox->setRange(0.01, 999999.99);
    m_amountSpinBox->setDecimals(2);
    m_amountSpinBox->setPrefix("$");
    m_amountSpinBox->setStyleSheet(AdobeStyles::LINE_EDIT);
    formLayout->addRow("Amount:", m_amountSpinBox);

    // Description
    m_descriptionEdit = new QLineEdit(formGroup);
    m_descriptionEdit->setPlaceholderText("Brief description of the income...");
    m_descriptionEdit->setStyleSheet(AdobeStyles::LINE_EDIT);
    formLayout->addRow("Description:", m_descriptionEdit);

    // Donor name (for donations)
    m_donorNameEdit = new QLineEdit(formGroup);
    m_donorNameEdit->setPlaceholderText("Name of donor or organization...");
    m_donorNameEdit->setStyleSheet(AdobeStyles::LINE_EDIT);
    formLayout->addRow("Donor/Source Name:", m_donorNameEdit);

    // Tax deductible checkbox
    m_taxDeductibleCheck = new QCheckBox("This income is tax deductible for the donor", formGroup);
    m_taxDeductibleCheck->setChecked(true); // Default to true for non-profits
    m_taxDeductibleCheck->setStyleSheet("QCheckBox { font-size: 10pt; }");
    formLayout->addRow("", m_taxDeductibleCheck);

    // Receipt issued checkbox
    m_receiptIssuedCheck = new QCheckBox("Receipt has been issued to donor", formGroup);
    m_receiptIssuedCheck->setStyleSheet("QCheckBox { font-size: 10pt; }");
    formLayout->addRow("", m_receiptIssuedCheck);

    // Notes
    m_notesEdit = new QTextEdit(formGroup);
    m_notesEdit->setPlaceholderText("Additional notes or details...");
    m_notesEdit->setMaximumHeight(80);
    m_notesEdit->setStyleSheet(AdobeStyles::TEXT_EDIT);
    formLayout->addRow("Notes:", m_notesEdit);

    mainLayout->addWidget(formGroup);

    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelBtn = new QPushButton("❌ Cancel", this);
    m_cancelBtn->setStyleSheet(AdobeStyles::CANCEL_BUTTON);
    m_cancelBtn->setMinimumHeight(40);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(m_cancelBtn);

    m_saveBtn = new QPushButton("💾 Save Income", this);
    m_saveBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    m_saveBtn->setMinimumHeight(40);
    m_saveBtn->setDefault(true);
    connect(m_saveBtn, &QPushButton::clicked, this, &IncomeDialog::accept);
    buttonLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(buttonLayout);

    // Set initial focus
    m_sourceCombo->setFocus();
}

void IncomeDialog::loadSources()
{
    QSqlQuery query;
    query.exec("SELECT category_name FROM budget_categories WHERE category_type = 'income' AND active = 1 ORDER BY category_name");
    
    while (query.next()) {
        QString source = query.value(0).toString();
        m_sourceCombo->addItem(source.toUpper(), source);
    }
    
    if (m_sourceCombo->count() == 0) {
        // Add default sources if none exist
        QStringList defaults = {"donation", "grant", "wood_sales", "fundraiser", "other"};
        for (const QString &source : defaults) {
            m_sourceCombo->addItem(source.toUpper(), source);
        }
    }
}

void IncomeDialog::loadIncomeData(int incomeId)
{
    QSqlQuery query;
    query.prepare("SELECT date, source, amount, description, donor_name, tax_deductible, receipt_issued "
                  "FROM income WHERE id = :id");
    query.bindValue(":id", incomeId);
    
    if (query.exec() && query.next()) {
        m_dateEdit->setDate(QDate::fromString(query.value(0).toString(), Qt::ISODate));
        
        QString source = query.value(1).toString();
        int sourceIndex = m_sourceCombo->findData(source);
        if (sourceIndex >= 0) {
            m_sourceCombo->setCurrentIndex(sourceIndex);
        }
        
        m_amountSpinBox->setValue(query.value(2).toDouble());
        m_descriptionEdit->setText(query.value(3).toString());
        m_donorNameEdit->setText(query.value(4).toString());
        m_taxDeductibleCheck->setChecked(query.value(5).toBool());
        m_receiptIssuedCheck->setChecked(query.value(6).toBool());
    } else {
        QMessageBox::warning(this, "Error", "Could not load income data: " + query.lastError().text());
    }
}

void IncomeDialog::setUsername(const QString &username)
{
    m_username = username;
}

void IncomeDialog::onSourceChanged()
{
    QString currentSource = m_sourceCombo->currentData().toString().toLower();
    
    // Adjust UI based on source type
    if (currentSource == "donation") {
        m_donorNameEdit->setPlaceholderText("Name of donor...");
        m_taxDeductibleCheck->setVisible(true);
        m_receiptIssuedCheck->setVisible(true);
        m_taxDeductibleCheck->setChecked(true);
    } else if (currentSource == "grant") {
        m_donorNameEdit->setPlaceholderText("Grant organization name...");
        m_taxDeductibleCheck->setVisible(false);
        m_receiptIssuedCheck->setVisible(false);
    } else if (currentSource == "wood_sales") {
        m_donorNameEdit->setPlaceholderText("Customer name (optional)...");
        m_taxDeductibleCheck->setVisible(false);
        m_receiptIssuedCheck->setVisible(true);
        m_receiptIssuedCheck->setChecked(false);
    } else if (currentSource == "fundraiser") {
        m_donorNameEdit->setPlaceholderText("Event or fundraiser name...");
        m_taxDeductibleCheck->setVisible(false);
        m_receiptIssuedCheck->setVisible(false);
    } else {
        m_donorNameEdit->setPlaceholderText("Source name...");
        m_taxDeductibleCheck->setVisible(true);
        m_receiptIssuedCheck->setVisible(true);
    }
}

bool IncomeDialog::validateInput()
{
    if (m_sourceCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select an income source.");
        m_sourceCombo->setFocus();
        return false;
    }
    
    if (m_amountSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid amount greater than $0.00.");
        m_amountSpinBox->setFocus();
        return false;
    }
    
    if (m_descriptionEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a description for this income.");
        m_descriptionEdit->setFocus();
        return false;
    }
    
    return true;
}

void IncomeDialog::accept()
{
    if (!validateInput()) {
        return;
    }
    
    saveIncome();
    QDialog::accept();
}

void IncomeDialog::saveIncome()
{
    QSqlQuery query;
    
    if (m_isEditMode) {
        // Update existing income
        query.prepare("UPDATE income SET "
                     "date = :date, source = :source, amount = :amount, "
                     "description = :description, donor_name = :donor, "
                     "tax_deductible = :tax_deductible, receipt_issued = :receipt_issued, "
                     "updated_at = CURRENT_TIMESTAMP "
                     "WHERE id = :id");
        query.bindValue(":id", m_incomeId);
    } else {
        // Insert new income
        query.prepare("INSERT INTO income (date, source, amount, description, donor_name, "
                     "tax_deductible, receipt_issued, created_by) "
                     "VALUES (:date, :source, :amount, :description, :donor, "
                     ":tax_deductible, :receipt_issued, :created_by)");
        query.bindValue(":created_by", m_username);
    }
    
    query.bindValue(":date", m_dateEdit->date().toString(Qt::ISODate));
    query.bindValue(":source", m_sourceCombo->currentData().toString());
    query.bindValue(":amount", m_amountSpinBox->value());
    query.bindValue(":description", m_descriptionEdit->text().trimmed());
    query.bindValue(":donor", m_donorNameEdit->text().trimmed());
    query.bindValue(":tax_deductible", m_taxDeductibleCheck->isChecked() ? 1 : 0);
    query.bindValue(":receipt_issued", m_receiptIssuedCheck->isChecked() ? 1 : 0);
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", 
                             "Failed to save income: " + query.lastError().text());
        return;
    }
    
    QString action = m_isEditMode ? "updated" : "created";
    qDebug() << "Income" << action << "successfully:" << m_descriptionEdit->text();
}
