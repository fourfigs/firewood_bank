#include "MainWindow.h"
#include "StyleSheet.h"
#include "DashboardWidget.h"
#include "VolunteerProfileWidget.h"
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

using namespace firewood::core;


MainWindow::MainWindow(const QString &username, const QString &fullName, 
                       const QString &role, QWidget *parent)
    : QMainWindow(parent), m_username(username), m_fullName(fullName), m_role(role)
{
    qDebug() << "Creating MainWindow...";
    
    logDatabaseStatus();
    loadUserInfo();
    setupUI();
    
    // Apply role-based permissions before setting up models and toolbar
    applyRoleBasedPermissions();
    
    setupMenuBar();
    setupDatabaseModels();
    setupToolbar();
    
    setWindowTitle(QString("Firewood Bank - %1 (%2)").arg(m_fullName).arg(m_role));
    
    // Open maximized (full screen)
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
    
    // Apply warm color scheme to main window
    setStyleSheet(FirewoodStyles::MAIN_WINDOW + FirewoodStyles::TAB_WIDGET);
    
    auto *tabs = new QTabWidget(this);
    setCentralWidget(tabs);
    
    // Store tabs for later use
    m_tabs = tabs;
    
    // Volunteers get a special profile view
    if (Authorization::isVolunteer(m_role)) {
        auto *volunteerProfile = new VolunteerProfileWidget(m_username, this);
        m_tabs->addTab(volunteerProfile, "👤 My Profile");
    } else {
        // Admin and Employees get the standard dashboard
        UserInfo userInfo;
        userInfo.username = m_username;
        userInfo.fullName = m_fullName;
        userInfo.role = m_role;
        userInfo.contactNumber = m_contactNumber;
        userInfo.email = m_email;
        
        auto *dashboard = new DashboardWidget(userInfo, this);
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
    
    // Volunteers don't see these tabs at all
    if (Authorization::isVolunteer(m_role)) {
        qDebug() << "Volunteer role - skipping admin/employee tabs";
        return;
    }
    
    // Admin and Employees can see clients
    if (Authorization::hasPermission(m_role, Authorization::Permission::ViewClients)) {
        // Households tab (editable for admin/employees)
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
        m_householdsView->setStyleSheet(FirewoodStyles::TABLE_VIEW);
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

        m_tabs->addTab(m_householdsView, "👥 Clients");
    }
    
    // Admin and Employees can see inventory
    if (Authorization::hasPermission(m_role, Authorization::Permission::ViewInventory)) {
        // Inventory tab - using new inventory_items table with relational model
        auto *inventoryRelModel = new QSqlRelationalTableModel(this, db);
        inventoryRelModel->setTable("inventory_items");
        inventoryRelModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        
        // Set up a relation to show category names instead of IDs
        inventoryRelModel->setRelation(1, QSqlRelation("inventory_categories", "id", "name"));
        
        if (!inventoryRelModel->select()) {
            qDebug() << "ERROR: Failed to select inventory_items table:" << inventoryRelModel->lastError().text();
            QMessageBox::warning(this, "Database Warning", 
                                "Could not load inventory data: " + inventoryRelModel->lastError().text());
        } else {
            qDebug() << "Inventory model loaded successfully, rows:" << inventoryRelModel->rowCount();
        }

        m_inventoryView = new QTableView(this);
        m_inventoryView->setModel(inventoryRelModel);
        m_inventoryView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_inventoryView->setSelectionMode(QAbstractItemView::SingleSelection);
        m_inventoryView->setAlternatingRowColors(true);
        m_inventoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_inventoryView->setStyleSheet(FirewoodStyles::TABLE_VIEW);
        connect(m_inventoryView, &QTableView::doubleClicked, this, &MainWindow::onInventoryDoubleClicked);
        
        // Store the model (cast to base class for compatibility)
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

        m_tabs->addTab(m_inventoryView, "📦 Inventory");
    }
    
    // Admin and Employees can see and manage orders
    if (Authorization::hasPermission(m_role, Authorization::Permission::AddOrders)) {
        // Orders tab
        m_ordersModel = new QSqlTableModel(this, db);
        m_ordersModel->setTable("orders");
        m_ordersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        
        if (!m_ordersModel->select()) {
            qDebug() << "ERROR: Failed to select orders table:" << m_ordersModel->lastError().text();
            QMessageBox::warning(this, "Database Warning", 
                                "Could not load orders data: " + m_ordersModel->lastError().text());
        } else {
            qDebug() << "Orders model loaded successfully, rows:" << m_ordersModel->rowCount();
        }

        m_ordersView = new QTableView(this);
        m_ordersView->setModel(m_ordersModel);
        m_ordersView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_ordersView->setSelectionMode(QAbstractItemView::SingleSelection);
        m_ordersView->setAlternatingRowColors(true);
        m_ordersView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_ordersView->setStyleSheet(FirewoodStyles::TABLE_VIEW);
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
        
        // Hide some detailed columns
        m_ordersView->hideColumn(8); // delivery_address
        m_ordersView->hideColumn(9); // delivery_notes
        m_ordersView->hideColumn(10); // assigned_driver
        m_ordersView->hideColumn(11); // payment_method
        m_ordersView->hideColumn(12); // amount_paid
        m_ordersView->hideColumn(13); // notes
        m_ordersView->hideColumn(14); // created_by
        m_ordersView->hideColumn(15); // created_at
        m_ordersView->hideColumn(16); // updated_at

        m_tabs->addTab(m_ordersView, "📋 Work Orders");
    }
}

void MainWindow::setupMenuBar()
{
    qDebug() << "Setting up menu bar...";
    
    auto *menuBar = this->menuBar();
    menuBar->setStyleSheet(
        "QMenuBar { "
        "   background: " + FirewoodStyles::GRADIENT_FOREST + "; "
        "   color: white; "
        "   padding: 6px; "
        "   font-size: 11pt; "
        "   border-bottom: 3px solid " + FirewoodStyles::EMBER_ORANGE + "; "
        "} "
        "QMenuBar::item { "
        "   background: transparent; "
        "   padding: 8px 16px; "
        "   border-radius: 4px; "
        "} "
        "QMenuBar::item:selected { "
        "   background: " + FirewoodStyles::EMBER_ORANGE + "; "
        "} "
        "QMenuBar::item:pressed { "
        "   background: " + FirewoodStyles::FLAME_RED + "; "
        "} "
        "QMenu { "
        "   background-color: white; "
        "   border: 2px solid " + FirewoodStyles::WARM_EARTH + "; "
        "   border-radius: 8px; "
        "   padding: 8px; "
        "} "
        "QMenu::item { "
        "   background: transparent; "
        "   padding: 10px 30px 10px 20px; "
        "   border-radius: 4px; "
        "   margin: 2px; "
        "} "
        "QMenu::item:selected { "
        "   background: " + FirewoodStyles::GRADIENT_EMBER_TO_FLAME + "; "
        "   color: white; "
        "} "
        "QMenu::separator { "
        "   height: 2px; "
        "   background: " + FirewoodStyles::WARM_EARTH + "; "
        "   margin: 6px 10px; "
        "}"
    );
    
    // ===== FILE MENU =====
    auto *fileMenu = menuBar->addMenu("📁 &File");
    
    auto *refreshAction = new QAction("🔄 &Refresh All", this);
    refreshAction->setShortcut(QKeySequence::Refresh); // F5
    refreshAction->setStatusTip("Refresh all data from database");
    connect(refreshAction, &QAction::triggered, [this]() {
        if (m_householdsModel) m_householdsModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        if (m_ordersModel) m_ordersModel->select();
    });
    fileMenu->addAction(refreshAction);
    
    fileMenu->addSeparator();
    
    // Export submenu
    auto *exportMenu = fileMenu->addMenu("📤 &Export");
    exportMenu->setStatusTip("Export data to Excel, Google Sheets, etc.");
    
    auto *exportClientsAction = new QAction("👥 Export &Clients to CSV", this);
    exportClientsAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    exportClientsAction->setStatusTip("Export all client data to CSV file");
    connect(exportClientsAction, &QAction::triggered, this, &MainWindow::exportClientsToCSV);
    exportMenu->addAction(exportClientsAction);
    
    auto *exportOrdersAction = new QAction("📋 Export &Orders to CSV", this);
    exportOrdersAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    exportOrdersAction->setStatusTip("Export all work orders to CSV file");
    connect(exportOrdersAction, &QAction::triggered, this, &MainWindow::exportOrdersToCSV);
    exportMenu->addAction(exportOrdersAction);
    
    auto *exportInventoryAction = new QAction("📦 Export &Inventory to CSV", this);
    exportInventoryAction->setShortcut(QKeySequence("Ctrl+Shift+V"));
    exportInventoryAction->setStatusTip("Export all inventory items to CSV file");
    connect(exportInventoryAction, &QAction::triggered, this, &MainWindow::exportInventoryToCSV);
    exportMenu->addAction(exportInventoryAction);
    
    fileMenu->addSeparator();
    
    auto *logoutAction = new QAction("🔥 &Logout", this);
    logoutAction->setShortcut(QKeySequence("Ctrl+L"));
    logoutAction->setStatusTip("Logout and return to login screen");
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    fileMenu->addAction(logoutAction);
    
    auto *exitAction = new QAction("❌ E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit); // Ctrl+Q
    exitAction->setStatusTip("Exit application");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);
    
    // ===== CLIENTS MENU =====
    if (Authorization::hasPermission(m_role, Authorization::Permission::EditClients)) {
        auto *clientsMenu = menuBar->addMenu("👥 &Clients");
        
        auto *addClientAction = new QAction("➕ &Add Client", this);
        addClientAction->setShortcut(QKeySequence("Ctrl+N"));
        addClientAction->setStatusTip("Add a new client/household");
        connect(addClientAction, &QAction::triggered, this, &MainWindow::addClient);
        clientsMenu->addAction(addClientAction);
        
        auto *editClientAction = new QAction("✏️ &Edit Client", this);
        editClientAction->setShortcut(QKeySequence("Ctrl+E"));
        editClientAction->setStatusTip("Edit selected client");
        connect(editClientAction, &QAction::triggered, this, &MainWindow::editClient);
        clientsMenu->addAction(editClientAction);
    }
    
    // ===== ORDERS MENU =====
    if (Authorization::hasPermission(m_role, Authorization::Permission::AddOrders)) {
        auto *ordersMenu = menuBar->addMenu("📋 &Orders");
        
        auto *newOrderAction = new QAction("📝 &New Order", this);
        newOrderAction->setShortcut(QKeySequence("Ctrl+Shift+N"));
        newOrderAction->setStatusTip("Create a new work order");
        connect(newOrderAction, &QAction::triggered, this, &MainWindow::addWorkOrder);
        ordersMenu->addAction(newOrderAction);
        
        auto *editOrderAction = new QAction("✏️ &Edit Order", this);
        editOrderAction->setShortcut(QKeySequence("Ctrl+Shift+E"));
        editOrderAction->setStatusTip("Edit selected work order");
        connect(editOrderAction, &QAction::triggered, this, &MainWindow::editWorkOrder);
        ordersMenu->addAction(editOrderAction);
    }
    
    // ===== INVENTORY MENU =====
    if (Authorization::hasPermission(m_role, Authorization::Permission::EditInventory)) {
        auto *inventoryMenu = menuBar->addMenu("📦 &Inventory");
        
        auto *addInventoryAction = new QAction("📦 &Add Item", this);
        addInventoryAction->setShortcut(QKeySequence("Ctrl+I"));
        addInventoryAction->setStatusTip("Add or update inventory item");
        connect(addInventoryAction, &QAction::triggered, this, &MainWindow::addInventoryItem);
        inventoryMenu->addAction(addInventoryAction);
        
        auto *editInventoryAction = new QAction("✏️ &Edit Item", this);
        editInventoryAction->setShortcut(QKeySequence("Ctrl+Shift+I"));
        editInventoryAction->setStatusTip("Edit selected inventory item");
        connect(editInventoryAction, &QAction::triggered, this, &MainWindow::editInventoryItem);
        inventoryMenu->addAction(editInventoryAction);
        
        inventoryMenu->addSeparator();
        
        auto *equipmentAction = new QAction("🔧 E&quipment Maintenance", this);
        equipmentAction->setShortcut(QKeySequence("Ctrl+M"));
        equipmentAction->setStatusTip("Manage equipment maintenance");
        connect(equipmentAction, &QAction::triggered, this, &MainWindow::manageEquipment);
        inventoryMenu->addAction(equipmentAction);
    }
    
    // ===== TOOLS MENU =====
    auto *toolsMenu = menuBar->addMenu("🔧 &Tools");
    
    auto *myProfileAction = new QAction("👤 My &Profile", this);
    myProfileAction->setShortcut(QKeySequence("Ctrl+P"));
    myProfileAction->setStatusTip("View and edit my profile");
    connect(myProfileAction, &QAction::triggered, this, &MainWindow::viewMyProfile);
    toolsMenu->addAction(myProfileAction);
    
    // Leads and Admins
    if (Authorization::isAdmin(m_role) || Authorization::isLead(m_role)) {
        auto *employeeListAction = new QAction("👥 &Employee List", this);
        employeeListAction->setShortcut(QKeySequence("Ctrl+Shift+L"));
        employeeListAction->setStatusTip("View complete employee directory");
        connect(employeeListAction, &QAction::triggered, this, &MainWindow::viewEmployeeDirectory);
        toolsMenu->addAction(employeeListAction);
        
        auto *deliveryLogAction = new QAction("🚚 &Delivery Log", this);
        deliveryLogAction->setShortcut(QKeySequence("Ctrl+D"));
        deliveryLogAction->setStatusTip("View delivery log with mileage tracking");
        connect(deliveryLogAction, &QAction::triggered, this, &MainWindow::viewDeliveryLog);
        toolsMenu->addAction(deliveryLogAction);
    }
    
    toolsMenu->addSeparator();
    
    // Admin-only
    if (Authorization::hasPermission(m_role, Authorization::Permission::ManageUsers)) {
        auto *changeRequestsAction = new QAction("📝 Change &Requests", this);
        changeRequestsAction->setShortcut(QKeySequence("Ctrl+R"));
        changeRequestsAction->setStatusTip("Review profile change requests");
        connect(changeRequestsAction, &QAction::triggered, this, &MainWindow::viewProfileChangeRequests);
        toolsMenu->addAction(changeRequestsAction);
        
        auto *manageUsersAction = new QAction("👥 &Manage Users", this);
        manageUsersAction->setShortcut(QKeySequence("Ctrl+U"));
        manageUsersAction->setStatusTip("Manage system users");
        connect(manageUsersAction, &QAction::triggered, this, &MainWindow::manageUsers);
        toolsMenu->addAction(manageUsersAction);
        
        auto *manageAgenciesAction = new QAction("🏢 Manage &Agencies", this);
        manageAgenciesAction->setShortcut(QKeySequence("Ctrl+Shift+A"));
        manageAgenciesAction->setStatusTip("Manage partner agencies");
        connect(manageAgenciesAction, &QAction::triggered, this, &MainWindow::manageAgencies);
        toolsMenu->addAction(manageAgenciesAction);
        
        toolsMenu->addSeparator();
        
        auto *loadSampleDataAction = new QAction("🎲 Load Sample Data", this);
        loadSampleDataAction->setStatusTip("Load sample data for testing and demonstration");
        connect(loadSampleDataAction, &QAction::triggered, this, &MainWindow::loadSampleData);
        toolsMenu->addAction(loadSampleDataAction);
        
        toolsMenu->addSeparator();
        
        auto *clearAllDataAction = new QAction("⚠️ Clear ALL Data", this);
        clearAllDataAction->setStatusTip("Clear all data from database (USE WITH EXTREME CAUTION!)");
        connect(clearAllDataAction, &QAction::triggered, this, &MainWindow::clearAllData);
        toolsMenu->addAction(clearAllDataAction);
    }
    
    // ===== HELP MENU =====
    auto *helpMenu = menuBar->addMenu("❓ &Help");
    
    auto *aboutAction = new QAction("ℹ️ &About Firewood Bank", this);
    aboutAction->setShortcut(QKeySequence::HelpContents); // F1
    aboutAction->setStatusTip("About this application");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About Firewood Bank",
            "<h2>🌲 Firewood Bank Management System 🔥</h2>"
            "<p><b>Version:</b> 1.0.0</p>"
            "<p><b>Organization:</b> Northern Mendocino Ecosystem Recovery Alliance</p>"
            "<p><br>A comprehensive system for managing firewood distribution, "
            "inventory tracking, delivery logistics, and client relationships.</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>📊 Real-time statistics dashboard</li>"
            "<li>🚚 Delivery tracking with mileage logging</li>"
            "<li>📦 Automated inventory management</li>"
            "<li>👥 Role-based access control</li>"
            "<li>📋 Complete audit trails</li>"
            "<li>📤 CSV export for accounting</li>"
            "</ul>"
            "<p><br><i>Helping families stay warm, one cord at a time.</i></p>"
            "<hr>"
            "<p style='font-size: 10pt;'><b>Developer:</b> Kenneth Hanks</p>"
            "<p style='font-size: 9pt;'>This program was created exclusively for the Northern Mendocino "
            "Ecosystem Recovery Alliance. If you would like a version tailored for your "
            "non-profit, contact Kenneth at <b>humboldtfleamarket@gmail.com</b></p>"
            "<p style='font-size: 9pt;'>Everything Kenneth does for NMERA has been a donation, including "
            "his time helping with the firewood bank. If you'd like to buy him some caffeine, "
            "his CashApp is <b>$pseutheo</b> (crypto addresses available by request).</p>"
            "<p style='font-size: 9pt; text-align: center;'><i>Cheers! 🌲🔥🪵</i></p>");
    });
    helpMenu->addAction(aboutAction);
    
    helpMenu->addSeparator();
    
    auto *shortcutsAction = new QAction("⌨️ &Keyboard Shortcuts", this);
    shortcutsAction->setStatusTip("View keyboard shortcuts");
    connect(shortcutsAction, &QAction::triggered, [this]() {
        QMessageBox::information(this, "Keyboard Shortcuts",
            "<h3>⌨️ Keyboard Shortcuts</h3>"
            "<table cellpadding='8'>"
            "<tr><td><b>Ctrl+N</b></td><td>New Client</td></tr>"
            "<tr><td><b>Ctrl+E</b></td><td>Edit Client</td></tr>"
            "<tr><td><b>Ctrl+Shift+N</b></td><td>New Order</td></tr>"
            "<tr><td><b>Ctrl+Shift+E</b></td><td>Edit Order</td></tr>"
            "<tr><td><b>Ctrl+I</b></td><td>Add Inventory</td></tr>"
            "<tr><td><b>Ctrl+M</b></td><td>Equipment Maintenance</td></tr>"
            "<tr><td><b>Ctrl+P</b></td><td>My Profile</td></tr>"
            "<tr><td><b>Ctrl+D</b></td><td>Delivery Log</td></tr>"
            "<tr><td><b>Ctrl+R</b></td><td>Change Requests (Admin)</td></tr>"
            "<tr><td><b>Ctrl+U</b></td><td>Manage Users (Admin)</td></tr>"
            "<tr><td><b>F5</b></td><td>Refresh All Data</td></tr>"
            "<tr><td><b>Ctrl+L</b></td><td>Logout</td></tr>"
            "<tr><td><b>Ctrl+Q</b></td><td>Exit</td></tr>"
            "<tr><td><b>F1</b></td><td>Help</td></tr>"
            "</table>");
    });
    helpMenu->addAction(shortcutsAction);
    
    qDebug() << "Menu bar setup complete";
}

void MainWindow::setupToolbar()
{
    qDebug() << "Setting up toolbar...";
    
    auto *tb = addToolBar("Main");
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));
    
    // Apply beautiful forest gradient to toolbar
    tb->setStyleSheet(FirewoodStyles::TOOLBAR);
    
    // Keep only the most essential, frequently-used actions in toolbar
    // (All other actions available via menu bar with keyboard shortcuts)
    
    // Most common actions: Add Client and New Order
    if (Authorization::hasPermission(m_role, Authorization::Permission::EditClients)) {
        auto *addClient = new QAction("➕ Client", this);
        addClient->setToolTip("Add a new client/household (Ctrl+N)");
        connect(addClient, &QAction::triggered, this, &MainWindow::addClient);
        tb->addAction(addClient);
    }
    
    if (Authorization::hasPermission(m_role, Authorization::Permission::AddOrders)) {
        auto *addOrder = new QAction("📝 Order", this);
        addOrder->setToolTip("Create a new work order (Ctrl+Shift+N)");
        connect(addOrder, &QAction::triggered, this, &MainWindow::addWorkOrder);
        tb->addAction(addOrder);
    }
    
    // Inventory management (common for Leads/Admins)
    if (Authorization::hasPermission(m_role, Authorization::Permission::EditInventory)) {
        auto *addInventory = new QAction("📦 Inventory", this);
        addInventory->setToolTip("Add or update inventory item (Ctrl+I)");
        connect(addInventory, &QAction::triggered, this, &MainWindow::addInventoryItem);
        tb->addAction(addInventory);
    }
    
    tb->addSeparator();
    
    // Delivery Log (essential for Leads/Admins)
    if (Authorization::isAdmin(m_role) || Authorization::isLead(m_role)) {
        auto *deliveryLogAction = new QAction("🚚 Deliveries", this);
        deliveryLogAction->setToolTip("View complete delivery log with mileage tracking (Ctrl+D)");
        connect(deliveryLogAction, &QAction::triggered, this, &MainWindow::viewDeliveryLog);
        tb->addAction(deliveryLogAction);
        tb->addSeparator();
    }
    
    // Everyone gets refresh
    auto *refreshAction = new QAction("🔄 Refresh", this);
    refreshAction->setToolTip("Refresh all data (F5)");
    connect(refreshAction, &QAction::triggered, [this]() {
        if (m_householdsModel) m_householdsModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        if (m_ordersModel) m_ordersModel->select();
    });
    tb->addAction(refreshAction);
    
    // Spacer to push logout to the right
    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tb->addWidget(spacer);
    
    // Everyone gets logout - Make it prominent on the right
    auto *logoutAction = new QAction("🔥 LOGOUT", this);
    logoutAction->setToolTip("Logout and return to login screen (Ctrl+L)");
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    tb->addAction(logoutAction);
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

void MainWindow::applyRoleBasedPermissions()
{
    qDebug() << "Applying role-based permissions for role:" << m_role;
    
    // Volunteers get a completely different view
    if (Authorization::isVolunteer(m_role)) {
        setupVolunteerView();
    }
}

void MainWindow::setupVolunteerView()
{
    qDebug() << "Setting up volunteer-specific view";
    
    // Volunteers see their profile in the main tab
    // Profile widget already created in setupUI()
}

void MainWindow::viewMyProfile()
{
    qDebug() << "Opening my profile dialog";
    
    MyProfileDialog dialog(m_username, m_role, this);
    dialog.exec();
}

void MainWindow::viewEmployeeDirectory()
{
    qDebug() << "Opening employee directory";
    
    EmployeeDirectoryDialog dialog(this);
    dialog.exec();
}

void MainWindow::viewProfileChangeRequests()
{
    qDebug() << "Opening profile change requests";
    
    ProfileChangeRequestDialog dialog(m_username, this);
    dialog.exec();
}

void MainWindow::viewDeliveryLog()
{
    qDebug() << "Opening delivery log";
    
    DeliveryLogDialog dialog(this);
    dialog.exec();
}

void MainWindow::loadSampleData()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Load Sample Data",
        "<h3>⚠️ Load Sample Data</h3>"
        "<p>This will populate your database with sample households, orders, inventory, and more.</p>"
        "<p><b>Warning:</b> This may create duplicate entries if you already have data!</p>"
        "<p>It's recommended to use this only on a fresh database for testing and demonstration.</p>"
        "<br>"
        "<p>Do you want to continue?</p>",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    qDebug() << "User requested to load sample data...";
    
    if (firewood::db::loadSampleData()) {
        // Refresh all models to show the new data
        if (m_householdsModel) m_householdsModel->select();
        if (m_ordersModel) m_ordersModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        
        QMessageBox::information(
            this,
            "Success",
            "<h3>✅ Sample Data Loaded!</h3>"
            "<p>Your database has been populated with:</p>"
            "<ul>"
            "<li>10 sample households/clients</li>"
            "<li>14 work orders (completed, in progress, scheduled, pending)</li>"
            "<li>25+ inventory items across 5 categories</li>"
            "<li>8 equipment maintenance records</li>"
            "<li>8 scheduled work days with volunteer signups</li>"
            "<li>6 additional staff/volunteer users</li>"
            "<li>5 referral agencies</li>"
            "<li>Volunteer certifications</li>"
            "<li>7 completed delivery log entries</li>"
            "</ul>"
            "<p><b>You can now:</b></p>"
            "<ul>"
            "<li>View the statistics dashboard (real data!)</li>"
            "<li>Edit client information</li>"
            "<li>Process work orders</li>"
            "<li>Check the delivery log</li>"
            "<li>Test all features with realistic data</li>"
            "</ul>"
            "<p><i>Login credentials for additional users: password is 'password123'</i></p>"
        );
        
        qDebug() << "Sample data loaded successfully and models refreshed";
    } else {
        QMessageBox::warning(
            this,
            "Error",
            "<h3>❌ Failed to Load Sample Data</h3>"
            "<p>Could not load the sample data file.</p>"
            "<p><b>Possible reasons:</b></p>"
            "<ul>"
            "<li>The file <code>docs/SAMPLE_DATA.sql</code> was not found</li>"
            "<li>Database connection error</li>"
            "<li>Permission issues</li>"
            "</ul>"
            "<p>Check the console output for more details.</p>"
        );
        
        qDebug() << "Failed to load sample data - check console for errors";
    }
}

void MainWindow::exportClientsToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Clients to CSV",
        QDir::homePath() + "/Firewood_Bank_Clients_" + QDate::currentDate().toString("yyyy-MM-dd") + ".csv",
        "CSV Files (*.csv);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Could not open file for writing:\n" + fileName);
        return;
    }
    
    QTextStream out(&file);
    
    // CSV Header
    out << "ID,Name,Phone,Email,Address,Mailing Address,Gate Code,Stove Size,"
        << "Is Volunteer,Waiver Signed,Has License,Has Vehicle,Works for Wood,"
        << "Wood Credit Received,Credit Balance,Stock on Hand,Order Count,Last Order Date,"
        << "Referral Source,Notes,Created At\n";
    
    // Query all households
    QSqlQuery query;
    if (!query.exec("SELECT * FROM households ORDER BY name")) {
        QMessageBox::critical(this, "Database Error", "Failed to query households:\n" + query.lastError().text());
        return;
    }
    
    int count = 0;
    while (query.next()) {
        // Escape and format each field for CSV
        auto escapeCSV = [](const QString &value) -> QString {
            QString escaped = value;
            escaped.replace("\"", "\"\""); // Escape quotes
            if (escaped.contains(',') || escaped.contains('\n') || escaped.contains('"')) {
                escaped = "\"" + escaped + "\""; // Quote if contains special chars
            }
            return escaped;
        };
        
        out << query.value("id").toString() << ","
            << escapeCSV(query.value("name").toString()) << ","
            << escapeCSV(query.value("phone").toString()) << ","
            << escapeCSV(query.value("email").toString()) << ","
            << escapeCSV(query.value("address").toString()) << ","
            << escapeCSV(query.value("mailing_address").toString()) << ","
            << escapeCSV(query.value("gate_code").toString()) << ","
            << escapeCSV(query.value("stove_fireplace_size").toString()) << ","
            << (query.value("is_volunteer").toBool() ? "Yes" : "No") << ","
            << (query.value("waiver_signed").toBool() ? "Yes" : "No") << ","
            << (query.value("has_license").toBool() ? "Yes" : "No") << ","
            << (query.value("has_working_vehicle").toBool() ? "Yes" : "No") << ","
            << (query.value("participating_in_work_for_wood").toBool() ? "Yes" : "No") << ","
            << query.value("wood_credit_received").toString() << ","
            << query.value("credit_balance").toString() << ","
            << query.value("stock_on_hand").toString() << ","
            << query.value("order_count").toString() << ","
            << query.value("last_order_date").toString() << ","
            << escapeCSV(query.value("referral_source").toString()) << ","
            << escapeCSV(query.value("notes").toString()) << ","
            << query.value("created_at").toString() << "\n";
        count++;
    }
    
    file.close();
    
    QMessageBox::information(
        this,
        "Export Complete",
        QString("<h3>✅ Export Successful</h3>"
                "<p>Exported <b>%1 clients</b> to:</p>"
                "<p><code>%2</code></p>"
                "<p>You can now:</p>"
                "<ul>"
                "<li>Open in Excel</li>"
                "<li>Import to Google Sheets</li>"
                "<li>Open in any spreadsheet software</li>"
                "</ul>").arg(count).arg(fileName)
    );
    
    qDebug() << "Exported" << count << "clients to" << fileName;
}

void MainWindow::exportOrdersToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Work Orders to CSV",
        QDir::homePath() + "/Firewood_Bank_Orders_" + QDate::currentDate().toString("yyyy-MM-dd") + ".csv",
        "CSV Files (*.csv);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Could not open file for writing:\n" + fileName);
        return;
    }
    
    QTextStream out(&file);
    
    // CSV Header
    out << "Order ID,Client Name,Date Received,Requested Cords,Delivered Cords,Status,Priority,"
        << "Delivery Date,Delivery Address,Assigned Driver,Payment Method,Amount Paid,"
        << "Delivery Time,Start Mileage,End Mileage,Total Miles,Completed Date,Notes,Created By,Created At\n";
    
    // Query all orders with client names
    QSqlQuery query;
    if (!query.exec("SELECT o.*, h.name as client_name FROM orders o "
                    "LEFT JOIN households h ON o.household_id = h.id "
                    "ORDER BY o.date_received DESC")) {
        QMessageBox::critical(this, "Database Error", "Failed to query orders:\n" + query.lastError().text());
        return;
    }
    
    int count = 0;
    while (query.next()) {
        auto escapeCSV = [](const QString &value) -> QString {
            QString escaped = value;
            escaped.replace("\"", "\"\"");
            if (escaped.contains(',') || escaped.contains('\n') || escaped.contains('"')) {
                escaped = "\"" + escaped + "\"";
            }
            return escaped;
        };
        
        double totalMiles = query.value("end_mileage").toDouble() - query.value("start_mileage").toDouble();
        
        out << query.value("id").toString() << ","
            << escapeCSV(query.value("client_name").toString()) << ","
            << query.value("date_received").toString() << ","
            << query.value("requested_cords").toString() << ","
            << query.value("delivered_cords").toString() << ","
            << escapeCSV(query.value("status").toString()) << ","
            << escapeCSV(query.value("priority").toString()) << ","
            << query.value("delivery_date").toString() << ","
            << escapeCSV(query.value("delivery_address").toString()) << ","
            << escapeCSV(query.value("assigned_driver").toString()) << ","
            << escapeCSV(query.value("payment_method").toString()) << ","
            << query.value("amount_paid").toString() << ","
            << escapeCSV(query.value("delivery_time").toString()) << ","
            << query.value("start_mileage").toString() << ","
            << query.value("end_mileage").toString() << ","
            << QString::number(totalMiles, 'f', 1) << ","
            << query.value("completed_date").toString() << ","
            << escapeCSV(query.value("notes").toString()) << ","
            << escapeCSV(query.value("created_by").toString()) << ","
            << query.value("created_at").toString() << "\n";
        count++;
    }
    
    file.close();
    
    QMessageBox::information(
        this,
        "Export Complete",
        QString("<h3>✅ Export Successful</h3>"
                "<p>Exported <b>%1 work orders</b> to:</p>"
                "<p><code>%2</code></p>"
                "<p>Perfect for:</p>"
                "<ul>"
                "<li>Grant reporting</li>"
                "<li>Financial tracking</li>"
                "<li>Impact analysis</li>"
                "</ul>").arg(count).arg(fileName)
    );
    
    qDebug() << "Exported" << count << "orders to" << fileName;
}

void MainWindow::exportInventoryToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Inventory to CSV",
        QDir::homePath() + "/Firewood_Bank_Inventory_" + QDate::currentDate().toString("yyyy-MM-dd") + ".csv",
        "CSV Files (*.csv);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Could not open file for writing:\n" + fileName);
        return;
    }
    
    QTextStream out(&file);
    
    // CSV Header
    out << "ID,Category,Item Name,Quantity,Unit,Location,Notes,Last Updated,Created At\n";
    
    // Query inventory with category names
    QSqlQuery query;
    if (!query.exec("SELECT i.*, c.name as category_name FROM inventory_items i "
                    "LEFT JOIN inventory_categories c ON i.category_id = c.id "
                    "ORDER BY c.name, i.name")) {
        QMessageBox::critical(this, "Database Error", "Failed to query inventory:\n" + query.lastError().text());
        return;
    }
    
    int count = 0;
    while (query.next()) {
        auto escapeCSV = [](const QString &value) -> QString {
            QString escaped = value;
            escaped.replace("\"", "\"\"");
            if (escaped.contains(',') || escaped.contains('\n') || escaped.contains('"')) {
                escaped = "\"" + escaped + "\"";
            }
            return escaped;
        };
        
        out << query.value("id").toString() << ","
            << escapeCSV(query.value("category_name").toString()) << ","
            << escapeCSV(query.value("name").toString()) << ","
            << query.value("quantity").toString() << ","
            << escapeCSV(query.value("unit").toString()) << ","
            << escapeCSV(query.value("location").toString()) << ","
            << escapeCSV(query.value("notes").toString()) << ","
            << query.value("last_updated").toString() << ","
            << query.value("created_at").toString() << "\n";
        count++;
    }
    
    file.close();
    
    QMessageBox::information(
        this,
        "Export Complete",
        QString("<h3>✅ Export Successful</h3>"
                "<p>Exported <b>%1 inventory items</b> to:</p>"
                "<p><code>%2</code></p>").arg(count).arg(fileName)
    );
    
    qDebug() << "Exported" << count << "inventory items to" << fileName;
}

void MainWindow::deleteSelectedClient()
{
    if (!m_householdsView || !m_householdsModel) {
        QMessageBox::warning(this, "Error", "No client view available");
        return;
    }
    
    QModelIndex current = m_householdsView->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select a client to delete.");
        return;
    }
    
    int row = current.row();
    int clientId = m_householdsModel->data(m_householdsModel->index(row, 0)).toInt();
    QString clientName = m_householdsModel->data(m_householdsModel->index(row, 1)).toString();
    
    // Warning dialog with export option
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("⚠️ Delete Client");
    msgBox.setText(QString("<h3>Delete Client: %1</h3>").arg(clientName));
    msgBox.setInformativeText(
        "<p><b>This will permanently delete:</b></p>"
        "<ul>"
        "<li>Client information</li>"
        "<li>All associated work orders</li>"
        "<li>Order history</li>"
        "</ul>"
        "<p><b>This action CANNOT be undone!</b></p>"
        "<p>Would you like to export client data first?</p>"
    );
    
    QPushButton *exportFirst = msgBox.addButton("📤 Export First", QMessageBox::ActionRole);
    QPushButton *deleteButton = msgBox.addButton("🗑️ Delete Now", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelButton);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == exportFirst) {
        exportClientsToCSV();
        // Ask again after export
        QMessageBox::StandardButton confirm = QMessageBox::question(
            this,
            "Proceed with Deletion?",
            QString("Data exported. Do you still want to delete client '%1'?").arg(clientName),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );
        if (confirm != QMessageBox::Yes) {
            return;
        }
    } else if (msgBox.clickedButton() != deleteButton) {
        return; // User cancelled
    }
    
    // Delete the client
    if (m_householdsModel->removeRow(row)) {
        if (m_householdsModel->submitAll()) {
            QMessageBox::information(this, "Success", QString("Client '%1' has been deleted.").arg(clientName));
            m_householdsModel->select(); // Refresh
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete client: " + m_householdsModel->lastError().text());
            m_householdsModel->revertAll();
        }
    }
}

void MainWindow::deleteSelectedOrder()
{
    if (!m_ordersView || !m_ordersModel) {
        QMessageBox::warning(this, "Error", "No order view available");
        return;
    }
    
    QModelIndex current = m_ordersView->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select an order to delete.");
        return;
    }
    
    int row = current.row();
    int orderId = m_ordersModel->data(m_ordersModel->index(row, 0)).toInt();
    
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this,
        "⚠️ Delete Order",
        QString("<h3>Delete Order #%1?</h3>"
                "<p>This will permanently delete this work order.</p>"
                "<p><b>This action CANNOT be undone!</b></p>").arg(orderId),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_ordersModel->removeRow(row)) {
            if (m_ordersModel->submitAll()) {
                QMessageBox::information(this, "Success", QString("Order #%1 has been deleted.").arg(orderId));
                m_ordersModel->select(); // Refresh
            } else {
                QMessageBox::critical(this, "Error", "Failed to delete order: " + m_ordersModel->lastError().text());
                m_ordersModel->revertAll();
            }
        }
    }
}

void MainWindow::deleteSelectedInventoryItem()
{
    if (!m_inventoryView || !m_inventoryModel) {
        QMessageBox::warning(this, "Error", "No inventory view available");
        return;
    }
    
    QModelIndex current = m_inventoryView->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "No Selection", "Please select an inventory item to delete.");
        return;
    }
    
    int row = current.row();
    QString itemName = m_inventoryModel->data(m_inventoryModel->index(row, 2)).toString();
    
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this,
        "⚠️ Delete Inventory Item",
        QString("<h3>Delete Inventory: %1?</h3>"
                "<p>This will permanently delete this inventory item.</p>"
                "<p><b>This action CANNOT be undone!</b></p>").arg(itemName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (m_inventoryModel->removeRow(row)) {
            if (m_inventoryModel->submitAll()) {
                QMessageBox::information(this, "Success", QString("Item '%1' has been deleted.").arg(itemName));
                m_inventoryModel->select(); // Refresh
            } else {
                QMessageBox::critical(this, "Error", "Failed to delete item: " + m_inventoryModel->lastError().text());
                m_inventoryModel->revertAll();
            }
        }
    }
}

void MainWindow::clearAllData()
{
    // EXTREME WARNING - requires admin role
    if (!Authorization::isAdmin(m_role)) {
        QMessageBox::critical(this, "Access Denied", "Only administrators can clear all data.");
        return;
    }
    
    // First warning
    QMessageBox::StandardButton firstWarning = QMessageBox::warning(
        this,
        "⚠️ EXTREME CAUTION REQUIRED",
        "<h2>⚠️ DELETE ALL DATA ⚠️</h2>"
        "<p><b>This will permanently delete EVERYTHING:</b></p>"
        "<ul>"
        "<li>All clients/households</li>"
        "<li>All work orders</li>"
        "<li>All inventory items</li>"
        "<li>All delivery logs</li>"
        "<li>All volunteer certifications</li>"
        "<li>All work schedules</li>"
        "</ul>"
        "<p><font color='red'><b>THIS CANNOT BE UNDONE!</b></font></p>"
        "<p>Are you ABSOLUTELY SURE?</p>",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (firstWarning != QMessageBox::Yes) {
        return;
    }
    
    // Offer to export first
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Export Data First?");
    msgBox.setText("<h3>Export Before Deleting?</h3>");
    msgBox.setInformativeText(
        "<p>It's <b>strongly recommended</b> to export your data before deleting everything.</p>"
        "<p>Would you like to export all data first?</p>"
    );
    
    QPushButton *exportAll = msgBox.addButton("📤 Export Everything", QMessageBox::ActionRole);
    QPushButton *skipExport = msgBox.addButton("Skip Export", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    msgBox.setDefaultButton(exportAll);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == cancelButton) {
        return;
    }
    
    if (msgBox.clickedButton() == exportAll) {
        // Export all data
        exportClientsToCSV();
        exportOrdersToCSV();
        exportInventoryToCSV();
        
        QMessageBox::information(this, "Export Complete", "All data has been exported to CSV files.");
    }
    
    // Final confirmation
    bool ok;
    QString confirmation = QInputDialog::getText(
        this,
        "Final Confirmation",
        "Type 'DELETE ALL DATA' to confirm deletion:",
        QLineEdit::Normal,
        "",
        &ok
    );
    
    if (!ok || confirmation != "DELETE ALL DATA") {
        QMessageBox::information(this, "Cancelled", "Data deletion cancelled.");
        return;
    }
    
    // Perform the deletion
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        QMessageBox::critical(this, "Error", "Failed to start database transaction");
        return;
    }
    
    QSqlQuery query(db);
    QStringList tables = {
        "delivery_log",
        "profile_change_requests",
        "volunteer_certifications",
        "work_schedule_signups",
        "work_schedule",
        "orders",
        "inventory_items",
        "households",
        "agencies"
    };
    
    bool success = true;
    for (const QString &table : tables) {
        if (!query.exec(QString("DELETE FROM %1 WHERE id > 0").arg(table))) {
            qDebug() << "Failed to clear table:" << table << query.lastError().text();
            success = false;
            break;
        }
    }
    
    if (success && db.commit()) {
        // Refresh all models
        if (m_householdsModel) m_householdsModel->select();
        if (m_ordersModel) m_ordersModel->select();
        if (m_inventoryModel) m_inventoryModel->select();
        
        QMessageBox::information(
            this,
            "Success",
            "<h3>✅ All Data Cleared</h3>"
            "<p>The database has been cleared of all:</p>"
            "<ul>"
            "<li>Clients</li>"
            "<li>Orders</li>"
            "<li>Inventory</li>"
            "<li>Delivery logs</li>"
            "<li>Work schedules</li>"
            "</ul>"
            "<p>Users and system settings have been preserved.</p>"
        );
    } else {
        db.rollback();
        QMessageBox::critical(this, "Error", "Failed to clear all data. Changes have been rolled back.");
    }
}

void MainWindow::manageUsers()
{
    qDebug() << "Opening user management dialog";
    
    UserManagementDialog dialog(this);
    dialog.exec();
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
    if (!m_inventoryView || !m_inventoryView->selectionModel()->hasSelection()) {
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

void MainWindow::manageEquipment()
{
    qDebug() << "Opening equipment maintenance management";
    
    // List equipment with simple dialog
    bool ok;
    QStringList items;
    QList<int> equipmentIds;
    
    QSqlQuery query("SELECT id, equipment_name, current_hours, next_service_hours FROM equipment_maintenance ORDER BY equipment_name");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        double current = query.value(2).toDouble();
        double next = query.value(3).toDouble();
        double remaining = next - current;
        
        QString status = remaining <= 0 ? "⚠️ OVERDUE" : 
                        remaining <= 5 ? QString("⚠️ %1h").arg(remaining, 0, 'f', 1) :
                        QString("✓ %1h").arg(remaining, 0, 'f', 1);
        
        items << QString("%1 - %2").arg(name).arg(status);
        equipmentIds << id;
    }
    
    items << "➕ Add New Equipment";
    items << "❌ Cancel";
    
    QString action = QInputDialog::getItem(this, "Equipment Maintenance", 
                                          "Select equipment to edit:", items, 0, false, &ok);
    
    if (!ok || action == "❌ Cancel") return;
    
    if (action == "➕ Add New Equipment") {
        EquipmentMaintenanceDialog dialog(-1, this);
        if (dialog.exec() == QDialog::Accepted) {
            QMessageBox::information(this, "Success", "Equipment added successfully!");
        }
    } else {
        int selectedIndex = items.indexOf(action);
        if (selectedIndex >= 0 && selectedIndex < equipmentIds.size()) {
            int equipmentId = equipmentIds[selectedIndex];
            EquipmentMaintenanceDialog dialog(equipmentId, this);
            if (dialog.exec() == QDialog::Accepted) {
                QMessageBox::information(this, "Success", "Equipment updated successfully!");
            }
        }
    }
}

void MainWindow::manageAgencies()
{
    qDebug() << "Opening agency management";
    
    // Simple agency management
    bool ok;
    QStringList items = {"Add New Agency", "List All Agencies", "Cancel"};
    QString action = QInputDialog::getItem(this, "Agency Management", 
                                          "Choose an action:", items, 0, false, &ok);
    
    if (!ok || action == "Cancel") return;
    
    if (action == "Add New Agency") {
        QString name = QInputDialog::getText(this, "Add Agency", "Agency Name:", 
                                            QLineEdit::Normal, "", &ok);
        if (!ok || name.isEmpty()) return;
        
        QStringList types = {"Food Bank", "Social Services", "Community Center", "Church", "Other"};
        QString type = QInputDialog::getItem(this, "Add Agency", "Agency Type:", types, 0, false, &ok);
        if (!ok) return;
        
        QSqlQuery query;
        query.prepare("INSERT INTO agencies (name, type, active) VALUES (:name, :type, 1)");
        query.bindValue(":name", name);
        query.bindValue(":type", type);
        
        if (query.exec()) {
            QMessageBox::information(this, "Success", 
                                   QString("Agency '%1' created successfully!").arg(name));
        } else {
            QMessageBox::critical(this, "Error", 
                                "Failed to create agency: " + query.lastError().text());
        }
    }
    else if (action == "List All Agencies") {
        QString agencyList;
        QSqlQuery query("SELECT name, type, active FROM agencies ORDER BY name");
        while (query.next()) {
            QString status = query.value(2).toInt() ? "Active" : "Inactive";
            agencyList += QString("%1 (%2) - %3\n")
                .arg(query.value(0).toString())
                .arg(query.value(1).toString())
                .arg(status);
        }
        if (agencyList.isEmpty()) {
            agencyList = "No agencies found.";
        }
        QMessageBox::information(this, "All Agencies", agencyList);
    }
}

void MainWindow::logout()
{
    auto response = QMessageBox::question(this, "Logout", 
                                         "Are you sure you want to logout?",
                                         QMessageBox::Yes | QMessageBox::No);
    
    if (response == QMessageBox::Yes) {
        qDebug() << "User" << m_username << "logging out...";
        emit logoutRequested();
        close();
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



