#include "BookkeepingWidget.h"
#include "StyleSheet.h"
#include "ExpenseDialog.h"
#include "IncomeDialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDate>
#include <QDebug>
#include <QHeaderView>
#include <QSplitter>
#include <QGridLayout>

BookkeepingWidget::BookkeepingWidget(const QString &username, QWidget *parent)
    : QWidget(parent), m_username(username)
{
    setupUI();
    setupModels();
    refreshData();
}

void BookkeepingWidget::setupUI()
{
    setStyleSheet(AdobeStyles::MAIN_WINDOW);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Header with firewood theme
    auto *headerLabel = new QLabel(AdobeStyles::ICON_MONEY + " Financial Management", this);
    headerLabel->setStyleSheet(
        "font-size: 18pt; font-weight: bold; color: white; "
        "padding: 15px; background: " + AdobeStyles::GRADIENT_PRIMARY + "; "
        "border-radius: 10px; margin-bottom: 10px;"
    );
    headerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(headerLabel);
    
    // Create tab widget
    m_tabs = new QTabWidget(this);
    m_tabs->setStyleSheet(AdobeStyles::TAB_WIDGET);
    
    setupExpensesTab();
    setupIncomeTab();
    setupSummaryTab();
    
    mainLayout->addWidget(m_tabs);
    
    // Export button at bottom
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_exportBtn = new QPushButton("📊 Export Financial Report", this);
    m_exportBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    m_exportBtn->setMinimumHeight(40);
    connect(m_exportBtn, &QPushButton::clicked, this, &BookkeepingWidget::exportFinancialReport);
    
    buttonLayout->addWidget(m_exportBtn);
    mainLayout->addLayout(buttonLayout);
}

void BookkeepingWidget::setupExpensesTab()
{
    m_expensesTab = new QWidget();
    auto *layout = new QVBoxLayout(m_expensesTab);
    layout->setSpacing(15);
    
    // Filter controls
    auto *filterGroup = new QGroupBox("Filter Expenses", m_expensesTab);
    auto *filterLayout = new QHBoxLayout(filterGroup);
    
    filterLayout->addWidget(new QLabel("From:"));
    m_expenseFromDate = new QDateEdit(QDate::currentDate().addMonths(-1), filterGroup);
    m_expenseFromDate->setCalendarPopup(true);
    filterLayout->addWidget(m_expenseFromDate);
    
    filterLayout->addWidget(new QLabel("To:"));
    m_expenseToDate = new QDateEdit(QDate::currentDate(), filterGroup);
    m_expenseToDate->setCalendarPopup(true);
    filterLayout->addWidget(m_expenseToDate);
    
    filterLayout->addWidget(new QLabel("Category:"));
    m_expenseCategoryFilter = new QComboBox(filterGroup);
    m_expenseCategoryFilter->addItem("All Categories", "");
    filterLayout->addWidget(m_expenseCategoryFilter);
    
    auto *filterBtn = new QPushButton("🔍 Filter", filterGroup);
    filterBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    connect(filterBtn, &QPushButton::clicked, this, &BookkeepingWidget::filterByDateRange);
    filterLayout->addWidget(filterBtn);
    
    filterLayout->addStretch();
    layout->addWidget(filterGroup);
    
    // Expenses table
    m_expensesView = new QTableView(m_expensesTab);
    m_expensesView->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_expensesView->setAlternatingRowColors(true);
    m_expensesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_expensesView->setSortingEnabled(true);
    connect(m_expensesView, &QTableView::doubleClicked, this, &BookkeepingWidget::onExpenseDoubleClicked);
    
    layout->addWidget(m_expensesView);
    
    // Expense buttons
    auto *expenseButtonLayout = new QHBoxLayout();
    
    m_addExpenseBtn = new QPushButton("➕ Add Expense", m_expensesTab);
    m_addExpenseBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    connect(m_addExpenseBtn, &QPushButton::clicked, this, &BookkeepingWidget::addExpense);
    expenseButtonLayout->addWidget(m_addExpenseBtn);
    
    m_editExpenseBtn = new QPushButton("✏️ Edit Expense", m_expensesTab);
    m_editExpenseBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    connect(m_editExpenseBtn, &QPushButton::clicked, this, &BookkeepingWidget::editExpense);
    expenseButtonLayout->addWidget(m_editExpenseBtn);
    
    m_deleteExpenseBtn = new QPushButton("🗑️ Delete Expense", m_expensesTab);
    m_deleteExpenseBtn->setStyleSheet(AdobeStyles::CANCEL_BUTTON);
    connect(m_deleteExpenseBtn, &QPushButton::clicked, this, &BookkeepingWidget::deleteExpense);
    expenseButtonLayout->addWidget(m_deleteExpenseBtn);
    
    expenseButtonLayout->addStretch();
    layout->addLayout(expenseButtonLayout);
    
    m_tabs->addTab(m_expensesTab, "💸 Expenses");
}

void BookkeepingWidget::setupIncomeTab()
{
    m_incomeTab = new QWidget();
    auto *layout = new QVBoxLayout(m_incomeTab);
    layout->setSpacing(15);
    
    // Filter controls
    auto *filterGroup = new QGroupBox("Filter Income", m_incomeTab);
    auto *filterLayout = new QHBoxLayout(filterGroup);
    
    filterLayout->addWidget(new QLabel("From:"));
    m_incomeFromDate = new QDateEdit(QDate::currentDate().addMonths(-1), filterGroup);
    m_incomeFromDate->setCalendarPopup(true);
    filterLayout->addWidget(m_incomeFromDate);
    
    filterLayout->addWidget(new QLabel("To:"));
    m_incomeToDate = new QDateEdit(QDate::currentDate(), filterGroup);
    m_incomeToDate->setCalendarPopup(true);
    filterLayout->addWidget(m_incomeToDate);
    
    filterLayout->addWidget(new QLabel("Source:"));
    m_incomeSourceFilter = new QComboBox(filterGroup);
    m_incomeSourceFilter->addItem("All Sources", "");
    filterLayout->addWidget(m_incomeSourceFilter);
    
    auto *filterBtn = new QPushButton("🔍 Filter", filterGroup);
    filterBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    connect(filterBtn, &QPushButton::clicked, this, &BookkeepingWidget::filterByDateRange);
    filterLayout->addWidget(filterBtn);
    
    filterLayout->addStretch();
    layout->addWidget(filterGroup);
    
    // Income table
    m_incomeView = new QTableView(m_incomeTab);
    m_incomeView->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_incomeView->setAlternatingRowColors(true);
    m_incomeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_incomeView->setSortingEnabled(true);
    connect(m_incomeView, &QTableView::doubleClicked, this, &BookkeepingWidget::onIncomeDoubleClicked);
    
    layout->addWidget(m_incomeView);
    
    // Income buttons
    auto *incomeButtonLayout = new QHBoxLayout();
    
    m_addIncomeBtn = new QPushButton("➕ Add Income", m_incomeTab);
    m_addIncomeBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    connect(m_addIncomeBtn, &QPushButton::clicked, this, &BookkeepingWidget::addIncome);
    incomeButtonLayout->addWidget(m_addIncomeBtn);
    
    m_editIncomeBtn = new QPushButton("✏️ Edit Income", m_incomeTab);
    m_editIncomeBtn->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    connect(m_editIncomeBtn, &QPushButton::clicked, this, &BookkeepingWidget::editIncome);
    incomeButtonLayout->addWidget(m_editIncomeBtn);
    
    m_deleteIncomeBtn = new QPushButton("🗑️ Delete Income", m_incomeTab);
    m_deleteIncomeBtn->setStyleSheet(AdobeStyles::CANCEL_BUTTON);
    connect(m_deleteIncomeBtn, &QPushButton::clicked, this, &BookkeepingWidget::deleteIncome);
    incomeButtonLayout->addWidget(m_deleteIncomeBtn);
    
    incomeButtonLayout->addStretch();
    layout->addLayout(incomeButtonLayout);
    
    m_tabs->addTab(m_incomeTab, "💰 Income");
}

void BookkeepingWidget::setupSummaryTab()
{
    m_summaryTab = new QWidget();
    auto *layout = new QVBoxLayout(m_summaryTab);
    layout->setSpacing(20);
    
    // Financial Summary Header
    auto *summaryHeader = new QLabel("📊 Financial Summary", m_summaryTab);
    summaryHeader->setStyleSheet(
        "font-size: 16pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + "; "
        "padding: 10px; text-align: center;"
    );
    summaryHeader->setAlignment(Qt::AlignCenter);
    layout->addWidget(summaryHeader);
    
    // Create grid layout for summary cards
    auto *summaryGrid = new QGridLayout();
    summaryGrid->setSpacing(15);
    
    // Total Income/Expenses (All Time)
    auto *totalGroup = new QGroupBox("All Time Totals", m_summaryTab);
    auto *totalLayout = new QGridLayout(totalGroup);
    
    m_totalIncomeLabel = new QLabel("$0.00", totalGroup);
    m_totalIncomeLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + ";");
    m_totalIncomeLabel->setAlignment(Qt::AlignCenter);
    
    m_totalExpensesLabel = new QLabel("$0.00", totalGroup);
    m_totalExpensesLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: " + AdobeStyles::ERROR_RED + ";");
    m_totalExpensesLabel->setAlignment(Qt::AlignCenter);
    
    m_netIncomeLabel = new QLabel("$0.00", totalGroup);
    m_netIncomeLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: " + AdobeStyles::TEXT_PRIMARY + ";");
    m_netIncomeLabel->setAlignment(Qt::AlignCenter);
    
    totalLayout->addWidget(new QLabel("Total Income:"), 0, 0);
    totalLayout->addWidget(m_totalIncomeLabel, 0, 1);
    totalLayout->addWidget(new QLabel("Total Expenses:"), 1, 0);
    totalLayout->addWidget(m_totalExpensesLabel, 1, 1);
    totalLayout->addWidget(new QLabel("Net Income:"), 2, 0);
    totalLayout->addWidget(m_netIncomeLabel, 2, 1);
    
    summaryGrid->addWidget(totalGroup, 0, 0, 1, 2);
    
    // Monthly Summary
    auto *monthlyGroup = new QGroupBox("This Month", m_summaryTab);
    auto *monthlyLayout = new QGridLayout(monthlyGroup);
    
    m_monthlyIncomeLabel = new QLabel("$0.00", monthlyGroup);
    m_monthlyIncomeLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + ";");
    m_monthlyIncomeLabel->setAlignment(Qt::AlignCenter);
    
    m_monthlyExpensesLabel = new QLabel("$0.00", monthlyGroup);
    m_monthlyExpensesLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: " + AdobeStyles::ERROR_RED + ";");
    m_monthlyExpensesLabel->setAlignment(Qt::AlignCenter);
    
    monthlyLayout->addWidget(new QLabel("Income:"), 0, 0);
    monthlyLayout->addWidget(m_monthlyIncomeLabel, 0, 1);
    monthlyLayout->addWidget(new QLabel("Expenses:"), 1, 0);
    monthlyLayout->addWidget(m_monthlyExpensesLabel, 1, 1);
    
    summaryGrid->addWidget(monthlyGroup, 1, 0);
    
    // Yearly Summary
    auto *yearlyGroup = new QGroupBox("This Year", m_summaryTab);
    auto *yearlyLayout = new QGridLayout(yearlyGroup);
    
    m_yearlyIncomeLabel = new QLabel("$0.00", yearlyGroup);
    m_yearlyIncomeLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + ";");
    m_yearlyIncomeLabel->setAlignment(Qt::AlignCenter);
    
    m_yearlyExpensesLabel = new QLabel("$0.00", yearlyGroup);
    m_yearlyExpensesLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: " + AdobeStyles::ERROR_RED + ";");
    m_yearlyExpensesLabel->setAlignment(Qt::AlignCenter);
    
    yearlyLayout->addWidget(new QLabel("Income:"), 0, 0);
    yearlyLayout->addWidget(m_yearlyIncomeLabel, 0, 1);
    yearlyLayout->addWidget(new QLabel("Expenses:"), 1, 0);
    yearlyLayout->addWidget(m_yearlyExpensesLabel, 1, 1);
    
    summaryGrid->addWidget(yearlyGroup, 1, 1);
    
    layout->addLayout(summaryGrid);
    layout->addStretch();
    
    m_tabs->addTab(m_summaryTab, "📊 Summary");
}

void BookkeepingWidget::setupModels()
{
    // Expenses model
    m_expensesModel = new QSqlTableModel(this);
    m_expensesModel->setTable("expenses");
    m_expensesModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_expensesModel->setSort(1, Qt::DescendingOrder); // Sort by date descending
    
    // Set column headers
    m_expensesModel->setHeaderData(0, Qt::Horizontal, "ID");
    m_expensesModel->setHeaderData(1, Qt::Horizontal, "Date");
    m_expensesModel->setHeaderData(2, Qt::Horizontal, "Category");
    m_expensesModel->setHeaderData(3, Qt::Horizontal, "Amount");
    m_expensesModel->setHeaderData(4, Qt::Horizontal, "Description");
    m_expensesModel->setHeaderData(5, Qt::Horizontal, "Vendor");
    m_expensesModel->setHeaderData(6, Qt::Horizontal, "Receipt");
    m_expensesModel->setHeaderData(7, Qt::Horizontal, "Payment Method");
    m_expensesModel->setHeaderData(8, Qt::Horizontal, "Created By");
    
    m_expensesView->setModel(m_expensesModel);
    m_expensesView->hideColumn(0); // Hide ID column
    m_expensesView->hideColumn(9); // Hide created_at
    m_expensesView->hideColumn(10); // Hide updated_at
    
    // Income model
    m_incomeModel = new QSqlTableModel(this);
    m_incomeModel->setTable("income");
    m_incomeModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_incomeModel->setSort(1, Qt::DescendingOrder); // Sort by date descending
    
    // Set column headers
    m_incomeModel->setHeaderData(0, Qt::Horizontal, "ID");
    m_incomeModel->setHeaderData(1, Qt::Horizontal, "Date");
    m_incomeModel->setHeaderData(2, Qt::Horizontal, "Source");
    m_incomeModel->setHeaderData(3, Qt::Horizontal, "Amount");
    m_incomeModel->setHeaderData(4, Qt::Horizontal, "Description");
    m_incomeModel->setHeaderData(5, Qt::Horizontal, "Donor");
    m_incomeModel->setHeaderData(6, Qt::Horizontal, "Tax Deductible");
    m_incomeModel->setHeaderData(7, Qt::Horizontal, "Receipt Issued");
    m_incomeModel->setHeaderData(8, Qt::Horizontal, "Created By");
    
    m_incomeView->setModel(m_incomeModel);
    m_incomeView->hideColumn(0); // Hide ID column
    m_incomeView->hideColumn(9); // Hide created_at
    m_incomeView->hideColumn(10); // Hide updated_at
    
    // Load category filters
    QSqlQuery query;
    if (query.exec("SELECT DISTINCT category_name FROM budget_categories WHERE category_type = 'expense' AND active = 1")) {
        while (query.next()) {
            QString category = query.value(0).toString();
            m_expenseCategoryFilter->addItem(category.toUpper(), category);
        }
    }
    
    if (query.exec("SELECT DISTINCT category_name FROM budget_categories WHERE category_type = 'income' AND active = 1")) {
        while (query.next()) {
            QString source = query.value(0).toString();
            m_incomeSourceFilter->addItem(source.toUpper(), source);
        }
    }
}

void BookkeepingWidget::refreshData()
{
    m_expensesModel->select();
    m_incomeModel->select();
    updateFinancialSummary();
}

void BookkeepingWidget::updateFinancialSummary()
{
    QSqlQuery query;
    
    // Total income and expenses (all time)
    double totalIncome = 0.0, totalExpenses = 0.0;
    
    if (query.exec("SELECT SUM(amount) FROM income")) {
        if (query.next()) {
            totalIncome = query.value(0).toDouble();
        }
    }
    
    if (query.exec("SELECT SUM(amount) FROM expenses")) {
        if (query.next()) {
            totalExpenses = query.value(0).toDouble();
        }
    }
    
    // Monthly totals
    QString currentMonth = QDate::currentDate().toString("yyyy-MM");
    double monthlyIncome = 0.0, monthlyExpenses = 0.0;
    
    query.prepare("SELECT SUM(amount) FROM income WHERE date LIKE :month");
    query.bindValue(":month", currentMonth + "%");
    if (query.exec() && query.next()) {
        monthlyIncome = query.value(0).toDouble();
    }
    
    query.prepare("SELECT SUM(amount) FROM expenses WHERE date LIKE :month");
    query.bindValue(":month", currentMonth + "%");
    if (query.exec() && query.next()) {
        monthlyExpenses = query.value(0).toDouble();
    }
    
    // Yearly totals
    QString currentYear = QDate::currentDate().toString("yyyy");
    double yearlyIncome = 0.0, yearlyExpenses = 0.0;
    
    query.prepare("SELECT SUM(amount) FROM income WHERE date LIKE :year");
    query.bindValue(":year", currentYear + "%");
    if (query.exec() && query.next()) {
        yearlyIncome = query.value(0).toDouble();
    }
    
    query.prepare("SELECT SUM(amount) FROM expenses WHERE date LIKE :year");
    query.bindValue(":year", currentYear + "%");
    if (query.exec() && query.next()) {
        yearlyExpenses = query.value(0).toDouble();
    }
    
    // Update labels
    m_totalIncomeLabel->setText(QString("$%1").arg(totalIncome, 0, 'f', 2));
    m_totalExpensesLabel->setText(QString("$%1").arg(totalExpenses, 0, 'f', 2));
    m_netIncomeLabel->setText(QString("$%1").arg(totalIncome - totalExpenses, 0, 'f', 2));
    
    m_monthlyIncomeLabel->setText(QString("$%1").arg(monthlyIncome, 0, 'f', 2));
    m_monthlyExpensesLabel->setText(QString("$%1").arg(monthlyExpenses, 0, 'f', 2));
    
    m_yearlyIncomeLabel->setText(QString("$%1").arg(yearlyIncome, 0, 'f', 2));
    m_yearlyExpensesLabel->setText(QString("$%1").arg(yearlyExpenses, 0, 'f', 2));
    
    // Update net income color based on positive/negative
    double netIncome = totalIncome - totalExpenses;
    if (netIncome >= 0) {
        m_netIncomeLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + ";");
    } else {
        m_netIncomeLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: " + AdobeStyles::ERROR_RED + ";");
    }
}

void BookkeepingWidget::addExpense()
{
    ExpenseDialog *dialog = new ExpenseDialog(this);
    dialog->setUsername(m_username);
    if (dialog->exec() == QDialog::Accepted) {
        refreshData();
    }
    dialog->deleteLater();
}

void BookkeepingWidget::editExpense()
{
    QModelIndex index = m_expensesView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select an expense to edit.");
        return;
    }
    
    int expenseId = m_expensesModel->data(m_expensesModel->index(index.row(), 0)).toInt();
    ExpenseDialog *dialog = new ExpenseDialog(expenseId, this);
    dialog->setUsername(m_username);
    if (dialog->exec() == QDialog::Accepted) {
        refreshData();
    }
    dialog->deleteLater();
}

void BookkeepingWidget::deleteExpense()
{
    QModelIndex index = m_expensesView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select an expense to delete.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Delete Expense", 
                                   "Are you sure you want to delete this expense?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_expensesModel->removeRow(index.row());
        m_expensesModel->submitAll();
        refreshData();
    }
}

void BookkeepingWidget::addIncome()
{
    IncomeDialog *dialog = new IncomeDialog(this);
    dialog->setUsername(m_username);
    if (dialog->exec() == QDialog::Accepted) {
        refreshData();
    }
    dialog->deleteLater();
}

void BookkeepingWidget::editIncome()
{
    QModelIndex index = m_incomeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select an income entry to edit.");
        return;
    }
    
    int incomeId = m_incomeModel->data(m_incomeModel->index(index.row(), 0)).toInt();
    IncomeDialog *dialog = new IncomeDialog(incomeId, this);
    dialog->setUsername(m_username);
    if (dialog->exec() == QDialog::Accepted) {
        refreshData();
    }
    dialog->deleteLater();
}

void BookkeepingWidget::deleteIncome()
{
    QModelIndex index = m_incomeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select an income entry to delete.");
        return;
    }
    
    int ret = QMessageBox::question(this, "Delete Income", 
                                   "Are you sure you want to delete this income entry?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_incomeModel->removeRow(index.row());
        m_incomeModel->submitAll();
        refreshData();
    }
}

void BookkeepingWidget::exportFinancialReport()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
        "Export Financial Report", 
        QString("financial_report_%1.csv").arg(QDate::currentDate().toString("yyyy-MM-dd")),
        "CSV Files (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Could not open file for writing.");
        return;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "NMERA Firewood Bank - Financial Report\n";
    out << "Generated: " << QDateTime::currentDateTime().toString() << "\n\n";
    
    // Write summary
    out << "FINANCIAL SUMMARY\n";
    out << "Total Income," << m_totalIncomeLabel->text().remove('$') << "\n";
    out << "Total Expenses," << m_totalExpensesLabel->text().remove('$') << "\n";
    out << "Net Income," << m_netIncomeLabel->text().remove('$') << "\n\n";
    
    // Write expenses
    out << "EXPENSES\n";
    out << "Date,Category,Amount,Description,Vendor,Payment Method\n";
    
    QSqlQuery expenseQuery("SELECT date, category, amount, description, vendor, payment_method FROM expenses ORDER BY date DESC");
    while (expenseQuery.next()) {
        out << expenseQuery.value(0).toString() << ","
            << expenseQuery.value(1).toString() << ","
            << expenseQuery.value(2).toString() << ","
            << "\"" << expenseQuery.value(3).toString() << "\","
            << expenseQuery.value(4).toString() << ","
            << expenseQuery.value(5).toString() << "\n";
    }
    
    out << "\nINCOME\n";
    out << "Date,Source,Amount,Description,Donor,Tax Deductible\n";
    
    QSqlQuery incomeQuery("SELECT date, source, amount, description, donor_name, tax_deductible FROM income ORDER BY date DESC");
    while (incomeQuery.next()) {
        out << incomeQuery.value(0).toString() << ","
            << incomeQuery.value(1).toString() << ","
            << incomeQuery.value(2).toString() << ","
            << "\"" << incomeQuery.value(3).toString() << "\","
            << incomeQuery.value(4).toString() << ","
            << (incomeQuery.value(5).toBool() ? "Yes" : "No") << "\n";
    }
    
    file.close();
    
    QMessageBox::information(this, "Export Complete", 
                            QString("Financial report exported to:\n%1").arg(fileName));
}

void BookkeepingWidget::onExpenseDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    editExpense();
}

void BookkeepingWidget::onIncomeDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    editIncome();
}

void BookkeepingWidget::filterByDateRange()
{
    QString fromDate = m_expenseFromDate->date().toString("yyyy-MM-dd");
    QString toDate = m_expenseToDate->date().toString("yyyy-MM-dd");
    QString category = m_expenseCategoryFilter->currentData().toString();
    
    QString filter = QString("date BETWEEN '%1' AND '%2'").arg(fromDate).arg(toDate);
    if (!category.isEmpty()) {
        filter += QString(" AND category = '%1'").arg(category);
    }
    
    m_expensesModel->setFilter(filter);
    
    // Apply similar filter to income
    QString incomeFromDate = m_incomeFromDate->date().toString("yyyy-MM-dd");
    QString incomeToDate = m_incomeToDate->date().toString("yyyy-MM-dd");
    QString source = m_incomeSourceFilter->currentData().toString();
    
    QString incomeFilter = QString("date BETWEEN '%1' AND '%2'").arg(incomeFromDate).arg(incomeToDate);
    if (!source.isEmpty()) {
        incomeFilter += QString(" AND source = '%1'").arg(source);
    }
    
    m_incomeModel->setFilter(incomeFilter);
}

void BookkeepingWidget::filterByCategory()
{
    filterByDateRange(); // Reuse the same logic
}
