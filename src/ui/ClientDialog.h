#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QTabWidget>
#include <QLabel>
#include <QSqlTableModel>

class ClientDialog : public QDialog {
  Q_OBJECT

public:
  explicit ClientDialog(int clientId = -1, QWidget* parent = nullptr);
  ~ClientDialog() = default;

  int getClientId() const { return m_clientId; }
  QString getClientName() const { return m_nameEdit->text(); }

private slots:
  void onSaveClicked();
  void onAddVolunteerHours();
  void onDeleteVolunteerHours();
  void onVolunteerCheckChanged(int state);
  void onWorksForWoodChanged(int state);
  void onOrderHistoryDoubleClicked(const QModelIndex& index);

private:
  void setupUI();
  void createBasicInfoTab();
  void createVolunteerTab();
  void createOrderHistoryTab();
  void loadClientData();
  void loadVolunteerHours();
  void loadOrderHistory();
  void calculateTotals();
  bool saveClientData();

  int m_clientId;
  bool m_isNewClient;

  // Tab widget
  QTabWidget* m_tabs = nullptr;

  // Basic Info fields
  QLineEdit* m_nameEdit = nullptr;
  QLineEdit* m_phoneEdit = nullptr;
  QTextEdit* m_addressEdit = nullptr;
  QLineEdit* m_emailEdit = nullptr;
  QTextEdit* m_mailingAddressEdit = nullptr;
  QLineEdit* m_gateCodeEdit = nullptr;
  QTextEdit* m_notesEdit = nullptr;
  QComboBox* m_stoveSizeCombo = nullptr;

  // Volunteer Info fields
  QCheckBox* m_isVolunteerCheck = nullptr;
  QCheckBox* m_waiverSignedCheck = nullptr;
  QCheckBox* m_hasLicenseCheck = nullptr;
  QCheckBox* m_hasVehicleCheck = nullptr;
  QCheckBox* m_worksForWoodCheck = nullptr;
  QDoubleSpinBox* m_woodCreditSpin = nullptr;
  QDoubleSpinBox* m_creditBalanceSpin = nullptr;
  QTableWidget* m_volunteerHoursTable = nullptr;
  QLabel* m_totalHoursLabel = nullptr;
  QLabel* m_lastVolunteerDateLabel = nullptr;

  // Order History fields
  QLabel* m_orderCountLabel = nullptr;
  QLabel* m_lastOrderDateLabel = nullptr;
  QTableView* m_orderHistoryView = nullptr;
  QSqlTableModel* m_orderHistoryModel = nullptr;

  // Buttons
  QPushButton* m_saveButton = nullptr;
  QPushButton* m_cancelButton = nullptr;
};
