#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>

class EmployeeDirectoryDialog : public QDialog {
  Q_OBJECT

public:
  explicit EmployeeDirectoryDialog(QWidget* parent = nullptr);
  ~EmployeeDirectoryDialog() = default;

private slots:
  void loadEmployees();
  void searchEmployees();
  void viewProfile();
  void viewDeliveryHistory();

private:
  void setupUI();

  QTableWidget* m_employeesTable = nullptr;
  QLineEdit* m_searchEdit = nullptr;
  QPushButton* m_viewButton = nullptr;
  QPushButton* m_deliveryHistoryButton = nullptr;
  QPushButton* m_refreshButton = nullptr;
};
