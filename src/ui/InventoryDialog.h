#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class InventoryDialog : public QDialog {
    Q_OBJECT

public:
    explicit InventoryDialog(int itemId = -1, QWidget *parent = nullptr);
    ~InventoryDialog() = default;

    int getItemId() const { return m_itemId; }

private slots:
    void onCategoryChanged(int index);
    void addNewItem();

private:
    void setupUI();
    void loadItem();
    void loadCategories();
    void saveItem();
    
    int m_itemId;
    
    // UI Components
    QComboBox *m_categoryCombo = nullptr;
    QComboBox *m_itemCombo = nullptr;
    QPushButton *m_addNewItemButton = nullptr;
    QDoubleSpinBox *m_quantityEdit = nullptr;
    QComboBox *m_unitCombo = nullptr;
    QLineEdit *m_locationEdit = nullptr;
    QDoubleSpinBox *m_reorderLevelEdit = nullptr;
    QDoubleSpinBox *m_emergencyLevelEdit = nullptr;
    QTextEdit *m_notesEdit = nullptr;
};

