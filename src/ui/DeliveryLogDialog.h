#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QDateEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class DeliveryLogDialog : public QDialog {
    Q_OBJECT

public:
    explicit DeliveryLogDialog(QWidget *parent = nullptr);
    ~DeliveryLogDialog() = default;

private slots:
    void loadDeliveryLog();
    void applyFilters();
    void exportToCsv();

private:
    void setupUI();
    
    QTableWidget *m_logTable = nullptr;
    QDateEdit *m_startDateEdit = nullptr;
    QDateEdit *m_endDateEdit = nullptr;
    QComboBox *m_driverFilterCombo = nullptr;
    QLabel *m_totalMilesLabel = nullptr;
    QLabel *m_totalCordsLabel = nullptr;
    QLabel *m_deliveryCountLabel = nullptr;
};

