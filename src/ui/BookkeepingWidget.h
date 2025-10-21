#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QDateEdit>
#include <QComboBox>

class BookkeepingWidget : public QWidget {
    Q_OBJECT

public:
    explicit BookkeepingWidget(const QString &username, QWidget *parent = nullptr);
    ~BookkeepingWidget() = default;

public slots:
    void refreshData();
    void updateFinancialSummary();

private slots:
    void addExpense();
    void editExpense();
    void deleteExpense();
    void addIncome();
    void editIncome();
    void deleteIncome();
    void exportFinancialReport();
    void onExpenseDoubleClicked(const QModelIndex &index);
    void onIncomeDoubleClicked(const QModelIndex &index);
    void filterByDateRange();
    void filterByCategory();

private:
    void setupUI();
    void setupExpensesTab();
    void setupIncomeTab();
    void setupSummaryTab();
    void setupModels();
    void updateSummaryWidgets();
    
    // User info
    QString m_username;
    
    // UI Components
    QTabWidget *m_tabs = nullptr;
    
    // Expenses tab
    QWidget *m_expensesTab = nullptr;
    QTableView *m_expensesView = nullptr;
    QPushButton *m_addExpenseBtn = nullptr;
    QPushButton *m_editExpenseBtn = nullptr;
    QPushButton *m_deleteExpenseBtn = nullptr;
    QDateEdit *m_expenseFromDate = nullptr;
    QDateEdit *m_expenseToDate = nullptr;
    QComboBox *m_expenseCategoryFilter = nullptr;
    
    // Income tab
    QWidget *m_incomeTab = nullptr;
    QTableView *m_incomeView = nullptr;
    QPushButton *m_addIncomeBtn = nullptr;
    QPushButton *m_editIncomeBtn = nullptr;
    QPushButton *m_deleteIncomeBtn = nullptr;
    QDateEdit *m_incomeFromDate = nullptr;
    QDateEdit *m_incomeToDate = nullptr;
    QComboBox *m_incomeSourceFilter = nullptr;
    
    // Summary tab
    QWidget *m_summaryTab = nullptr;
    QLabel *m_totalIncomeLabel = nullptr;
    QLabel *m_totalExpensesLabel = nullptr;
    QLabel *m_netIncomeLabel = nullptr;
    QLabel *m_monthlyIncomeLabel = nullptr;
    QLabel *m_monthlyExpensesLabel = nullptr;
    QLabel *m_yearlyIncomeLabel = nullptr;
    QLabel *m_yearlyExpensesLabel = nullptr;
    
    // Export button
    QPushButton *m_exportBtn = nullptr;
    
    // Database Models
    QSqlTableModel *m_expensesModel = nullptr;
    QSqlTableModel *m_incomeModel = nullptr;
};
