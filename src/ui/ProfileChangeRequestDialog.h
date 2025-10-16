#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>

class ProfileChangeRequestDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProfileChangeRequestDialog(const QString &adminUsername, QWidget *parent = nullptr);
    ~ProfileChangeRequestDialog() = default;

private slots:
    void loadRequests();
    void approveRequest();
    void rejectRequest();
    void filterByStatus(int index);

private:
    void setupUI();
    void processRequest(int requestId, const QString &action);
    
    QString m_adminUsername;
    
    QTableWidget *m_requestsTable = nullptr;
    QComboBox *m_statusFilter = nullptr;
    QPushButton *m_approveButton = nullptr;
    QPushButton *m_rejectButton = nullptr;
    QPushButton *m_refreshButton = nullptr;
    
    QString m_currentFilter = "Pending";
};

