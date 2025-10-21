#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

class IncomeDialog : public QDialog {
    Q_OBJECT

public:
    explicit IncomeDialog(QWidget *parent = nullptr);
    explicit IncomeDialog(int incomeId, QWidget *parent = nullptr);
    ~IncomeDialog() = default;

    void setUsername(const QString &username);

private slots:
    void accept() override;
    void onSourceChanged();

private:
    void setupUI();
    void loadSources();
    void loadIncomeData(int incomeId);
    void saveIncome();
    bool validateInput();

    // Form fields
    QDateEdit *m_dateEdit = nullptr;
    QComboBox *m_sourceCombo = nullptr;
    QDoubleSpinBox *m_amountSpinBox = nullptr;
    QLineEdit *m_descriptionEdit = nullptr;
    QLineEdit *m_donorNameEdit = nullptr;
    QCheckBox *m_taxDeductibleCheck = nullptr;
    QCheckBox *m_receiptIssuedCheck = nullptr;
    QTextEdit *m_notesEdit = nullptr;

    // Buttons
    QPushButton *m_saveBtn = nullptr;
    QPushButton *m_cancelBtn = nullptr;

    // Data
    QString m_username;
    int m_incomeId = -1; // -1 for new income, positive for editing
    bool m_isEditMode = false;
};
