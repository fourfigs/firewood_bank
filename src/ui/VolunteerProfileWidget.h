#pragma once

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>

class VolunteerProfileWidget : public QWidget {
    Q_OBJECT

public:
    explicit VolunteerProfileWidget(const QString &username, QWidget *parent = nullptr);
    ~VolunteerProfileWidget() = default;

private slots:
    void loadVolunteerProfile();
    void loadVolunteerHours();
    void loadCertifications();
    void loadAvailableWorkDays();
    void signUpForWorkDay();
    void cancelSignUp();
    void addCertification();
    void refreshData();

private:
    void setupUI();
    int getHouseholdId();
    
    QString m_username;
    int m_householdId = -1;
    
    // Profile Information Labels
    QLabel *m_nameLabel = nullptr;
    QLabel *m_addressLabel = nullptr;
    QLabel *m_mailingAddressLabel = nullptr;
    QLabel *m_phoneLabel = nullptr;
    QLabel *m_emailLabel = nullptr;
    QTextEdit *m_availabilityText = nullptr;
    
    // Status Checkboxes (read-only for volunteer)
    QCheckBox *m_waiverCheck = nullptr;
    QCheckBox *m_licenseCheck = nullptr;
    QCheckBox *m_vehicleCheck = nullptr;
    
    // Tables
    QTableWidget *m_hoursTable = nullptr;
    QTableWidget *m_certificationsTable = nullptr;
    QTableWidget *m_workDaysTable = nullptr;
    
    // Action buttons
    QPushButton *m_signUpButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QPushButton *m_addCertButton = nullptr;
    QPushButton *m_refreshButton = nullptr;
};

