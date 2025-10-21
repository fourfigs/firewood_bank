#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>

class ExpenseDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExpenseDialog(QWidget *parent = nullptr);
    explicit ExpenseDialog(int expenseId, QWidget *parent = nullptr);
    ~ExpenseDialog() = default;

    void setUsername(const QString &username);

private slots:
    void selectReceiptFile();
    void accept() override;

private:
    void setupUI();
    void loadCategories();
    void loadExpenseData(int expenseId);
    void saveExpense();
    bool validateInput();

    // Form fields
    QDateEdit *m_dateEdit = nullptr;
    QComboBox *m_categoryCombo = nullptr;
    QDoubleSpinBox *m_amountSpinBox = nullptr;
    QLineEdit *m_descriptionEdit = nullptr;
    QLineEdit *m_vendorEdit = nullptr;
    QLineEdit *m_receiptPathEdit = nullptr;
    QPushButton *m_selectReceiptBtn = nullptr;
    QComboBox *m_paymentMethodCombo = nullptr;
    QTextEdit *m_notesEdit = nullptr;

    // Buttons
    QPushButton *m_saveBtn = nullptr;
    QPushButton *m_cancelBtn = nullptr;

    // Data
    QString m_username;
    int m_expenseId = -1; // -1 for new expense, positive for editing
    bool m_isEditMode = false;
};
