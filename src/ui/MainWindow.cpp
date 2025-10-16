#include "MainWindow.h"
#include "DashboardWidget.h"
#include "ClientDialog.h"
#include <QApplication>
#include <QLabel>
#include <QTabWidget>
#include <QTableView>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(const QString &username, const QString &fullName, 
                       const QString &role, QWidget *parent)
    : QMainWindow(parent), m_username(username), m_fullName(fullName), m_role(role)
{
    qDebug() << "Creating MainWindow...";
    
    logDatabaseStatus();
    loadUserInfo();
    setupUI();
    setupDatabaseModels();
    setupToolbar();
    
    setWindowTitle(QString("Firewood Bank - %1 (%2)").arg(m_fullName).arg(m_role));
    resize(1400, 850);
    
    qDebug() << "MainWindow created successfully";
}

void MainWindow::loadUserInfo()
{
    QSqlQuery query;
    query.prepare("SELECT email, full_name FROM users WHERE username = :username");
    query.bindValue(":username", m_username);
    
    if (query.exec() && query.next()) {
        m_email = query.value(0).toString();
        // Update full name from database if different
        if (!query.value(1).toString().isEmpty()) {
            m_fullName = query.value(1).toString();
        }
        qDebug() << "Loaded user info for:" << m_username;
    } else {
        qDebug() << "WARNING: Could not load user info:" << query.lastError().text();
    }
    
    // TODO: Add phone/contact_number to users table in future migration
    m_contactNumber = "N/A";
}

void MainWindow::setupUI()
{
    qDebug() << "Setting up UI...";
    
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);
    
    // Store tabs for later use
    m_tabs = tabs;
    
    // Add Dashboard as first tab
    UserInfo userInfo;
    userInfo.username = m_username;
    userInfo.fullName = m_fullName;
    userInfo.role = m_role;
    userInfo.contactNumber = m_contactNumber;
    userInfo.email = m_email;
    
    auto *dashboard = new DashboardWidget(userInfo, this);
    m_tabs->addTab(dashboard, "🏠 Dashboard");
}

void MainWindow::setupDatabaseModels()
{
    qDebug() << "Setting up database models...";
    
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "ERROR: Database is not open!";
        QMessageBox::critical(this, "Database Error", 
                             "Database connection is not available. Please check your database setup.");
        return;
    }
    
    // Households tab (editable)
    m_householdsModel = new QSqlTableModel(this, db);
    m_householdsModel->setTable("households");
    m_householdsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    
    if (!m_householdsModel->select()) {
        qDebug() << "ERROR: Failed to select households table:" << m_householdsModel->lastError().text();
        QMessageBox::warning(this, "Database Warning", 
                            "Could not load households data: " + m_householdsModel->lastError().text());
    } else {
        qDebug() << "Households model loaded successfully, rows:" << m_householdsModel->rowCount();
    }

    m_householdsView = new QTableView(this);
    m_householdsView->setModel(m_householdsModel);
    m_householdsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_householdsView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_householdsView->setAlternatingRowColors(true);
    connect(m_householdsView, &QTableView::doubleClicked, this, &MainWindow::onClientDoubleClicked);
    
    // Set user-friendly column headers
    m_householdsModel->setHeaderData(1, Qt::Horizontal, "Name");
    m_householdsModel->setHeaderData(2, Qt::Horizontal, "Phone");
    m_householdsModel->setHeaderData(3, Qt::Horizontal, "Address");
    m_householdsModel->setHeaderData(4, Qt::Horizontal, "Email");
    m_householdsModel->setHeaderData(8, Qt::Horizontal, "Stove Size");
    m_householdsModel->setHeaderData(9, Qt::Horizontal, "Volunteer");
    m_householdsModel->setHeaderData(16, Qt::Horizontal, "Orders");
    
    // Hide some detailed columns for the table view
    m_householdsView->hideColumn(0); // ID
    m_householdsView->hideColumn(5); // mailing_address
    m_householdsView->hideColumn(6); // gate_code
    m_householdsView->hideColumn(7); // notes
    m_householdsView->hideColumn(10); // waiver_signed
    m_householdsView->hideColumn(11); // has_license
    m_householdsView->hideColumn(12); // has_working_vehicle
    m_householdsView->hideColumn(13); // works_for_wood
    m_householdsView->hideColumn(14); // wood_credit_received
    m_householdsView->hideColumn(15); // credit_balance
    m_householdsView->hideColumn(17); // last_volunteer_date
    m_householdsView->hideColumn(18); // last_order_date
    m_householdsView->hideColumn(19); // created_at

    // Inventory tab (read-only)
    m_inventoryModel = new QSqlTableModel(this, db);
    m_inventoryModel->setTable("inventory");
    m_inventoryModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    if (!m_inventoryModel->select()) {
        qDebug() << "ERROR: Failed to select inventory table:" << m_inventoryModel->lastError().text();
        QMessageBox::warning(this, "Database Warning", 
                            "Could not load inventory data: " + m_inventoryModel->lastError().text());
    } else {
        qDebug() << "Inventory model loaded successfully, rows:" << m_inventoryModel->rowCount();
    }

    m_inventoryView = new QTableView(this);
    m_inventoryView->setModel(m_inventoryModel);
    m_inventoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_inventoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_inventoryView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_tabs->addTab(m_householdsView, "Households");
    m_tabs->addTab(m_inventoryView, "Inventory");
}

void MainWindow::setupToolbar()
{
    qDebug() << "Setting up toolbar...";
    
    auto *tb = addToolBar("Main");
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));
    
    auto *addClient = new QAction("➕ Add Client", this);
    addClient->setToolTip("Add a new client/household");
    connect(addClient, &QAction::triggered, this, &MainWindow::addClient);
    tb->addAction(addClient);
    
    auto *editClient = new QAction("✏️ Edit Client", this);
    editClient->setToolTip("Edit selected client");
    connect(editClient, &QAction::triggered, this, &MainWindow::editClient);
    tb->addAction(editClient);
    
    tb->addSeparator();
    
    auto *refreshAction = new QAction("🔄 Refresh", this);
    refreshAction->setToolTip("Refresh data");
    connect(refreshAction, &QAction::triggered, [this]() {
        if (m_householdsModel) m_householdsModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
    });
    tb->addAction(refreshAction);
}

void MainWindow::addClient()
{
    qDebug() << "Opening add client dialog...";
    
    ClientDialog dialog(-1, this);
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Client added successfully";
        if (m_householdsModel) {
            m_householdsModel->select(); // Refresh the view
        }
    }
}

void MainWindow::editClient()
{
    if (!m_householdsView->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "No Selection", "Please select a client to edit.");
        return;
    }
    
    QModelIndex index = m_householdsView->selectionModel()->currentIndex();
    onClientDoubleClicked(index);
}

void MainWindow::onClientDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    int row = index.row();
    QModelIndex idIndex = m_householdsModel->index(row, 0);
    int clientId = m_householdsModel->data(idIndex).toInt();
    
    qDebug() << "Opening edit dialog for client ID:" << clientId;
    
    ClientDialog dialog(clientId, this);
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Client updated successfully";
        if (m_householdsModel) {
            m_householdsModel->select(); // Refresh the view
        }
    }
}

void MainWindow::logDatabaseStatus()
{
    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << "Database connection status:";
    qDebug() << "  - Is open:" << db.isOpen();
    qDebug() << "  - Database name:" << db.databaseName();
    qDebug() << "  - Driver name:" << db.driverName();
    
    if (!db.isOpen()) {
        qDebug() << "  - Last error:" << db.lastError().text();
    }
}

// Provide a simple factory for reuse
QMainWindow* createMainWindow(const QString &username, const QString &fullName, const QString &role) {
    return new MainWindow(username, fullName, role);
}



