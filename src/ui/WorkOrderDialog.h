#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class WorkOrderDialog : public QDialog {
  Q_OBJECT

public:
  explicit WorkOrderDialog(int orderId = -1, QWidget* parent = nullptr);
  ~WorkOrderDialog() = default;

  int getOrderId() const { return m_orderId; }

private:
  void setupUI();
  void loadOrder();
  void loadClients();
  void saveOrder();

  int m_orderId;

  // Order fields
  QComboBox* m_clientCombo = nullptr;
  QPushButton* m_newClientButton = nullptr;
  QDateEdit* m_orderDateEdit = nullptr;
  QDoubleSpinBox* m_requestedCordsEdit = nullptr;
  QDoubleSpinBox* m_deliveredCordsEdit = nullptr;
  QComboBox* m_statusCombo = nullptr;
  QComboBox* m_priorityCombo = nullptr;
  QDateEdit* m_deliveryDateEdit = nullptr;
  QLineEdit* m_deliveryAddressEdit = nullptr;
  QTextEdit* m_deliveryNotesEdit = nullptr;
  QLineEdit* m_assignedDriverEdit = nullptr;
  QComboBox* m_paymentMethodCombo = nullptr;
  QDoubleSpinBox* m_amountPaidEdit = nullptr;
  QTextEdit* m_notesEdit = nullptr;

  // Delivery tracking fields (NEW!)
  QLineEdit* m_deliveryTimeEdit = nullptr;
  QDoubleSpinBox* m_startMileageEdit = nullptr;
  QDoubleSpinBox* m_endMileageEdit = nullptr;

private slots:
  void onStatusChanged(const QString& status);
  void onNewClientClicked();
};
