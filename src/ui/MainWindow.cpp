#include "MainWindow.h"
#include "StyleSheet.h"
#include "DashboardWidget.h"
#include "VolunteerProfileWidget.h"
#include "BookkeepingWidget.h"
#include "ClientDialog.h"
#include "WorkOrderDialog.h"
#include "InventoryDialog.h"
#include "EquipmentMaintenanceDialog.h"
#include "UserManagementDialog.h"
#include "MyProfileDialog.h"
#include "EmployeeDirectoryDialog.h"
#include "ProfileChangeRequestDialog.h"
#include "DeliveryLogDialog.h"
#include "Authorization.h"
#include "database.h"
#include <QApplication>
#include <QLabel>
#include <QTabWidget>
#include <QTableView>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QInputDialog>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDate>
#include <QDebug>
#include <QInputDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QShortcut>
#include <QKeySequence>

using namespace firewood::core;


MainWindow::MainWindow(const QString& username, const QString& fullName,
  const QString& userType, QWidget* parent)
  : QMainWindow(parent), m_username(username), m_fullName(fullName), m_userType(userType)
{
  qDebug() << "Creating MainWindow...";

  logDatabaseStatus();
  loadUserInfo();
  setupUI();

  applyRoleBasedPermissions();

  setupMenuBar();
  setupDatabaseModels();
  setupToolbar();
  
  // Setup status bar
  m_statusBar = new QStatusBar(this);
  setStatusBar(m_statusBar);
  updateStatusBar();
  
  // Setup keyboard shortcuts
  setupKeyboardShortcuts();

  setWindowTitle(QString("Firewood Bank - %1 (%2)").arg(m_fullName).arg(m_userType));

  showMaximized();

  qDebug() << "MainWindow created successfully";
}

void MainWindow::loadUserInfo()
{
  QSqlQuery query;
  query.prepare("SELECT email, full_name FROM users WHERE username = :username");
  query.bindValue(":username", m_username);

  if (query.exec() && query.next()) {
    m_email = query.value(0).toString();
    if (!query.value(1).toString().isEmpty()) {
      m_fullName = query.value(1).toString();
    }
    qDebug() << "Loaded user info for:" << m_username;
  }
  else {
    qDebug() << "WARNING: Could not load user info:" << query.lastError().text();
  }

  m_contactNumber = "N/A";
}

void MainWindow::setupUI()
{
  qDebug() << "Setting up UI...";

  setStyleSheet(AdobeStyles::APPLICATION_STYLE);

  auto* tabs = new QTabWidget(this);
  setCentralWidget(tabs);

  m_tabs = tabs;

  if (Authorization::isVolunteer(m_userType)) {
    auto* volunteerProfile = new VolunteerProfileWidget(m_username, this);
    m_tabs->addTab(volunteerProfile, "👤 My Profile");
  }
  else {
    UserInfo userInfo;
    userInfo.username = m_username;
    userInfo.fullName = m_fullName;
    userInfo.role = m_userType;
    userInfo.contactNumber = m_contactNumber;
    userInfo.email = m_email;

    auto* dashboard = new DashboardWidget(userInfo, this);
    m_tabs->addTab(dashboard, "🏠 Dashboard");
  }
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

  if (Authorization::isVolunteer(m_userType)) {
    qDebug() << "Volunteer role - skipping admin/employee tabs";
    return;
  }

  if (Authorization::hasPermission(m_userType, Authorization::Permission::ViewClients)) {
    m_householdsModel = new QSqlTableModel(this, db);
    m_householdsModel->setTable("users");
    m_householdsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    // Filter to show only clients and volunteers (people who can receive firewood)
    m_householdsModel->setFilter("user_type IN ('client', 'volunteer')");

    if (!m_householdsModel->select()) {
      qDebug() << "ERROR: Failed to select households table:" << m_householdsModel->lastError().text();
    }
    else {
      qDebug() << "Households model loaded successfully, rows:" << m_householdsModel->rowCount();
    }

    m_householdsView = new QTableView(this);
    m_householdsView->setModel(m_householdsModel);
    m_householdsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_householdsView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_householdsView->setAlternatingRowColors(true);
    m_householdsView->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_householdsView->setSortingEnabled(true);
    connect(m_householdsView, &QTableView::doubleClicked, this, &MainWindow::onClientDoubleClicked);

    // Set user-friendly column headers for users table
    m_householdsModel->setHeaderData(4, Qt::Horizontal, "Name");        // full_name
    m_householdsModel->setHeaderData(5, Qt::Horizontal, "Email");       // email
    m_householdsModel->setHeaderData(6, Qt::Horizontal, "Phone");       // phone
    m_householdsModel->setHeaderData(7, Qt::Horizontal, "Address");     // address
    m_householdsModel->setHeaderData(11, Qt::Horizontal, "Stove Size"); // stove_size
    m_householdsModel->setHeaderData(12, Qt::Horizontal, "Volunteer");  // is_volunteer
    m_householdsModel->setHeaderData(23, Qt::Horizontal, "Orders");     // order_count
    m_householdsModel->setHeaderData(2, Qt::Horizontal, "Type");        // user_type
    
    // Hide some detailed columns for the table view
    m_householdsView->hideColumn(0); // ID
    m_householdsView->hideColumn(1); // username
    m_householdsView->hideColumn(3); // role (legacy)
    m_householdsView->hideColumn(8); // mailing_address
    m_householdsView->hideColumn(9); // gate_code
    m_householdsView->hideColumn(10); // notes
    m_householdsView->hideColumn(13); // waiver_signed
    m_householdsView->hideColumn(14); // has_license
    m_householdsView->hideColumn(15); // has_working_vehicle
    m_householdsView->hideColumn(16); // works_for_wood
    m_householdsView->hideColumn(17); // wood_credit_received
    m_householdsView->hideColumn(18); // credit_balance
    m_householdsView->hideColumn(19); // last_volunteer_date
    m_householdsView->hideColumn(20); // last_order_date
    m_householdsView->hideColumn(21); // availability
    m_householdsView->hideColumn(22); // active
    m_householdsView->hideColumn(24); // created_at
    m_householdsView->hideColumn(25); // last_login

    // Create clients tab with search functionality
    auto *clientsTab = new QWidget();
    auto *clientsLayout = new QVBoxLayout(clientsTab);
    clientsLayout->setSpacing(10);
    clientsLayout->setContentsMargins(10, 10, 10, 10);
    
    // Add search box
    auto *searchLayout = new QHBoxLayout();
    auto *searchLabel = new QLabel("🔍 Search Clients:", clientsTab);
    auto *clientSearchBox = new QLineEdit(clientsTab);
    clientSearchBox->setPlaceholderText("Search by name, phone, or address...");
    clientSearchBox->setStyleSheet(AdobeStyles::LINE_EDIT);
    
    connect(clientSearchBox, &QLineEdit::textChanged, this, &MainWindow::searchClients);
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(clientSearchBox);
    searchLayout->addStretch();
    
    clientsLayout->addLayout(searchLayout);
    clientsLayout->addWidget(m_householdsView);
    
    m_tabs->addTab(clientsTab, "👥 Clients");
  }

  if (Authorization::hasPermission(m_userType, Authorization::Permission::ViewInventory)) {
    auto* inventoryRelModel = new QSqlRelationalTableModel(this, db);
    inventoryRelModel->setTable("inventory_items");
    inventoryRelModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    inventoryRelModel->setRelation(1, QSqlRelation("inventory_categories", "id", "name"));

    if (!inventoryRelModel->select()) {
      qDebug() << "ERROR: Failed to select inventory_items table:" << inventoryRelModel->lastError().text();
    }
    else {
      qDebug() << "Inventory model loaded successfully, rows:" << inventoryRelModel->rowCount();
    }

    m_inventoryView = new QTableView(this);
    m_inventoryView->setModel(inventoryRelModel);
    m_inventoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_inventoryView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_inventoryView->setAlternatingRowColors(true);
    m_inventoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_inventoryView->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_inventoryView->setSortingEnabled(true);
    connect(m_inventoryView, &QTableView::doubleClicked, this, &MainWindow::onInventoryDoubleClicked);

    m_inventoryModel = inventoryRelModel;

    // Set user-friendly column headers
    inventoryRelModel->setHeaderData(0, Qt::Horizontal, "ID");
    inventoryRelModel->setHeaderData(1, Qt::Horizontal, "Category");
    inventoryRelModel->setHeaderData(2, Qt::Horizontal, "Item Name");
    inventoryRelModel->setHeaderData(3, Qt::Horizontal, "Quantity");
    inventoryRelModel->setHeaderData(4, Qt::Horizontal, "Unit");
    inventoryRelModel->setHeaderData(5, Qt::Horizontal, "Location");
    inventoryRelModel->setHeaderData(6, Qt::Horizontal, "Notes");
    
    // Hide some columns
    m_inventoryView->hideColumn(0); // ID
    m_inventoryView->hideColumn(7); // last_updated
    m_inventoryView->hideColumn(8); // created_at

    // Create inventory tab with search functionality
    auto *inventoryTab = new QWidget();
    auto *inventoryLayout = new QVBoxLayout(inventoryTab);
    inventoryLayout->setSpacing(10);
    inventoryLayout->setContentsMargins(10, 10, 10, 10);
    
    // Add search box
    auto *invSearchLayout = new QHBoxLayout();
    auto *invSearchLabel = new QLabel("🔍 Search Inventory:", inventoryTab);
    auto *inventorySearchBox = new QLineEdit(inventoryTab);
    inventorySearchBox->setPlaceholderText("Search by item name or category...");
    inventorySearchBox->setStyleSheet(AdobeStyles::LINE_EDIT);
    
    connect(inventorySearchBox, &QLineEdit::textChanged, this, &MainWindow::searchInventory);
    
    invSearchLayout->addWidget(invSearchLabel);
    invSearchLayout->addWidget(inventorySearchBox);
    invSearchLayout->addStretch();
    
    inventoryLayout->addLayout(invSearchLayout);
    inventoryLayout->addWidget(m_inventoryView);
    
    m_tabs->addTab(inventoryTab, "📦 Inventory");
  }

  if (Authorization::hasPermission(m_userType, Authorization::Permission::AddOrders)) {
    m_ordersModel = new QSqlTableModel(this, db);
    m_ordersModel->setTable("orders");
    m_ordersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    // Note: orders still reference household_id for now, will be updated in next migration

    if (!m_ordersModel->select()) {
      qDebug() << "ERROR: Failed to select orders table:" << m_ordersModel->lastError().text();
    }
    else {
      qDebug() << "Orders model loaded successfully, rows:" << m_ordersModel->rowCount();
    }

    m_ordersView = new QTableView(this);
    m_ordersView->setModel(m_ordersModel);
    m_ordersView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersView->setAlternatingRowColors(true);
    m_ordersView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ordersView->setStyleSheet(AdobeStyles::TABLE_VIEW);
    m_ordersView->setSortingEnabled(true);
    connect(m_ordersView, &QTableView::doubleClicked, this, &MainWindow::onWorkOrderDoubleClicked);

    // Set user-friendly column headers
    m_ordersModel->setHeaderData(0, Qt::Horizontal, "ID");
    m_ordersModel->setHeaderData(1, Qt::Horizontal, "Client ID");
    m_ordersModel->setHeaderData(2, Qt::Horizontal, "Order Date");
    m_ordersModel->setHeaderData(3, Qt::Horizontal, "Requested");
    m_ordersModel->setHeaderData(4, Qt::Horizontal, "Delivered");
    m_ordersModel->setHeaderData(5, Qt::Horizontal, "Status");
    m_ordersModel->setHeaderData(6, Qt::Horizontal, "Priority");
    m_ordersModel->setHeaderData(7, Qt::Horizontal, "Delivery Date");
    m_ordersModel->setHeaderData(10, Qt::Horizontal, "Driver");
    m_ordersModel->setHeaderData(11, Qt::Horizontal, "Payment");
    m_ordersModel->setHeaderData(12, Qt::Horizontal, "Amount");
    
    // Hide some detailed columns
    m_ordersView->hideColumn(0); // ID
    m_ordersView->hideColumn(8); // delivery_address
    m_ordersView->hideColumn(9); // delivery_notes
    m_ordersView->hideColumn(13); // notes
    m_ordersView->hideColumn(14); // created_by
    m_ordersView->hideColumn(15); // created_at
    m_ordersView->hideColumn(16); // updated_at
    m_ordersView->hideColumn(17); // delivery_time
    m_ordersView->hideColumn(18); // start_mileage
    m_ordersView->hideColumn(19); // end_mileage
    m_ordersView->hideColumn(20); // completed_date

    // Create orders tab with search functionality
    auto *ordersTab = new QWidget();
    auto *ordersLayout = new QVBoxLayout(ordersTab);
    ordersLayout->setSpacing(10);
    ordersLayout->setContentsMargins(10, 10, 10, 10);
    
    // Add search box
    auto *orderSearchLayout = new QHBoxLayout();
    auto *orderSearchLabel = new QLabel("🔍 Search Orders:", ordersTab);
    auto *orderSearchBox = new QLineEdit(ordersTab);
    orderSearchBox->setPlaceholderText("Search by status, priority, or date...");
    orderSearchBox->setStyleSheet(AdobeStyles::LINE_EDIT);
    
    connect(orderSearchBox, &QLineEdit::textChanged, this, &MainWindow::searchOrders);
    
    orderSearchLayout->addWidget(orderSearchLabel);
    orderSearchLayout->addWidget(orderSearchBox);
    orderSearchLayout->addStretch();
    
    ordersLayout->addLayout(orderSearchLayout);
    ordersLayout->addWidget(m_ordersView);
    
    m_tabs->addTab(ordersTab, "📋 Work Orders");
  }
  
  // Add Bookkeeping tab for Admin and Lead roles
  if (Authorization::isAdmin(m_userType) || Authorization::isLead(m_userType)) {
    auto* bookkeepingWidget = new BookkeepingWidget(m_username, this);
    m_tabs->addTab(bookkeepingWidget, "💰 Bookkeeping");
    qDebug() << "Added Bookkeeping tab for user type:" << m_userType;
  }
}

void MainWindow::searchClients(const QString &text)
{
    if (!m_householdsModel) return;
    
    if (text.isEmpty()) {
        m_householdsModel->setFilter("");
    } else {
        QString filter = QString("name LIKE '%%1%' OR phone LIKE '%%1%' OR address LIKE '%%1%'")
                        .arg(text);
        m_householdsModel->setFilter(filter);
    }
}

void MainWindow::searchOrders(const QString &text)
{
    if (!m_ordersModel) return;
    
    if (text.isEmpty()) {
        m_ordersModel->setFilter("");
    } else {
        QString filter = QString("status LIKE '%%1%' OR priority LIKE '%%1%' OR order_date LIKE '%%1%'")
                        .arg(text);
        m_ordersModel->setFilter(filter);
    }
}

void MainWindow::searchInventory(const QString &text)
{
    if (!m_inventoryModel) return;
    
    if (text.isEmpty()) {
        m_inventoryModel->setFilter("");
    } else {
        QString filter = QString("item_name LIKE '%%1%'").arg(text);
        m_inventoryModel->setFilter(filter);
    }
}

void MainWindow::updateStatusBar()
{
    if (!m_statusBar) return;
    
    QString statusText = QString("📊 System Status - User: %1 (%2)").arg(m_fullName).arg(m_userType);
    
    // Add record counts if models are available
    QStringList counts;
    if (m_householdsModel) {
        counts << QString("Clients: %1").arg(m_householdsModel->rowCount());
    }
    if (m_ordersModel) {
        counts << QString("Orders: %1").arg(m_ordersModel->rowCount());
    }
    if (m_inventoryModel) {
        counts << QString("Inventory Items: %1").arg(m_inventoryModel->rowCount());
    }
    
    if (!counts.isEmpty()) {
        statusText += " | " + counts.join(" | ");
    }
    
    m_statusBar->showMessage(statusText);
}

void MainWindow::setupKeyboardShortcuts()
{
    // Delete key - Delete selected record
    auto *deleteShortcut = new QShortcut(QKeySequence::Delete, this);
    connect(deleteShortcut, &QShortcut::activated, [this]() {
        QWidget *currentTab = m_tabs->currentWidget();
        if (!currentTab) return;
        
        // Determine which tab is active and call appropriate delete function
        QString tabText = m_tabs->tabText(m_tabs->currentIndex());
        if (tabText.contains("Clients") && m_householdsView && m_householdsView->hasFocus()) {
            deleteSelectedClient();
        } else if (tabText.contains("Orders") && m_ordersView && m_ordersView->hasFocus()) {
            deleteSelectedOrder();
        } else if (tabText.contains("Inventory") && m_inventoryView && m_inventoryView->hasFocus()) {
            deleteSelectedInventoryItem();
        }
    });
    
    // Enter key - Edit selected record
    auto *editShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(editShortcut, &QShortcut::activated, [this]() {
        QWidget *currentTab = m_tabs->currentWidget();
        if (!currentTab) return;
        
        QString tabText = m_tabs->tabText(m_tabs->currentIndex());
        if (tabText.contains("Clients") && m_householdsView && m_householdsView->hasFocus()) {
            editClient();
        } else if (tabText.contains("Orders") && m_ordersView && m_ordersView->hasFocus()) {
            editWorkOrder();
        } else if (tabText.contains("Inventory") && m_inventoryView && m_inventoryView->hasFocus()) {
            editInventoryItem();
        }
    });
    
    // Ctrl+N - Add new record
    auto *newShortcut = new QShortcut(QKeySequence::New, this);
    connect(newShortcut, &QShortcut::activated, [this]() {
        QString tabText = m_tabs->tabText(m_tabs->currentIndex());
        if (tabText.contains("Clients")) {
            addClient();
        } else if (tabText.contains("Orders")) {
            addWorkOrder();
        } else if (tabText.contains("Inventory")) {
            addInventoryItem();
        }
    });
    
    // F5 - Refresh data
    auto *refreshShortcut = new QShortcut(QKeySequence::Refresh, this);
    connect(refreshShortcut, &QShortcut::activated, [this]() {
        if (m_householdsModel) m_householdsModel->select();
        if (m_ordersModel) m_ordersModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        updateStatusBar();
        
        // Refresh dashboard if it exists
        for (int i = 0; i < m_tabs->count(); ++i) {
            if (m_tabs->tabText(i).contains("Dashboard")) {
                DashboardWidget *dashboard = qobject_cast<DashboardWidget*>(m_tabs->widget(i));
                if (dashboard) {
                    dashboard->refreshData();
                }
                break;
            }
        }
        
        m_statusBar->showMessage("Data refreshed!", 2000);
    });
    
    // Escape - Clear search (focus search box and clear it)
    auto *escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escapeShortcut, &QShortcut::activated, [this]() {
        // Find search boxes in current tab and clear them
        QWidget *currentTab = m_tabs->currentWidget();
        if (currentTab) {
            QList<QLineEdit*> searchBoxes = currentTab->findChildren<QLineEdit*>();
            for (QLineEdit *searchBox : searchBoxes) {
                if (searchBox->placeholderText().contains("Search")) {
                    searchBox->clear();
                    searchBox->setFocus();
                    break;
                }
            }
        }
    });
    
    qDebug() << "Keyboard shortcuts setup complete";
}

// Missing method implementations
void MainWindow::setupMenuBar()
{
    qDebug() << "Setting up menu bar...";
    
    auto *menuBar = this->menuBar();
    
    // File menu
    auto *fileMenu = menuBar->addMenu("&File");
    
    auto *refreshAction = fileMenu->addAction("&Refresh Data");
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, &QAction::triggered, [this]() {
        if (m_householdsModel) m_householdsModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        if (m_ordersModel) m_ordersModel->select();
    });
    
    fileMenu->addSeparator();
    
    auto *logoutAction = fileMenu->addAction("&Logout");
    logoutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    
    fileMenu->addSeparator();
    
    auto *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // Admin menu (only for admin users)
    if (Authorization::hasPermission(m_userType, Authorization::Permission::ManageUsers)) {
        auto *adminMenu = menuBar->addMenu("&Admin");
        
        auto *loadSampleDataAction = adminMenu->addAction("&Load Sample Data");
        connect(loadSampleDataAction, &QAction::triggered, this, &MainWindow::loadSampleData);
        
        adminMenu->addSeparator();
        
        auto *exportClientsAction = adminMenu->addAction("Export &Clients to CSV");
        connect(exportClientsAction, &QAction::triggered, this, &MainWindow::exportClientsToCSV);
        
        auto *exportOrdersAction = adminMenu->addAction("Export &Orders to CSV");
        connect(exportOrdersAction, &QAction::triggered, this, &MainWindow::exportOrdersToCSV);
        
        auto *exportInventoryAction = adminMenu->addAction("Export &Inventory to CSV");
        connect(exportInventoryAction, &QAction::triggered, this, &MainWindow::exportInventoryToCSV);
        
        adminMenu->addSeparator();
        
        auto *clearDataAction = adminMenu->addAction("&Clear All Data");
        connect(clearDataAction, &QAction::triggered, this, &MainWindow::clearAllData);
    }
    
    // Help menu
    auto *helpMenu = menuBar->addMenu("&Help");
    
    auto *aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About Firewood Bank",
            "<h2>Firewood Bank Management System</h2>"
            "<p>Version 1.0</p>"
            "<p>A comprehensive system for managing firewood distribution, "
            "volunteer coordination, and client services.</p>"
            "<p>Built with Qt6 and C++17</p>");
    });
}

void MainWindow::setupToolbar()
{
    qDebug() << "Setting up toolbar...";
    
    auto *tb = addToolBar("Main");
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));
    
    // Everyone can view their own profile
    auto *myProfileAction = new QAction(AdobeStyles::ICON_USERS + " My Profile", this);
    myProfileAction->setToolTip("View and edit my profile");
    connect(myProfileAction, &QAction::triggered, this, &MainWindow::viewMyProfile);
    tb->addAction(myProfileAction);
    
    // Employees and Admins can view employee directory
    if (Authorization::hasPermission(m_userType, Authorization::Permission::ViewClients)) {
        auto *employeeDirectoryAction = new QAction(AdobeStyles::ICON_USERS + " Employee Directory", this);
        employeeDirectoryAction->setToolTip("CRM employee contact information");
        connect(employeeDirectoryAction, &QAction::triggered, this, &MainWindow::viewEmployeeDirectory);
        tb->addAction(employeeDirectoryAction);
    }
    
    tb->addSeparator();
    
    // Admin-only actions
    if (Authorization::hasPermission(m_userType, Authorization::Permission::ManageUsers)) {
        auto *changeRequestsAction = new QAction(AdobeStyles::ICON_ORDERS + " Change Requests", this);
        changeRequestsAction->setToolTip("Review profile change requests");
        connect(changeRequestsAction, &QAction::triggered, this, &MainWindow::viewProfileChangeRequests);
        tb->addAction(changeRequestsAction);
        tb->addSeparator();
        auto *manageUsersAction = new QAction(AdobeStyles::ICON_SETTINGS + " Manage Users", this);
        manageUsersAction->setToolTip("Manage system users");
        connect(manageUsersAction, &QAction::triggered, this, &MainWindow::manageUsers);
        tb->addAction(manageUsersAction);
        
        auto *manageAgenciesAction = new QAction("🏢 Manage Agencies", this);
        manageAgenciesAction->setToolTip("Manage partner agencies");
        connect(manageAgenciesAction, &QAction::triggered, this, &MainWindow::manageAgencies);
        tb->addAction(manageAgenciesAction);
        
        tb->addSeparator();
    }
    
    // Admin and Employee actions
    if (Authorization::hasPermission(m_userType, Authorization::Permission::EditClients)) {
        auto *addClient = new QAction(AdobeStyles::ICON_ADD + " Add Client", this);
        addClient->setToolTip("Add a new client/household");
        connect(addClient, &QAction::triggered, this, &MainWindow::addClient);
        tb->addAction(addClient);
        
        auto *editClient = new QAction(AdobeStyles::ICON_EDIT + " Edit Client", this);
        editClient->setToolTip("Edit selected client");
        connect(editClient, &QAction::triggered, this, &MainWindow::editClient);
        tb->addAction(editClient);
        
        tb->addSeparator();
    }
    
    // Add/Edit Orders (Admin and Employees)
    if (Authorization::hasPermission(m_userType, Authorization::Permission::AddOrders)) {
        auto *addOrder = new QAction(AdobeStyles::ICON_ORDERS + " New Order", this);
        addOrder->setToolTip("Create a new work order");
        connect(addOrder, &QAction::triggered, this, &MainWindow::addWorkOrder);
        tb->addAction(addOrder);
        
        auto *editOrder = new QAction(AdobeStyles::ICON_EDIT + " Edit Order", this);
        editOrder->setToolTip("Edit selected work order");
        connect(editOrder, &QAction::triggered, this, &MainWindow::editWorkOrder);
        tb->addAction(editOrder);
        
        tb->addSeparator();
    }
    
    // Inventory Management (Admin and Employees)
    if (Authorization::hasPermission(m_userType, Authorization::Permission::EditInventory)) {
        auto *addInventory = new QAction(AdobeStyles::ICON_INVENTORY + " Add Inventory", this);
        addInventory->setToolTip("Add or update inventory item");
        connect(addInventory, &QAction::triggered, this, &MainWindow::addInventoryItem);
        tb->addAction(addInventory);
        
        auto *editInventory = new QAction(AdobeStyles::ICON_EDIT + " Edit Inventory", this);
        editInventory->setToolTip("Edit selected inventory item");
        connect(editInventory, &QAction::triggered, this, &MainWindow::editInventoryItem);
        tb->addAction(editInventory);
        
        auto *manageEquip = new QAction("🔧 Equipment", this);
        manageEquip->setToolTip("Manage equipment maintenance");
        connect(manageEquip, &QAction::triggered, this, &MainWindow::manageEquipment);
        tb->addAction(manageEquip);
        
        tb->addSeparator();
    }
    
    // Everyone gets refresh
    auto *refreshAction = new QAction(AdobeStyles::ICON_REFRESH + " Refresh", this);
    refreshAction->setToolTip("Refresh data");
    connect(refreshAction, &QAction::triggered, [this]() {
        if (m_householdsModel) m_householdsModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        if (m_ordersModel) m_ordersModel->select();
    });
    tb->addAction(refreshAction);
    
    tb->addSeparator();
    
    // Everyone gets logout
    auto *logoutAction = new QAction(AdobeStyles::ICON_LOGOUT + " Logout", this);
    logoutAction->setToolTip("Logout and return to login screen");
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    tb->addAction(logoutAction);
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

void MainWindow::addWorkOrder()
{
    qDebug() << "Opening add work order dialog...";
    
    WorkOrderDialog dialog(-1, this);
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Work order created successfully";
        if (m_ordersModel) {
            m_ordersModel->select(); // Refresh the view
        }
        if (m_householdsModel) {
            m_householdsModel->select(); // Refresh in case order count updated
        }
    }
}

void MainWindow::editWorkOrder()
{
    if (!m_ordersView || !m_ordersView->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "No Selection", "Please select a work order to edit.");
        return;
    }
    
    QModelIndex index = m_ordersView->selectionModel()->currentIndex();
    onWorkOrderDoubleClicked(index);
}

void MainWindow::onWorkOrderDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    int row = index.row();
    QModelIndex idIndex = m_ordersModel->index(row, 0);
    int orderId = m_ordersModel->data(idIndex).toInt();
    
    qDebug() << "Opening edit dialog for work order ID:" << orderId;
    
    WorkOrderDialog dialog(orderId, this);
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Work order updated successfully";
        if (m_ordersModel) {
            m_ordersModel->select(); // Refresh the view
        }
        if (m_householdsModel) {
            m_householdsModel->select(); // Refresh in case order count updated
        }
    }
}

void MainWindow::addInventoryItem()
{
    qDebug() << "Opening add inventory item dialog...";
    
    InventoryDialog dialog(-1, this);
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Inventory item added successfully";
        if (m_inventoryModel) {
            m_inventoryModel->select(); // Refresh the view
        }
    }
}

void MainWindow::editInventoryItem()
{
    if (!m_inventoryView->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "No Selection", "Please select an inventory item to edit.");
        return;
    }
    
    QModelIndex index = m_inventoryView->selectionModel()->currentIndex();
    onInventoryDoubleClicked(index);
}

void MainWindow::onInventoryDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    int row = index.row();
    QModelIndex idIndex = m_inventoryModel->index(row, 0);
    int itemId = m_inventoryModel->data(idIndex).toInt();
    
    qDebug() << "Opening edit dialog for inventory item ID:" << itemId;
    
    InventoryDialog dialog(itemId, this);
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Inventory item updated successfully";
        if (m_inventoryModel) {
            m_inventoryModel->select(); // Refresh the view
        }
    }
}

void MainWindow::deleteSelectedClient()
{
    if (!m_householdsView->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "No Selection", "Please select a client to delete.");
        return;
    }
    
    QModelIndex index = m_householdsView->selectionModel()->currentIndex();
    int row = index.row();
    QString clientName = m_householdsModel->data(m_householdsModel->index(row, 4)).toString(); // full_name column
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        QString("Are you sure you want to delete client '%1'?\n\nThis action cannot be undone.").arg(clientName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_householdsModel->removeRow(row);
        if (m_householdsModel->submitAll()) {
            QMessageBox::information(this, "Success", "Client deleted successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete client: " + m_householdsModel->lastError().text());
            m_householdsModel->revertAll();
        }
    }
}

void MainWindow::deleteSelectedOrder()
{
    if (!m_ordersView || !m_ordersView->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "No Selection", "Please select an order to delete.");
        return;
    }
    
    QModelIndex index = m_ordersView->selectionModel()->currentIndex();
    int row = index.row();
    int orderId = m_ordersModel->data(m_ordersModel->index(row, 0)).toInt();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        QString("Are you sure you want to delete work order #%1?\n\nThis action cannot be undone.").arg(orderId),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_ordersModel->removeRow(row);
        if (m_ordersModel->submitAll()) {
            QMessageBox::information(this, "Success", "Work order deleted successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete work order: " + m_ordersModel->lastError().text());
            m_ordersModel->revertAll();
        }
    }
}

void MainWindow::deleteSelectedInventoryItem()
{
    if (!m_inventoryView->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "No Selection", "Please select an inventory item to delete.");
        return;
    }
    
    QModelIndex index = m_inventoryView->selectionModel()->currentIndex();
    int row = index.row();
    QString itemName = m_inventoryModel->data(m_inventoryModel->index(row, 2)).toString(); // item_name column
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        QString("Are you sure you want to delete inventory item '%1'?\n\nThis action cannot be undone.").arg(itemName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_inventoryModel->removeRow(row);
        if (m_inventoryModel->submitAll()) {
            QMessageBox::information(this, "Success", "Inventory item deleted successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete inventory item: " + m_inventoryModel->lastError().text());
            m_inventoryModel->revertAll();
        }
    }
}

void MainWindow::applyRoleBasedPermissions()
{
    qDebug() << "Applying role-based permissions for user type:" << m_userType;
    // Permissions are already applied during UI setup based on Authorization checks
}

void MainWindow::logDatabaseStatus()
{
    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << "Database Status:";
    qDebug() << "  - Connected:" << db.isOpen();
    qDebug() << "  - Database name:" << db.databaseName();
    qDebug() << "  - Driver name:" << db.driverName();
    
    if (!db.isOpen()) {
        qDebug() << "  - Last error:" << db.lastError().text();
    }
}

void MainWindow::logout()
{
    qDebug() << "Logout requested";
    emit logoutRequested();
}

void MainWindow::viewMyProfile()
{
    qDebug() << "Opening my profile dialog...";
    MyProfileDialog dialog(m_username, m_fullName, this);
    dialog.exec();
}

void MainWindow::viewEmployeeDirectory()
{
    qDebug() << "Opening employee directory dialog...";
    EmployeeDirectoryDialog dialog(this);
    dialog.exec();
}

void MainWindow::viewProfileChangeRequests()
{
    qDebug() << "Opening profile change requests dialog...";
    ProfileChangeRequestDialog dialog(m_username, this);
    dialog.exec();
}

void MainWindow::viewDeliveryLog()
{
    qDebug() << "Opening delivery log dialog...";
    DeliveryLogDialog dialog(m_username, this);
    dialog.exec();
}

void MainWindow::manageEquipment()
{
    qDebug() << "Opening equipment maintenance dialog...";
    EquipmentMaintenanceDialog dialog(-1, this);
    dialog.exec();
}

void MainWindow::manageUsers()
{
    qDebug() << "Opening user management dialog...";
    UserManagementDialog dialog(this);
    dialog.exec();
}

void MainWindow::manageAgencies()
{
    qDebug() << "Opening agencies management dialog...";
    QMessageBox::information(this, "Coming Soon", "Agencies management feature coming soon!");
}

void MainWindow::loadSampleData()
{
    qDebug() << "Loading sample data...";
    bool success = firewood::db::loadSampleData();
    if (success) {
        QMessageBox::information(this, "Success", "Sample data loaded successfully!");
        // Refresh all models
        if (m_householdsModel) m_householdsModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        if (m_ordersModel) m_ordersModel->select();
    } else {
        QMessageBox::warning(this, "Error", "Failed to load sample data. Check console for details.");
    }
}

void MainWindow::exportClientsToCSV()
{
    qDebug() << "Exporting clients to CSV...";
    QMessageBox::information(this, "Coming Soon", "CSV export feature coming soon!");
}

void MainWindow::exportOrdersToCSV()
{
    qDebug() << "Exporting orders to CSV...";
    QMessageBox::information(this, "Coming Soon", "CSV export feature coming soon!");
}

void MainWindow::exportInventoryToCSV()
{
    qDebug() << "Exporting inventory to CSV...";
    QMessageBox::information(this, "Coming Soon", "CSV export feature coming soon!");
}

void MainWindow::clearAllData()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Clear All Data",
        "Are you sure you want to clear ALL data from the database?\n\nThis action cannot be undone and will remove:\n"
        "- All clients and households\n- All work orders\n- All inventory items\n- All volunteer hours\n- All equipment records\n\n"
        "This is a DESTRUCTIVE operation!",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Coming Soon", "Clear all data feature coming soon!");
    }
}
