#include "InventoryDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDateTime>

InventoryDialog::InventoryDialog(int itemId, QWidget *parent)
    : QDialog(parent), m_itemId(itemId)
{
    setWindowTitle(itemId < 0 ? "Add Inventory Item" : "Edit Inventory Item");
    resize(600, 450);
    
    setupUI();
    
    if (m_itemId >= 0) {
        loadItem();
    }
}

void InventoryDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    
    // Item Information Group
    auto *itemGroup = new QGroupBox("Item Information", this);
    auto *itemLayout = new QFormLayout(itemGroup);
    
    // Category selection
    m_categoryCombo = new QComboBox(this);
    loadCategories();
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &InventoryDialog::onCategoryChanged);
    itemLayout->addRow("Category:", m_categoryCombo);
    
    // Item selection with add new button
    auto *itemRow = new QHBoxLayout();
    m_itemCombo = new QComboBox(this);
    m_itemCombo->setEditable(false);
    m_itemCombo->setMinimumWidth(300);
    itemRow->addWidget(m_itemCombo);
    
    m_addNewItemButton = new QPushButton("+ New Item", this);
    m_addNewItemButton->setMaximumWidth(100);
    connect(m_addNewItemButton, &QPushButton::clicked, this, &InventoryDialog::addNewItem);
    itemRow->addWidget(m_addNewItemButton);
    itemRow->addStretch();
    
    itemLayout->addRow("Item:", itemRow);
    
    // Quantity
    m_quantityEdit = new QDoubleSpinBox(this);
    m_quantityEdit->setRange(0.0, 999999.0);
    m_quantityEdit->setSingleStep(1.0);
    m_quantityEdit->setDecimals(2);
    itemLayout->addRow("Quantity:", m_quantityEdit);
    
    // Unit
    m_unitCombo = new QComboBox(this);
    m_unitCombo->addItems({"units", "cords", "gallons", "liters", "pounds", "kilograms"});
    itemLayout->addRow("Unit:", m_unitCombo);
    
    // Location
    m_locationEdit = new QLineEdit(this);
    m_locationEdit->setPlaceholderText("e.g., Storage Shed A, Main Yard");
    itemLayout->addRow("Location:", m_locationEdit);
    
    // Alert Levels
    auto *alertSpacer = new QLabel("<hr><b>📊 Inventory Alerts</b>", this);
    alertSpacer->setStyleSheet("color: #d97732; padding-top: 10px;");
    itemLayout->addRow("", alertSpacer);
    
    m_reorderLevelEdit = new QDoubleSpinBox(this);
    m_reorderLevelEdit->setRange(0.0, 999999.0);
    m_reorderLevelEdit->setSingleStep(1.0);
    m_reorderLevelEdit->setDecimals(2);
    m_reorderLevelEdit->setToolTip("Quantity at which to reorder (yellow alert)");
    m_reorderLevelEdit->setStyleSheet("QDoubleSpinBox { padding: 8px; font-size: 11pt; }");
    itemLayout->addRow("⚠️ Reorder Level:", m_reorderLevelEdit);
    
    m_emergencyLevelEdit = new QDoubleSpinBox(this);
    m_emergencyLevelEdit->setRange(0.0, 999999.0);
    m_emergencyLevelEdit->setSingleStep(1.0);
    m_emergencyLevelEdit->setDecimals(2);
    m_emergencyLevelEdit->setToolTip("Critical quantity level (red alert)");
    m_emergencyLevelEdit->setStyleSheet("QDoubleSpinBox { padding: 8px; font-size: 11pt; }");
    itemLayout->addRow("🚨 Emergency Level:", m_emergencyLevelEdit);
    
    auto *alertHelpLabel = new QLabel(
        "<small><i>Set alert levels to get warnings when stock is low.<br>"
        "Leave at 0 to disable alerts for this item.</i></small>", this
    );
    alertHelpLabel->setWordWrap(true);
    alertHelpLabel->setStyleSheet("color: #6b7280; padding: 5px 0px;");
    itemLayout->addRow("", alertHelpLabel);
    
    mainLayout->addWidget(itemGroup);
    
    // Notes Group
    auto *notesGroup = new QGroupBox("Notes", this);
    auto *notesLayout = new QVBoxLayout(notesGroup);
    
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMaximumHeight(100);
    m_notesEdit->setPlaceholderText("Additional information about this inventory item...");
    notesLayout->addWidget(m_notesEdit);
    
    mainLayout->addWidget(notesGroup);
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    auto *saveButton = new QPushButton("💾 Save", this);
    saveButton->setDefault(true);
    connect(saveButton, &QPushButton::clicked, this, &InventoryDialog::saveItem);
    buttonLayout->addWidget(saveButton);
    
    auto *cancelButton = new QPushButton("❌ Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Trigger initial category change to load items
    if (m_categoryCombo->count() > 0) {
        onCategoryChanged(0);
    }
}

void InventoryDialog::loadCategories()
{
    QSqlQuery query("SELECT id, name FROM inventory_categories ORDER BY name");
    
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        m_categoryCombo->addItem(name, id);
    }
    
    if (m_categoryCombo->count() == 0) {
        QMessageBox::warning(this, "No Categories", 
                           "No inventory categories found. Please contact administrator.");
    }
}

void InventoryDialog::onCategoryChanged(int index)
{
    if (index < 0 || !m_itemCombo) return;
    
    m_itemCombo->clear();
    
    int categoryId = m_categoryCombo->itemData(index).toInt();
    
    QSqlQuery query;
    query.prepare("SELECT DISTINCT item_name FROM inventory_items WHERE category_id = :cat_id ORDER BY item_name");
    query.bindValue(":cat_id", categoryId);
    
    if (query.exec()) {
        while (query.next()) {
            QString itemName = query.value(0).toString();
            m_itemCombo->addItem(itemName);
        }
    }
    
    // If no items exist for this category, show a helpful message
    if (m_itemCombo->count() == 0) {
        m_itemCombo->addItem("(Click + New Item to add)");
        m_itemCombo->setCurrentIndex(0);
    }
}

void InventoryDialog::addNewItem()
{
    bool ok;
    QString itemName = QInputDialog::getText(this, "Add New Item", 
                                            "Enter item name:",
                                            QLineEdit::Normal, "", &ok);
    
    if (!ok || itemName.trimmed().isEmpty()) {
        return;
    }
    
    itemName = itemName.trimmed();
    
    // Check if item already exists in this category
    int categoryId = m_categoryCombo->currentData().toInt();
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM inventory_items WHERE category_id = :cat_id AND item_name = :name");
    checkQuery.bindValue(":cat_id", categoryId);
    checkQuery.bindValue(":name", itemName);
    
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QMessageBox::information(this, "Item Exists", 
                               "This item already exists in the selected category.");
        // Select the existing item
        int index = m_itemCombo->findText(itemName);
        if (index >= 0) {
            m_itemCombo->setCurrentIndex(index);
        }
        return;
    }
    
    // Add to combo box
    if (m_itemCombo->count() == 1 && m_itemCombo->itemText(0).contains("Click + New Item")) {
        m_itemCombo->clear();
    }
    
    m_itemCombo->addItem(itemName);
    m_itemCombo->setCurrentText(itemName);
}

void InventoryDialog::loadItem()
{
    QSqlQuery query;
    query.prepare("SELECT category_id, item_name, quantity, unit, location, notes, "
                 "reorder_level, emergency_level "
                 "FROM inventory_items WHERE id = :id");
    query.bindValue(":id", m_itemId);
    
    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Failed to load inventory item: " + query.lastError().text());
        return;
    }
    
    // Set category
    int categoryId = query.value(0).toInt();
    int catIndex = m_categoryCombo->findData(categoryId);
    if (catIndex >= 0) {
        m_categoryCombo->setCurrentIndex(catIndex);
    }
    
    // Set item name
    QString itemName = query.value(1).toString();
    int itemIndex = m_itemCombo->findText(itemName);
    if (itemIndex >= 0) {
        m_itemCombo->setCurrentIndex(itemIndex);
    } else {
        m_itemCombo->addItem(itemName);
        m_itemCombo->setCurrentText(itemName);
    }
    
    // Set quantity and unit
    m_quantityEdit->setValue(query.value(2).toDouble());
    m_unitCombo->setCurrentText(query.value(3).toString());
    
    // Set location and notes
    m_locationEdit->setText(query.value(4).toString());
    m_notesEdit->setPlainText(query.value(5).toString());
    
    // Load alert levels
    m_reorderLevelEdit->setValue(query.value(6).toDouble());
    m_emergencyLevelEdit->setValue(query.value(7).toDouble());
}

void InventoryDialog::saveItem()
{
    // Validation
    if (m_categoryCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a category.");
        m_categoryCombo->setFocus();
        return;
    }
    
    QString itemName = m_itemCombo->currentText();
    if (itemName.isEmpty() || itemName.contains("Click + New Item")) {
        QMessageBox::warning(this, "Validation Error", "Please select or add an item.");
        m_itemCombo->setFocus();
        return;
    }
    
    int categoryId = m_categoryCombo->currentData().toInt();
    double quantity = m_quantityEdit->value();
    QString unit = m_unitCombo->currentText();
    QString location = m_locationEdit->text();
    QString notes = m_notesEdit->toPlainText();
    double reorderLevel = m_reorderLevelEdit->value();
    double emergencyLevel = m_emergencyLevelEdit->value();
    
    QSqlQuery query;
    
    if (m_itemId < 0) {
        // Check if this exact item already exists
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT id FROM inventory_items WHERE category_id = :cat_id AND item_name = :name");
        checkQuery.bindValue(":cat_id", categoryId);
        checkQuery.bindValue(":name", itemName);
        
        if (checkQuery.exec() && checkQuery.next()) {
            // Item exists, update it instead
            m_itemId = checkQuery.value(0).toInt();
            
            query.prepare("UPDATE inventory_items SET quantity = quantity + :qty, "
                         "unit = :unit, location = :location, notes = :notes, "
                         "reorder_level = :reorder, emergency_level = :emergency, "
                         "last_updated = :updated WHERE id = :id");
            query.bindValue(":qty", quantity);
            query.bindValue(":id", m_itemId);
        } else {
            // Create new item
            query.prepare("INSERT INTO inventory_items (category_id, item_name, quantity, unit, location, notes, reorder_level, emergency_level, last_updated) "
                         "VALUES (:cat_id, :name, :qty, :unit, :location, :notes, :reorder, :emergency, :updated)");
            query.bindValue(":cat_id", categoryId);
            query.bindValue(":name", itemName);
            query.bindValue(":qty", quantity);
        }
    } else {
        // Update existing item
        query.prepare("UPDATE inventory_items SET category_id = :cat_id, item_name = :name, "
                     "quantity = :qty, unit = :unit, location = :location, notes = :notes, "
                     "reorder_level = :reorder, emergency_level = :emergency, "
                     "last_updated = :updated WHERE id = :id");
        query.bindValue(":cat_id", categoryId);
        query.bindValue(":name", itemName);
        query.bindValue(":qty", quantity);
        query.bindValue(":id", m_itemId);
    }
    
    // Bind common values
    query.bindValue(":unit", unit);
    query.bindValue(":location", location);
    query.bindValue(":notes", notes);
    query.bindValue(":reorder", reorderLevel);
    query.bindValue(":emergency", emergencyLevel);
    query.bindValue(":updated", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", 
                            "Failed to save inventory item: " + query.lastError().text());
        qDebug() << "SQL Error:" << query.lastError().text();
        return;
    }
    
    accept();
}

