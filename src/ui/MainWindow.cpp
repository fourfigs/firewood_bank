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
  const QString& role, QWidget* parent)
  : QMainWindow(parent), m_username(username), m_fullName(fullName), m_role(role)
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

  setWindowTitle(QString("Firewood Bank - %1 (%2)").arg(m_fullName).arg(m_role));

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

  setStyleSheet(FirewoodStyles::MAIN_WINDOW + FirewoodStyles::TAB_WIDGET);

  auto* tabs = new QTabWidget(this);
  setCentralWidget(tabs);

  m_tabs = tabs;

  if (Authorization::isVolunteer(m_role)) {
    auto* volunteerProfile = new VolunteerProfileWidget(m_username, this);
    m_tabs->addTab(volunteerProfile, "👤 My Profile");
  }
  else {
    UserInfo userInfo;
    userInfo.username = m_username;
    userInfo.fullName = m_fullName;
    userInfo.role = m_role;
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

  if (Authorization::isVolunteer(m_role)) {
    qDebug() << "Volunteer role - skipping admin/employee tabs";
    return;
  }

  if (Authorization::hasPermission(m_role, Authorization::Permission::ViewClients)) {
    m_householdsModel = new QSqlTableModel(this, db);
    m_householdsModel->setTable("households");
    m_householdsModel->setEditStrategy(QSqlTableModel::OnFieldChange);

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
    m_householdsView->setStyleSheet(FirewoodStyles::TABLE_VIEW);
    m_householdsView->setSortingEnabled(true);
    connect(m_householdsView, &QTableView::doubleClicked, this, &MainWindow::onClientDoubleClicked);

    m_householdsModel->setHeaderData(1, Qt::Horizontal, "Name");
    m_householdsModel->setHeaderData(2, Qt::Horizontal, "Phone");
    m_householdsModel->setHeaderData(3, Qt::Horizontal, "Address");

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
    clientSearchBox->setStyleSheet(FirewoodStyles::LINE_EDIT);
    
    connect(clientSearchBox, &QLineEdit::textChanged, this, &MainWindow::searchClients);
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(clientSearchBox);
    searchLayout->addStretch();
    
    clientsLayout->addLayout(searchLayout);
    clientsLayout->addWidget(m_householdsView);
    
    m_tabs->addTab(clientsTab, "👥 Clients");
  }

  if (Authorization::hasPermission(m_role, Authorization::Permission::ViewInventory)) {
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
    m_inventoryView->setStyleSheet(FirewoodStyles::TABLE_VIEW);
    m_inventoryView->setSortingEnabled(true);
    connect(m_inventoryView, &QTableView::doubleClicked, this, &MainWindow::onInventoryDoubleClicked);

    m_inventoryModel = inventoryRelModel;

    inventoryRelModel->setHeaderData(1, Qt::Horizontal, "Category");
    inventoryRelModel->setHeaderData(2, Qt::Horizontal, "Item Name");
    inventoryRelModel->setHeaderData(3, Qt::Horizontal, "Quantity");

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
    inventorySearchBox->setStyleSheet(FirewoodStyles::LINE_EDIT);
    
    connect(inventorySearchBox, &QLineEdit::textChanged, this, &MainWindow::searchInventory);
    
    invSearchLayout->addWidget(invSearchLabel);
    invSearchLayout->addWidget(inventorySearchBox);
    invSearchLayout->addStretch();
    
    inventoryLayout->addLayout(invSearchLayout);
    inventoryLayout->addWidget(m_inventoryView);
    
    m_tabs->addTab(inventoryTab, "📦 Inventory");
  }

  if (Authorization::hasPermission(m_role, Authorization::Permission::AddOrders)) {
    m_ordersModel = new QSqlTableModel(this, db);
    m_ordersModel->setTable("orders");
    m_ordersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

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
    m_ordersView->setStyleSheet(FirewoodStyles::TABLE_VIEW);
    m_ordersView->setSortingEnabled(true);
    connect(m_ordersView, &QTableView::doubleClicked, this, &MainWindow::onWorkOrderDoubleClicked);

    m_ordersModel->setHeaderData(2, Qt::Horizontal, "Order Date");
    m_ordersModel->setHeaderData(5, Qt::Horizontal, "Status");
    m_ordersModel->setHeaderData(6, Qt::Horizontal, "Priority");

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
    orderSearchBox->setStyleSheet(FirewoodStyles::LINE_EDIT);
    
    connect(orderSearchBox, &QLineEdit::textChanged, this, &MainWindow::searchOrders);
    
    orderSearchLayout->addWidget(orderSearchLabel);
    orderSearchLayout->addWidget(orderSearchBox);
    orderSearchLayout->addStretch();
    
    ordersLayout->addLayout(orderSearchLayout);
    ordersLayout->addWidget(m_ordersView);
    
    m_tabs->addTab(ordersTab, "📋 Work Orders");
  }
  
  // Add Bookkeeping tab for Admin and Lead roles
  if (Authorization::isAdmin(m_role) || Authorization::isLead(m_role)) {
    auto* bookkeepingWidget = new BookkeepingWidget(m_username, this);
    m_tabs->addTab(bookkeepingWidget, "💰 Bookkeeping");
    qDebug() << "Added Bookkeeping tab for role:" << m_role;
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
    
    QString statusText = QString("📊 System Status - User: %1 (%2)").arg(m_fullName).arg(m_role);
    
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
