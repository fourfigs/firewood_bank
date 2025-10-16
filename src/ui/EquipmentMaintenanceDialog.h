#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QLabel>

class EquipmentMaintenanceDialog : public QDialog {
    Q_OBJECT

public:
    explicit EquipmentMaintenanceDialog(int equipmentId = -1, QWidget *parent = nullptr);
    ~EquipmentMaintenanceDialog() = default;

    int getEquipmentId() const { return m_equipmentId; }

private slots:
    void updateServiceAlert();
    void recordService();

private:
    void setupUI();
    void loadEquipment();
    void saveEquipment();
    
    int m_equipmentId;
    
    // UI Components
    QLineEdit *m_equipmentNameEdit = nullptr;
    QDoubleSpinBox *m_currentHoursEdit = nullptr;
    QDoubleSpinBox *m_nextServiceHoursEdit = nullptr;
    QDoubleSpinBox *m_alertThresholdEdit = nullptr;
    QDateEdit *m_lastServiceDateEdit = nullptr;
    QTextEdit *m_lastServiceNotesEdit = nullptr;
    QTextEdit *m_notesEdit = nullptr;
    QLabel *m_serviceAlertLabel = nullptr;
};

