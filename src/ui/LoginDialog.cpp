#include "LoginDialog.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("🌲 Firewood Bank - Login 🔥");
    setModal(true);
    setFixedSize(650, 500);
    
    // Apply warm background
    setStyleSheet(
        "QDialog { background-color: " + AdobeStyles::SURFACE_BG + "; }" +
        AdobeStyles::LINE_EDIT + 
        AdobeStyles::PRIMARY_BUTTON +
        AdobeStyles::CANCEL_BUTTON
    );
}

void LoginDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    
    // Beautiful header with gradient
    auto *headerLabel = new QLabel(this);
    headerLabel->setText(
        "<div style='text-align: center;'>"
        "<div style='font-size: 20pt; font-weight: bold; margin-bottom: 8px;'>" + AdobeStyles::ICON_TREE + " Northern Mendocino Ecosystem Recovery Alliance " + AdobeStyles::ICON_FIRE + "</div>"
        "<div style='font-size: 14pt; font-weight: 600;'>Community Firewood Bank</div>"
        "</div>"
    );
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet(
        "background: " + AdobeStyles::GRADIENT_PRIMARY + "; "
        "color: white; "
        "border-radius: 10px; "
        "padding: 25px 20px; "
        "margin-bottom: 15px;"
    );
    mainLayout->addWidget(headerLabel);
    
    auto *subtitleLabel = new QLabel("Please log in to continue", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: " + AdobeStyles::TEXT_PRIMARY + "; font-size: 11pt; padding: 8px;");
    mainLayout->addWidget(subtitleLabel);
    
    mainLayout->addSpacing(10);
    
    // Form layout with proper spacing
    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(25);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    formLayout->setVerticalSpacing(25);
    formLayout->setContentsMargins(0, 20, 0, 20);
    
    auto *usernameLabel = new QLabel("Username:", this);
    usernameLabel->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + "; padding-right: 10px;");
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Enter username");
    m_usernameEdit->setStyleSheet(AdobeStyles::LINE_EDIT);
    formLayout->addRow(usernameLabel, m_usernameEdit);
    
    auto *passwordLabel = new QLabel("Password:", this);
    passwordLabel->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + AdobeStyles::SUCCESS_GREEN + "; padding-right: 10px;");
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setStyleSheet(AdobeStyles::LINE_EDIT);
    formLayout->addRow(passwordLabel, m_passwordEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Error label
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet(
        "color: white; "
        "background-color: " + AdobeStyles::ERROR_RED + "; "
        "padding: 12px; "
        "border-radius: 6px; "
        "font-size: 10pt; "
        "font-weight: bold;"
    );
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();
    mainLayout->addWidget(m_errorLabel);
    
    mainLayout->addSpacing(10);
    
    // Buttons with fiery styling
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    
    m_cancelButton = new QPushButton(AdobeStyles::ICON_ERROR + " Cancel", this);
    m_cancelButton->setProperty("class", "cancel");
    m_cancelButton->setMinimumWidth(180);
    m_cancelButton->setStyleSheet(AdobeStyles::CANCEL_BUTTON);
    buttonLayout->addWidget(m_cancelButton);
    
    m_loginButton = new QPushButton(AdobeStyles::ICON_SUCCESS + " Login", this);
    m_loginButton->setProperty("class", "primary");
    m_loginButton->setMinimumWidth(180);
    m_loginButton->setDefault(true);
    m_loginButton->setStyleSheet(AdobeStyles::PRIMARY_BUTTON);
    buttonLayout->addWidget(m_loginButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connections
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    
    // Set initial focus
    m_usernameEdit->setFocus();
}

void LoginDialog::onLoginClicked()
{
    m_errorLabel->hide();
    
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        m_errorLabel->setText("Please enter both username and password");
        m_errorLabel->show();
        return;
    }
    
    if (validateCredentials(username, password)) {
        qDebug() << "Login successful for user:" << username;
        accept();
    } else {
        m_errorLabel->setText("Invalid username or password");
        m_errorLabel->show();
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}

bool LoginDialog::validateCredentials(const QString &username, const QString &password)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "ERROR: Database is not open!";
        m_errorLabel->setText("Database connection error");
        m_errorLabel->show();
        return false;
    }
    
    // Hash the provided password using SHA-256
    QByteArray passwordHash = QCryptographicHash::hash(
        password.toUtf8(), 
        QCryptographicHash::Sha256
    ).toHex();
    
    qDebug() << "Login attempt - Username:" << username;
    qDebug() << "Password hash:" << QString::fromLatin1(passwordHash);
    
    QSqlQuery query(db);
    query.prepare("SELECT username, role FROM users WHERE username = :username AND password_hash = :password_hash AND active = 1");
    query.bindValue(":username", username);
    query.bindValue(":password_hash", QString::fromLatin1(passwordHash));
    
    if (!query.exec()) {
        qDebug() << "ERROR: Login query failed:" << query.lastError().text();
        return false;
    }
    
    // Debug: Check what users exist in the database
    QSqlQuery debugQuery(db);
    if (debugQuery.exec("SELECT username, role, active FROM users")) {
        qDebug() << "Available users in database:";
        while (debugQuery.next()) {
            qDebug() << "  Username:" << debugQuery.value(0).toString() 
                     << "Role:" << debugQuery.value(1).toString() 
                     << "Active:" << debugQuery.value(2).toString();
        }
    }
    
    if (query.next()) {
        m_loggedInUsername = query.value(0).toString();
        m_loggedInRole = query.value(1).toString();
        qDebug() << "User authenticated:" << m_loggedInUsername << "Role:" << m_loggedInRole;
        return true;
    }
    
    return false;
}

QString LoginDialog::getUsername() const
{
    return m_loggedInUsername;
}

QString LoginDialog::getRole() const
{
    return m_loggedInRole;
}

