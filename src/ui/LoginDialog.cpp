#include "LoginDialog.h"
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
    setWindowTitle("Firewood Bank - Login");
    setModal(true);
    setFixedSize(400, 250);
}

void LoginDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    
    // Title
    auto *titleLabel = new QLabel("Firewood Bank", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    auto *subtitleLabel = new QLabel("Please log in to continue", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #666;");
    mainLayout->addWidget(subtitleLabel);
    
    mainLayout->addSpacing(10);
    
    // Form layout
    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Enter username");
    m_usernameEdit->setMinimumHeight(30);
    formLayout->addRow("Username:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("Enter password");
    m_passwordEdit->setMinimumHeight(30);
    formLayout->addRow("Password:", m_passwordEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Error label
    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();
    mainLayout->addWidget(m_errorLabel);
    
    mainLayout->addStretch();
    
    // Buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setMinimumHeight(35);
    buttonLayout->addWidget(m_cancelButton);
    
    m_loginButton = new QPushButton("Login", this);
    m_loginButton->setMinimumHeight(35);
    m_loginButton->setDefault(true);
    m_loginButton->setStyleSheet(
        "QPushButton { background-color: #0078d4; color: white; font-weight: bold; }"
        "QPushButton:hover { background-color: #106ebe; }"
    );
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
    query.prepare("SELECT username, role FROM users WHERE username = :username AND password_hash = :password_hash AND active = 1");
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

