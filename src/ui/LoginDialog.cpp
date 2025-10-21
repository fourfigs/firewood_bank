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
    setFixedSize(500, 380);
    
    // Apply warm background
    setStyleSheet(
        "QDialog { background-color: " + FirewoodStyles::SMOKE_WHITE + "; }" +
        FirewoodStyles::LINE_EDIT + 
        FirewoodStyles::PRIMARY_BUTTON +
        FirewoodStyles::CANCEL_BUTTON
    );
}

void LoginDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(25);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    
    // Beautiful header with gradient
    auto *headerLabel = new QLabel(this);
    headerLabel->setText(
        "<div style='text-align: center;'>"
        "<div style='font-size: 20pt; font-weight: bold; margin-bottom: 8px;'>🌲 Firewood Bank 🔥</div>"
        "<div style='font-size: 11pt;'>Northern Mendocino Ecosystem Recovery Alliance</div>"
        "</div>"
    );
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet(
        "background: " + FirewoodStyles::GRADIENT_EMBER_TO_FLAME + "; "
        "color: white; "
        "border-radius: 10px; "
        "padding: 25px 20px; "
        "margin-bottom: 15px;"
    );
    mainLayout->addWidget(headerLabel);
    
    auto *subtitleLabel = new QLabel("Please log in to continue", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: " + FirewoodStyles::CHARCOAL + "; font-size: 11pt; padding: 10px;");
    mainLayout->addWidget(subtitleLabel);
    
    mainLayout->addSpacing(15);
    
    // Form layout with better spacing
    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(22);
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    
    auto *usernameLabel = new QLabel("Username:", this);
    usernameLabel->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::FOREST_GREEN + "; padding-right: 10px;");
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Enter username");
    m_usernameEdit->setMinimumHeight(42);
    formLayout->addRow(usernameLabel, m_usernameEdit);
    
    auto *passwordLabel = new QLabel("Password:", this);
    passwordLabel->setStyleSheet("font-size: 11pt; font-weight: bold; color: " + FirewoodStyles::FOREST_GREEN + "; padding-right: 10px;");
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setMinimumHeight(42);
    formLayout->addRow(passwordLabel, m_passwordEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Error label
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet(
        "color: white; "
        "background-color: " + FirewoodStyles::FLAME_RED + "; "
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
    
    m_cancelButton = new QPushButton("❌ Cancel", this);
    m_cancelButton->setProperty("class", "cancel");
    m_cancelButton->setMinimumHeight(45);
    m_cancelButton->setMinimumWidth(140);
    buttonLayout->addWidget(m_cancelButton);
    
    m_loginButton = new QPushButton("🔓 Login", this);
    m_loginButton->setProperty("class", "primary");
    m_loginButton->setMinimumHeight(45);
    m_loginButton->setMinimumWidth(140);
    m_loginButton->setDefault(true);
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
    
    QSqlQuery query(db);
    query.prepare("SELECT username, user_type FROM users WHERE username = :username AND password_hash = :password_hash AND active = 1");
    query.bindValue(":username", username);
    query.bindValue(":password_hash", QString::fromLatin1(passwordHash));
    
    if (!query.exec()) {
        qDebug() << "ERROR: Login query failed:" << query.lastError().text();
        return false;
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

