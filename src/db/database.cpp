#include "database.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>

namespace firewood::db {

static void ensureAppDataPath(QString &dbFilePath) {
    const QString appDataRoot = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    qDebug() << "App data root:" << appDataRoot;
    
    if (!QDir().mkpath(appDataRoot)) {
        qDebug() << "ERROR: Failed to create app data directory:" << appDataRoot;
        return;
    }
    
    dbFilePath = appDataRoot + "/firewood_bank.sqlite";
    qDebug() << "Database file path:" << dbFilePath;
}

static void runMigrations(QSqlDatabase &db) {
    qDebug() << "Running database migrations...";
    
    QSqlQuery query(db);
    
    // Create schema version table
    if (!query.exec("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER NOT NULL);")) {
        qDebug() << "ERROR: Failed to create schema_version table:" << query.lastError().text();
        return;
    }
    
    if (!query.exec("INSERT INTO schema_version (version) SELECT 0 WHERE NOT EXISTS (SELECT 1 FROM schema_version);")) {
        qDebug() << "ERROR: Failed to initialize schema version:" << query.lastError().text();
        return;
    }

    int version = 0;
    if (query.exec("SELECT version FROM schema_version LIMIT 1;") && query.next()) {
        version = query.value(0).toInt();
        qDebug() << "Current schema version:" << version;
    } else {
        qDebug() << "ERROR: Failed to read schema version:" << query.lastError().text();
        return;
    }

    if (!db.transaction()) {
        qDebug() << "ERROR: Failed to start transaction:" << db.lastError().text();
        return;
    }
    
    // Migration 1: households and inventory (minimal)
    if (version < 1) {
        qDebug() << "Running migration 1: Creating households and inventory tables...";
        
        if (!query.exec("CREATE TABLE IF NOT EXISTS households (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  name TEXT NOT NULL,\n"
                       "  address TEXT,\n"
                       "  phone TEXT,\n"
                       "  notes TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create households table:" << query.lastError().text();
            db.rollback();
            return;
        }

        if (!query.exec("CREATE TABLE IF NOT EXISTS inventory (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  species TEXT,\n"
                       "  form TEXT,\n"
                       "  volume_cords REAL NOT NULL DEFAULT 0,\n"
                       "  moisture_pct REAL,\n"
                       "  status TEXT,\n"
                       "  location TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create inventory table:" << query.lastError().text();
            db.rollback();
            return;
        }

        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 1;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 1;
        qDebug() << "Migration 1 completed successfully";
    }
    
    // Migration 2: users table with authentication
    if (version < 2) {
        qDebug() << "Running migration 2: Creating users table...";
        
        if (!query.exec("CREATE TABLE IF NOT EXISTS users (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  username TEXT NOT NULL UNIQUE,\n"
                       "  password_hash TEXT NOT NULL,\n"
                       "  role TEXT NOT NULL DEFAULT 'volunteer',\n"
                       "  full_name TEXT,\n"
                       "  email TEXT,\n"
                       "  active INTEGER NOT NULL DEFAULT 1,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  last_login TEXT\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create users table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create default users (admin, user, volunteer)
        QList<QMap<QString, QString>> defaultUsers = {
            {
                {"username", "admin"},
                {"password_hash", "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918"}, // SHA-256 of "admin"
                {"role", "admin"},
                {"full_name", "System Administrator"},
                {"email", "admin@firewoodbank.org"}
            },
            {
                {"username", "lead"},
                {"password_hash", "a2e88876c089dccf60923bb7cec0fa5e40e91ea2f8c1d8e19d09b12949eb25d3"}, // SHA-256 of "lead"
                {"role", "lead"},
                {"full_name", "Team Lead"},
                {"email", "lead@firewoodbank.org"}
            },
            {
                {"username", "user"},
                {"password_hash", "04f8996da763b7a969b1028ee3007569eaf3a635486ddab211d512c85b9df8fb"}, // SHA-256 of "user"
                {"role", "employee"},
                {"full_name", "Regular User"},
                {"email", "user@firewoodbank.org"}
            },
            {
                {"username", "volunteer"},
                {"password_hash", "38c6d4e87238c6c3578704ba9b0d47b3d8f4ce7fc2ed8c8e3da7b0c9c5e0f067"}, // SHA-256 of "volunteer"
                {"role", "volunteer"},
                {"full_name", "Volunteer User"},
                {"email", "volunteer@firewoodbank.org"}
            }
        };
        
        for (const auto &userData : defaultUsers) {
            QSqlQuery insertUser(db);
            insertUser.prepare("INSERT INTO users (username, password_hash, role, full_name, email, active) "
                              "VALUES (:username, :password_hash, :role, :full_name, :email, :active)");
            insertUser.bindValue(":username", userData["username"]);
            insertUser.bindValue(":password_hash", userData["password_hash"]);
            insertUser.bindValue(":role", userData["role"]);
            insertUser.bindValue(":full_name", userData["full_name"]);
            insertUser.bindValue(":email", userData["email"]);
            insertUser.bindValue(":active", 1);
            
            if (!insertUser.exec()) {
                qDebug() << "ERROR: Failed to create user" << userData["username"] << ":" << insertUser.lastError().text();
                db.rollback();
                return;
            }
            
            qDebug() << "Created default user:" << userData["username"];
        }
        
        qDebug() << "All default users created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 2;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 2;
        qDebug() << "Migration 2 completed successfully";
    }
    
    // Migration 3: Enhanced client/household tracking
    if (version < 3) {
        qDebug() << "Running migration 3: Expanding households table with detailed client tracking...";
        
        // Add new columns to households table
        QStringList newColumns = {
            "ALTER TABLE households ADD COLUMN email TEXT;",
            "ALTER TABLE households ADD COLUMN mailing_address TEXT;",
            "ALTER TABLE households ADD COLUMN gate_code TEXT;",
            "ALTER TABLE households ADD COLUMN stove_size TEXT;",
            "ALTER TABLE households ADD COLUMN is_volunteer INTEGER DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN waiver_signed INTEGER DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN has_license INTEGER DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN has_working_vehicle INTEGER DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN works_for_wood INTEGER DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN wood_credit_received REAL DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN credit_balance REAL DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN last_volunteer_date TEXT;",
            "ALTER TABLE households ADD COLUMN order_count INTEGER DEFAULT 0;",
            "ALTER TABLE households ADD COLUMN last_order_date TEXT;"
        };
        
        for (const QString &sql : newColumns) {
            if (!query.exec(sql)) {
                qDebug() << "ERROR: Failed to alter households table:" << query.lastError().text();
                qDebug() << "SQL:" << sql;
                db.rollback();
                return;
            }
        }
        
        // Create volunteer_hours table for detailed tracking
        if (!query.exec("CREATE TABLE IF NOT EXISTS volunteer_hours (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  household_id INTEGER NOT NULL,\n"
                       "  date TEXT NOT NULL,\n"
                       "  hours REAL NOT NULL,\n"
                       "  activity TEXT,\n"
                       "  notes TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  FOREIGN KEY (household_id) REFERENCES households(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create volunteer_hours table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create index for faster lookups
        query.exec("CREATE INDEX IF NOT EXISTS idx_volunteer_hours_household ON volunteer_hours(household_id);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_volunteer_hours_date ON volunteer_hours(date);");
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 3;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 3;
        qDebug() << "Migration 3 completed successfully";
    }
    
    // Migration 4: Agencies table
    if (version < 4) {
        qDebug() << "Running migration 4: Creating agencies table...";
        
        if (!query.exec("CREATE TABLE IF NOT EXISTS agencies (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  name TEXT NOT NULL,\n"
                       "  type TEXT,\n"
                       "  contact_name TEXT,\n"
                       "  phone TEXT,\n"
                       "  email TEXT,\n"
                       "  address TEXT,\n"
                       "  notes TEXT,\n"
                       "  active INTEGER DEFAULT 1,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create agencies table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        qDebug() << "Agencies table created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 4;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 4;
        qDebug() << "Migration 4 completed successfully";
    }
    
    // Migration 5: Work Orders table
    if (version < 5) {
        qDebug() << "Running migration 5: Creating orders table...";
        
        if (!query.exec("CREATE TABLE IF NOT EXISTS orders (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  household_id INTEGER NOT NULL,\n"
                       "  order_date TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  requested_cords REAL NOT NULL,\n"
                       "  delivered_cords REAL DEFAULT 0,\n"
                       "  status TEXT DEFAULT 'Pending',\n"
                       "  priority TEXT DEFAULT 'Normal',\n"
                       "  delivery_date TEXT,\n"
                       "  delivery_address TEXT,\n"
                       "  delivery_notes TEXT,\n"
                       "  assigned_driver TEXT,\n"
                       "  payment_method TEXT,\n"
                       "  amount_paid REAL DEFAULT 0,\n"
                       "  notes TEXT,\n"
                       "  created_by TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  updated_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  FOREIGN KEY (household_id) REFERENCES households(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create orders table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create indexes for better performance
        query.exec("CREATE INDEX IF NOT EXISTS idx_orders_household ON orders(household_id);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_orders_status ON orders(status);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_orders_date ON orders(order_date);");
        
        qDebug() << "Orders table created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 5;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 5;
        qDebug() << "Migration 5 completed successfully";
    }
    
    // Migration 6: Enhanced inventory system with categories and equipment tracking
    if (version < 6) {
        qDebug() << "Running migration 6: Creating enhanced inventory system...";
        
        // Create inventory_categories table
        if (!query.exec("CREATE TABLE IF NOT EXISTS inventory_categories (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  name TEXT NOT NULL UNIQUE,\n"
                       "  description TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create inventory_categories table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create new inventory_items table to replace old inventory
        if (!query.exec("CREATE TABLE IF NOT EXISTS inventory_items (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  category_id INTEGER NOT NULL,\n"
                       "  item_name TEXT NOT NULL,\n"
                       "  quantity REAL NOT NULL DEFAULT 0,\n"
                       "  unit TEXT NOT NULL DEFAULT 'units',\n"
                       "  location TEXT,\n"
                       "  notes TEXT,\n"
                       "  last_updated TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  FOREIGN KEY (category_id) REFERENCES inventory_categories(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create inventory_items table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create equipment_maintenance table for tracking service hours
        if (!query.exec("CREATE TABLE IF NOT EXISTS equipment_maintenance (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  equipment_name TEXT NOT NULL,\n"
                       "  current_hours REAL NOT NULL DEFAULT 0,\n"
                       "  next_service_hours REAL NOT NULL DEFAULT 0,\n"
                       "  last_service_date TEXT,\n"
                       "  last_service_notes TEXT,\n"
                       "  alert_threshold_hours REAL DEFAULT 5,\n"
                       "  notes TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  updated_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create equipment_maintenance table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create indexes
        query.exec("CREATE INDEX IF NOT EXISTS idx_inventory_items_category ON inventory_items(category_id);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_inventory_items_name ON inventory_items(item_name);");
        
        // Populate default inventory categories
        QStringList defaultCategories = {"Wood", "Safety Equipment", "Chainsaw Supplies"};
        for (const QString &category : defaultCategories) {
            QSqlQuery insertCat(db);
            insertCat.prepare("INSERT INTO inventory_categories (name) VALUES (:name)");
            insertCat.bindValue(":name", category);
            if (!insertCat.exec()) {
                qDebug() << "ERROR: Failed to insert category" << category << ":" << insertCat.lastError().text();
                db.rollback();
                return;
            }
        }
        
        // Migrate existing inventory data to new structure
        QSqlQuery migrateQuery(db);
        migrateQuery.exec("SELECT id FROM inventory_categories WHERE name = 'Wood'");
        if (migrateQuery.next()) {
            int woodCategoryId = migrateQuery.value(0).toInt();
            
            // Copy old inventory records to new inventory_items as wood entries
            QSqlQuery oldInv(db);
            if (oldInv.exec("SELECT species, form, volume_cords, location FROM inventory WHERE volume_cords > 0")) {
                while (oldInv.next()) {
                    QString species = oldInv.value(0).toString();
                    QString form = oldInv.value(1).toString();
                    double cords = oldInv.value(2).toDouble();
                    QString location = oldInv.value(3).toString();
                    
                    QString itemName = QString("%1 - %2").arg(species.isEmpty() ? "Mixed" : species)
                                                         .arg(form.isEmpty() ? "Split" : form);
                    
                    QSqlQuery insertItem(db);
                    insertItem.prepare("INSERT INTO inventory_items (category_id, item_name, quantity, unit, location) "
                                      "VALUES (:cat_id, :name, :qty, :unit, :location)");
                    insertItem.bindValue(":cat_id", woodCategoryId);
                    insertItem.bindValue(":name", itemName);
                    insertItem.bindValue(":qty", cords);
                    insertItem.bindValue(":unit", "cords");
                    insertItem.bindValue(":location", location);
                    
                    if (!insertItem.exec()) {
                        qDebug() << "Warning: Failed to migrate inventory item:" << insertItem.lastError().text();
                    }
                }
            }
        }
        
        // Add default chainsaw supplies items
        migrateQuery.exec("SELECT id FROM inventory_categories WHERE name = 'Chainsaw Supplies'");
        if (migrateQuery.next()) {
            int chainsawCategoryId = migrateQuery.value(0).toInt();
            
            QStringList chainsawItems = {
                "Chainsaws", "Extra Bars", "Extra Chains", "2-Cycle Oil", 
                "Bar Oil", "Mixed Gas", "Unmixed Gas"
            };
            
            for (const QString &item : chainsawItems) {
                QSqlQuery insertItem(db);
                insertItem.prepare("INSERT INTO inventory_items (category_id, item_name, quantity, unit) "
                                  "VALUES (:cat_id, :name, 0, :unit)");
                insertItem.bindValue(":cat_id", chainsawCategoryId);
                insertItem.bindValue(":name", item);
                insertItem.bindValue(":unit", item.contains("Gas") || item.contains("Oil") ? "gallons" : "units");
                
                if (!insertItem.exec()) {
                    qDebug() << "Warning: Failed to insert chainsaw item:" << insertItem.lastError().text();
                }
            }
        }
        
        // Add default safety equipment items
        migrateQuery.exec("SELECT id FROM inventory_categories WHERE name = 'Safety Equipment'");
        if (migrateQuery.next()) {
            int safetyCategoryId = migrateQuery.value(0).toInt();
            
            QStringList safetyItems = {
                "Safety Glasses", "Hearing Protection", "Work Gloves", 
                "Chainsaw Chaps", "Hard Hats", "First Aid Kits"
            };
            
            for (const QString &item : safetyItems) {
                QSqlQuery insertItem(db);
                insertItem.prepare("INSERT INTO inventory_items (category_id, item_name, quantity, unit) "
                                  "VALUES (:cat_id, :name, 0, 'units')");
                insertItem.bindValue(":cat_id", safetyCategoryId);
                insertItem.bindValue(":name", item);
                
                if (!insertItem.exec()) {
                    qDebug() << "Warning: Failed to insert safety item:" << insertItem.lastError().text();
                }
            }
        }
        
        // Add default equipment (splitter)
        QSqlQuery insertEquip(db);
        insertEquip.prepare("INSERT INTO equipment_maintenance (equipment_name, current_hours, next_service_hours, alert_threshold_hours, notes) "
                           "VALUES ('Log Splitter', 0, 50, 5, 'Service every 50 hours of operation')");
        if (!insertEquip.exec()) {
            qDebug() << "Warning: Failed to insert default equipment:" << insertEquip.lastError().text();
        }
        
        qDebug() << "Enhanced inventory system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 6;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 6;
        qDebug() << "Migration 6 completed successfully";
    }
    
    // Migration 7: Work schedule and volunteer enhancements
    if (version < 7) {
        qDebug() << "Running migration 7: Creating work schedule and certifications tables...";
        
        // Create work_schedule table for scheduling work days
        if (!query.exec("CREATE TABLE IF NOT EXISTS work_schedule (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  work_date TEXT NOT NULL,\n"
                       "  start_time TEXT,\n"
                       "  end_time TEXT,\n"
                       "  activity_type TEXT,\n"
                       "  description TEXT,\n"
                       "  location TEXT,\n"
                       "  volunteer_slots INTEGER DEFAULT 5,\n"
                       "  slots_filled INTEGER DEFAULT 0,\n"
                       "  created_by TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  updated_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create work_schedule table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create work_schedule_signups table for tracking volunteer sign-ups
        if (!query.exec("CREATE TABLE IF NOT EXISTS work_schedule_signups (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  schedule_id INTEGER NOT NULL,\n"
                       "  household_id INTEGER NOT NULL,\n"
                       "  signup_date TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  status TEXT DEFAULT 'Confirmed',\n"
                       "  notes TEXT,\n"
                       "  FOREIGN KEY (schedule_id) REFERENCES work_schedule(id) ON DELETE CASCADE,\n"
                       "  FOREIGN KEY (household_id) REFERENCES households(id) ON DELETE CASCADE,\n"
                       "  UNIQUE(schedule_id, household_id)\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create work_schedule_signups table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create volunteer_certifications table
        if (!query.exec("CREATE TABLE IF NOT EXISTS volunteer_certifications (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  household_id INTEGER NOT NULL,\n"
                       "  certification_name TEXT NOT NULL,\n"
                       "  issue_date TEXT,\n"
                       "  expiration_date TEXT,\n"
                       "  notes TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  FOREIGN KEY (household_id) REFERENCES households(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create volunteer_certifications table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Add availability column to households table
        QStringList newVolunteerColumns = {
            "ALTER TABLE households ADD COLUMN availability TEXT;"
        };
        
        for (const QString &sql : newVolunteerColumns) {
            if (!query.exec(sql)) {
                // Column might already exist, log but continue
                qDebug() << "Note: Could not add column (may already exist):" << query.lastError().text();
            }
        }
        
        // Create indexes
        query.exec("CREATE INDEX IF NOT EXISTS idx_work_schedule_date ON work_schedule(work_date);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_work_signups_schedule ON work_schedule_signups(schedule_id);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_work_signups_household ON work_schedule_signups(household_id);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_certifications_household ON volunteer_certifications(household_id);");
        
        // Insert some sample work days
        QList<QMap<QString, QString>> sampleWorkDays = {
            {
                {"date", QDate::currentDate().addDays(7).toString(Qt::ISODate)},
                {"start", "09:00"},
                {"end", "15:00"},
                {"activity", "Wood Splitting"},
                {"description", "Split and stack firewood for upcoming deliveries"},
                {"location", "Main Yard"},
                {"slots", "6"}
            },
            {
                {"date", QDate::currentDate().addDays(10).toString(Qt::ISODate)},
                {"start", "08:00"},
                {"end", "12:00"},
                {"activity", "Deliveries"},
                {"description", "Help deliver firewood to households"},
                {"location", "Various Locations"},
                {"slots", "4"}
            },
            {
                {"date", QDate::currentDate().addDays(14).toString(Qt::ISODate)},
                {"start", "10:00"},
                {"end", "14:00"},
                {"activity", "Chainsaw Work"},
                {"description", "Cut and process logs (chainsaw certification required)"},
                {"location", "North Lot"},
                {"slots", "3"}
            }
        };
        
        for (const auto &workDay : sampleWorkDays) {
            QSqlQuery insertWork(db);
            insertWork.prepare("INSERT INTO work_schedule (work_date, start_time, end_time, activity_type, "
                              "description, location, volunteer_slots, slots_filled, created_by) "
                              "VALUES (:date, :start, :end, :activity, :description, :location, :slots, 0, 'system')");
            insertWork.bindValue(":date", workDay["date"]);
            insertWork.bindValue(":start", workDay["start"]);
            insertWork.bindValue(":end", workDay["end"]);
            insertWork.bindValue(":activity", workDay["activity"]);
            insertWork.bindValue(":description", workDay["description"]);
            insertWork.bindValue(":location", workDay["location"]);
            insertWork.bindValue(":slots", workDay["slots"]);
            
            if (!insertWork.exec()) {
                qDebug() << "Warning: Failed to insert sample work day:" << insertWork.lastError().text();
            }
        }
        
        qDebug() << "Work schedule system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 7;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 7;
        qDebug() << "Migration 7 completed successfully";
    }
    
    // Migration 8: Profile change requests and employee phone/availability
    if (version < 8) {
        qDebug() << "Running migration 8: Adding profile change requests and employee info...";
        
        // Add phone and availability to users table
        QStringList newUserColumns = {
            "ALTER TABLE users ADD COLUMN phone TEXT;",
            "ALTER TABLE users ADD COLUMN availability TEXT;"
        };
        
        for (const QString &sql : newUserColumns) {
            if (!query.exec(sql)) {
                qDebug() << "Note: Could not add column (may already exist):" << query.lastError().text();
            }
        }
        
        // Create profile_change_requests table
        if (!query.exec("CREATE TABLE IF NOT EXISTS profile_change_requests (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  user_id INTEGER NOT NULL,\n"
                       "  requested_by TEXT NOT NULL,\n"
                       "  field_name TEXT NOT NULL,\n"
                       "  old_value TEXT,\n"
                       "  new_value TEXT,\n"
                       "  status TEXT DEFAULT 'Pending',\n"
                       "  request_date TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  reviewed_by TEXT,\n"
                       "  reviewed_date TEXT,\n"
                       "  notes TEXT,\n"
                       "  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create profile_change_requests table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        query.exec("CREATE INDEX IF NOT EXISTS idx_change_requests_user ON profile_change_requests(user_id);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_change_requests_status ON profile_change_requests(status);");
        
        qDebug() << "Profile change request system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 8;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 8;
        qDebug() << "Migration 8 completed successfully";
    }
    
    // Migration 9: Delivery tracking - mileage, time, auto-inventory update
    if (version < 9) {
        qDebug() << "Running migration 9: Adding delivery tracking fields...";
        
        // Add delivery tracking fields to orders table
        QStringList deliveryColumns = {
            "ALTER TABLE orders ADD COLUMN delivery_time TEXT;",           // Time driver departs
            "ALTER TABLE orders ADD COLUMN start_mileage REAL DEFAULT 0;", // Starting odometer
            "ALTER TABLE orders ADD COLUMN end_mileage REAL DEFAULT 0;",   // Ending odometer
            "ALTER TABLE orders ADD COLUMN completed_date TEXT;"           // When marked complete
        };
        
        for (const QString &sql : deliveryColumns) {
            if (!query.exec(sql)) {
                qDebug() << "Note: Could not add delivery column (may already exist):" << query.lastError().text();
            }
        }
        
        // Create delivery_log table for leads to review
        if (!query.exec("CREATE TABLE IF NOT EXISTS delivery_log (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  order_id INTEGER NOT NULL,\n"
                       "  driver TEXT NOT NULL,\n"
                       "  delivery_date TEXT NOT NULL,\n"
                       "  delivery_time TEXT,\n"
                       "  start_mileage REAL NOT NULL,\n"
                       "  end_mileage REAL NOT NULL,\n"
                       "  total_miles REAL GENERATED ALWAYS AS (end_mileage - start_mileage) STORED,\n"
                       "  delivered_cords REAL NOT NULL,\n"
                       "  client_name TEXT,\n"
                       "  client_address TEXT,\n"
                       "  logged_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create delivery_log table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        query.exec("CREATE INDEX IF NOT EXISTS idx_delivery_log_driver ON delivery_log(driver);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_delivery_log_date ON delivery_log(delivery_date);");
        
        qDebug() << "Delivery tracking system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 9;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 9;
        qDebug() << "Migration 9 completed successfully";
    }
    
    // Migration 10: Inventory alert levels
    if (version < 10) {
        qDebug() << "Running migration 10: Inventory Alert Levels";
        
        // Add alert level columns to inventory_items
        QStringList alertColumns = {
            "ALTER TABLE inventory_items ADD COLUMN reorder_level REAL DEFAULT 0;",    // When to reorder
            "ALTER TABLE inventory_items ADD COLUMN emergency_level REAL DEFAULT 0;"   // Critical level
        };
        
        for (const QString &sql : alertColumns) {
            if (!query.exec(sql)) {
                qDebug() << "Note: Could not add inventory alert column (may already exist):" << query.lastError().text();
            }
        }
        
        qDebug() << "Inventory alert system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 10;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 10;
        qDebug() << "Migration 10 completed successfully";
    }
    
    // Migration 11: Bookkeeping and Financial Tracking
    if (version < 11) {
        qDebug() << "Running migration 11: Creating bookkeeping and financial tracking tables...";
        
        // Create expenses table
        if (!query.exec("CREATE TABLE IF NOT EXISTS expenses (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  date TEXT NOT NULL,\n"
                       "  category TEXT NOT NULL,\n"  // fuel, maintenance, wood_purchase, admin, utilities, insurance, equipment
                       "  amount REAL NOT NULL,\n"
                       "  description TEXT,\n"
                       "  vendor TEXT,\n"
                       "  receipt_path TEXT,\n"
                       "  payment_method TEXT,\n"      // cash, check, card, bank_transfer
                       "  created_by TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  updated_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create expenses table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create income table
        if (!query.exec("CREATE TABLE IF NOT EXISTS income (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  date TEXT NOT NULL,\n"
                       "  source TEXT NOT NULL,\n"     // donation, grant, wood_sales, fundraiser, other
                       "  amount REAL NOT NULL,\n"
                       "  description TEXT,\n"
                       "  donor_name TEXT,\n"
                       "  tax_deductible INTEGER DEFAULT 1,\n"
                       "  receipt_issued INTEGER DEFAULT 0,\n"
                       "  created_by TEXT,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,\n"
                       "  updated_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create income table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create budget_categories table for planning
        if (!query.exec("CREATE TABLE IF NOT EXISTS budget_categories (\n"
                       "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                       "  category_name TEXT NOT NULL UNIQUE,\n"
                       "  category_type TEXT NOT NULL,\n"  // expense or income
                       "  annual_budget REAL DEFAULT 0,\n"
                       "  description TEXT,\n"
                       "  active INTEGER DEFAULT 1,\n"
                       "  created_at TEXT DEFAULT CURRENT_TIMESTAMP\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create budget_categories table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Create indexes for better performance
        query.exec("CREATE INDEX IF NOT EXISTS idx_expenses_date ON expenses(date);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_expenses_category ON expenses(category);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_income_date ON income(date);");
        query.exec("CREATE INDEX IF NOT EXISTS idx_income_source ON income(source);");
        
        // Insert default expense categories
        QStringList expenseCategories = {
            "fuel", "maintenance", "wood_purchase", "admin", "utilities", 
            "insurance", "equipment", "supplies", "marketing", "other"
        };
        
        for (const QString &category : expenseCategories) {
            QSqlQuery insertCat(db);
            insertCat.prepare("INSERT INTO budget_categories (category_name, category_type, description) "
                             "VALUES (:name, 'expense', :desc)");
            insertCat.bindValue(":name", category);
            insertCat.bindValue(":desc", QString("Default %1 expense category").arg(category));
            
            if (!insertCat.exec()) {
                qDebug() << "Warning: Failed to insert expense category" << category << ":" << insertCat.lastError().text();
            }
        }
        
        // Insert default income categories
        QStringList incomeCategories = {
            "donation", "grant", "wood_sales", "fundraiser", "other"
        };
        
        for (const QString &category : incomeCategories) {
            QSqlQuery insertCat(db);
            insertCat.prepare("INSERT INTO budget_categories (category_name, category_type, description) "
                             "VALUES (:name, 'income', :desc)");
            insertCat.bindValue(":name", category);
            insertCat.bindValue(":desc", QString("Default %1 income category").arg(category));
            
            if (!insertCat.exec()) {
                qDebug() << "Warning: Failed to insert income category" << category << ":" << insertCat.lastError().text();
            }
        }
        
        qDebug() << "Bookkeeping system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 11;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 11;
        qDebug() << "Migration 11 completed successfully";
    }
    
    // Migration 12: Unified User System - Consolidate users and households
    if (version < 12) {
        qDebug() << "Running migration 12: Creating unified user system...";
        
        // First, add new columns to users table to support all household data
        QStringList userColumns = {
            "ALTER TABLE users ADD COLUMN address TEXT;",
            "ALTER TABLE users ADD COLUMN mailing_address TEXT;",
            "ALTER TABLE users ADD COLUMN gate_code TEXT;",
            "ALTER TABLE users ADD COLUMN stove_size TEXT;",
            "ALTER TABLE users ADD COLUMN is_volunteer INTEGER DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN waiver_signed INTEGER DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN has_license INTEGER DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN has_working_vehicle INTEGER DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN works_for_wood INTEGER DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN wood_credit_received REAL DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN credit_balance REAL DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN last_volunteer_date TEXT;",
            "ALTER TABLE users ADD COLUMN order_count INTEGER DEFAULT 0;",
            "ALTER TABLE users ADD COLUMN last_order_date TEXT;"
        };
        
        for (const QString &sql : userColumns) {
            if (!query.exec(sql)) {
                qDebug() << "Note: Could not add user column (may already exist):" << query.lastError().text();
            }
        }
        
        // Add user_type column and update existing users
        if (!query.exec("ALTER TABLE users ADD COLUMN user_type TEXT DEFAULT 'client';")) {
            qDebug() << "Note: Could not add user_type column (may already exist):" << query.lastError().text();
        }
        
        // Update existing users to have proper user_type based on role
        QStringList roleUpdates = {
            "UPDATE users SET user_type = 'admin' WHERE role = 'admin';",
            "UPDATE users SET user_type = 'lead' WHERE role = 'lead';",
            "UPDATE users SET user_type = 'employee' WHERE role = 'employee' OR role = 'user';",
            "UPDATE users SET user_type = 'volunteer' WHERE role = 'volunteer';"
        };
        
        for (const QString &updateSql : roleUpdates) {
            if (!query.exec(updateSql)) {
                qDebug() << "Warning: Failed to update user type:" << query.lastError().text();
            }
        }
        
        // Migrate household data to users table
        QSqlQuery migrateHouseholds(db);
        if (migrateHouseholds.exec("SELECT * FROM households")) {
            while (migrateHouseholds.next()) {
                int householdId = migrateHouseholds.value(0).toInt();
                QString name = migrateHouseholds.value(1).toString();
                QString address = migrateHouseholds.value(2).toString();
                QString phone = migrateHouseholds.value(3).toString();
                QString notes = migrateHouseholds.value(4).toString();
                QString createdAt = migrateHouseholds.value(5).toString();
                
                // Check if a user already exists with this name/phone
                QSqlQuery checkUser(db);
                checkUser.prepare("SELECT id FROM users WHERE full_name = :name OR phone = :phone");
                checkUser.bindValue(":name", name);
                checkUser.bindValue(":phone", phone);
                
                if (checkUser.exec() && checkUser.next()) {
                    // User exists, update with household data
                    int userId = checkUser.value(0).toInt();
                    QSqlQuery updateUser(db);
                    updateUser.prepare("UPDATE users SET address = :address, phone = :phone, user_type = 'client' WHERE id = :id");
                    updateUser.bindValue(":address", address);
                    updateUser.bindValue(":phone", phone);
                    updateUser.bindValue(":id", userId);
                    
                    if (!updateUser.exec()) {
                        qDebug() << "Warning: Failed to update user with household data:" << updateUser.lastError().text();
                    }
                } else {
                    // Create new user from household
                    QSqlQuery insertUser(db);
                    insertUser.prepare("INSERT INTO users (full_name, address, phone, user_type, active, created_at) "
                                      "VALUES (:name, :address, :phone, 'client', 1, :created_at)");
                    insertUser.bindValue(":name", name);
                    insertUser.bindValue(":address", address);
                    insertUser.bindValue(":phone", phone);
                    insertUser.bindValue(":created_at", createdAt);
                    
                    if (!insertUser.exec()) {
                        qDebug() << "Warning: Failed to insert user from household:" << insertUser.lastError().text();
                    }
                }
            }
        }
        
        // Create mapping table to track household_id -> user_id relationships
        if (!query.exec("CREATE TABLE IF NOT EXISTS household_user_mapping (\n"
                       "  household_id INTEGER PRIMARY KEY,\n"
                       "  user_id INTEGER NOT NULL,\n"
                       "  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE\n"
                       ");")) {
            qDebug() << "ERROR: Failed to create household_user_mapping table:" << query.lastError().text();
            db.rollback();
            return;
        }
        
        // Populate mapping table
        QSqlQuery populateMapping(db);
        if (populateMapping.exec("SELECT h.id, h.name, h.phone FROM households h")) {
            while (populateMapping.next()) {
                int householdId = populateMapping.value(0).toInt();
                QString name = populateMapping.value(1).toString();
                QString phone = populateMapping.value(2).toString();
                
                QSqlQuery findUser(db);
                findUser.prepare("SELECT id FROM users WHERE full_name = :name OR phone = :phone LIMIT 1");
                findUser.bindValue(":name", name);
                findUser.bindValue(":phone", phone);
                
                if (findUser.exec() && findUser.next()) {
                    int userId = findUser.value(0).toInt();
                    
                    QSqlQuery insertMapping(db);
                    insertMapping.prepare("INSERT INTO household_user_mapping (household_id, user_id) VALUES (:household_id, :user_id)");
                    insertMapping.bindValue(":household_id", householdId);
                    insertMapping.bindValue(":user_id", userId);
                    
                    if (!insertMapping.exec()) {
                        qDebug() << "Warning: Failed to insert household mapping:" << insertMapping.lastError().text();
                    }
                }
            }
        }
        
        qDebug() << "Unified user system created successfully";
        
        QSqlQuery up(db);
        if (!up.exec("UPDATE schema_version SET version = 12;")) {
            qDebug() << "ERROR: Failed to update schema version:" << up.lastError().text();
            db.rollback();
            return;
        }
        version = 12;
        qDebug() << "Migration 12 completed successfully";
    }
    
    if (!db.commit()) {
        qDebug() << "ERROR: Failed to commit transaction:" << db.lastError().text();
        return;
    }
    
    qDebug() << "All migrations completed successfully";
}

QSqlDatabase openDefaultConnection() {
    qDebug() << "Opening default database connection...";
    
    QString dbPath;
    ensureAppDataPath(dbPath);
    
    if (dbPath.isEmpty()) {
        qDebug() << "ERROR: Database path is empty!";
        return QSqlDatabase();
    }
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "ERROR: Failed to open database:" << db.lastError().text();
        qDebug() << "Database path:" << dbPath;
        return QSqlDatabase();
    }
    
    qDebug() << "Database opened successfully";
    runMigrations(db);
    
    if (!db.isOpen()) {
        qDebug() << "ERROR: Database connection lost after migrations!";
        return QSqlDatabase();
    }
    
    qDebug() << "Database connection established successfully";
    return db;
}

bool loadSqlScript(const QString &filePath, QSqlDatabase &db) {
    qDebug() << "Loading SQL script from:" << filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR: Failed to open SQL file:" << filePath;
        return false;
    }
    
    QString scriptContent = QString::fromUtf8(file.readAll());
    file.close();
    
    if (scriptContent.isEmpty()) {
        qDebug() << "WARNING: SQL file is empty:" << filePath;
        return false;
    }
    
    // Remove all comment lines first to avoid issues with splitting
    QStringList lines = scriptContent.split('\n');
    QStringList cleanedLines;
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        // Skip comment-only lines and empty lines
        if (!trimmedLine.isEmpty() && !trimmedLine.startsWith("--")) {
            cleanedLines.append(line);
        }
    }
    QString cleanedContent = cleanedLines.join('\n');
    
    // Split by semicolons to handle multiple statements
    QStringList statements = cleanedContent.split(';', Qt::SkipEmptyParts);
    
    qDebug() << "✅ SQL file loaded successfully";
    qDebug() << "   Total characters read:" << scriptContent.length();
    qDebug() << "   Total statements found after cleaning:" << statements.size();
    
    // Don't use transaction - execute statements individually to avoid rollback on errors
    QSqlQuery query(db);
    int successCount = 0;
    int failCount = 0;
    int skippedCount = 0;
    QStringList failedStatements;
    
    for (const QString &statement : statements) {
        QString trimmed = statement.trimmed();
        
        // Skip empty statements
        if (trimmed.isEmpty()) {
            skippedCount++;
            continue;
        }
        
        // Skip SELECT statements (they're just for display in the script)
        if (trimmed.toUpper().startsWith("SELECT")) {
            skippedCount++;
            continue;
        }
        
        qDebug() << "\n🔵 Executing:" << trimmed.left(80) << "...";
        
        if (!query.exec(trimmed)) {
            QString errorMsg = query.lastError().text();
            qDebug() << "  ❌ FAILED!";
            qDebug() << "     Error:" << errorMsg;
            qDebug() << "     Statement:" << trimmed.left(200);
            failCount++;
            failedStatements.append(QString("Error: %1\nStatement: %2").arg(errorMsg).arg(trimmed.left(100)));
        } else {
            int rowsAffected = query.numRowsAffected();
            qDebug() << "  ✅ Success! Rows affected:" << rowsAffected;
            successCount++;
        }
    }
    
    qDebug() << "\n========================================";
    qDebug() << "STATEMENT PROCESSING SUMMARY";
    qDebug() << "========================================";
    qDebug() << "  ✅ Executed successfully:" << successCount;
    qDebug() << "  ❌ Failed:" << failCount;
    qDebug() << "  ⏭️  Skipped (comments/empty):" << skippedCount;
    
    if (failCount > 0) {
        qDebug() << "\n⚠️  FAILED STATEMENTS:";
        for (int i = 0; i < failedStatements.size() && i < 5; ++i) {
            qDebug() << "\n" << (i + 1) << "." << failedStatements[i];
        }
        if (failedStatements.size() > 5) {
            qDebug() << "\n... and" << (failedStatements.size() - 5) << "more failed statements";
        }
    }
    qDebug() << "========================================\n";
    
    qDebug() << "SQL script execution completed:";
    qDebug() << "  Successful statements:" << successCount;
    qDebug() << "  Failed statements:" << failCount;
    
    return successCount > 0;
}

bool loadSampleData() {
    qDebug() << "Loading sample data from SAMPLE_DATA.sql...";
    
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        qDebug() << "ERROR: No database connection available";
        return false;
    }
    
    // Create sample_data_status table if it doesn't exist
    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS sample_data_status ("
                   "  id INTEGER PRIMARY KEY,"
                   "  loaded_at TEXT DEFAULT CURRENT_TIMESTAMP,"
                   "  file_path TEXT,"
                   "  records_loaded INTEGER DEFAULT 0,"
                   "  database_modified INTEGER DEFAULT 0"
                   ");")) {
        qDebug() << "WARNING: Could not create sample_data_status table:" << query.lastError().text();
    }
    
    // Check if sample data has already been loaded
    bool alreadyLoaded = false;
    bool databaseModified = false;
    if (query.exec("SELECT COUNT(*) FROM sample_data_status") && query.next()) {
        alreadyLoaded = query.value(0).toInt() > 0;
    }
    
    // Check if database has been modified (has real data)
    int clientCount = 0, orderCount = 0;
    if (query.exec("SELECT COUNT(*) FROM households") && query.next()) {
        clientCount = query.value(0).toInt();
    }
    if (query.exec("SELECT COUNT(*) FROM orders") && query.next()) {
        orderCount = query.value(0).toInt();
    }
    
    databaseModified = (clientCount > 30 || orderCount > 20); // Threshold for "real" data
    
    if (alreadyLoaded) {
        qDebug() << "⚠️ Sample data has already been loaded";
        if (databaseModified) {
            qDebug() << "⚠️ Database appears to have been modified with real data";
            qDebug() << "   Clients:" << clientCount << "Orders:" << orderCount;
            qDebug() << "   Consider backing up before reloading sample data";
        }
        // Return true but let UI handle the warning/confirmation
        return true;
    }
    
    // Try multiple possible paths for the SQL file
    QStringList possiblePaths = {
        "docs/SAMPLE_DATA.sql",
        "../docs/SAMPLE_DATA.sql",
        "../../docs/SAMPLE_DATA.sql",
        QCoreApplication::applicationDirPath() + "/docs/SAMPLE_DATA.sql",
        QCoreApplication::applicationDirPath() + "/../docs/SAMPLE_DATA.sql",
        QCoreApplication::applicationDirPath() + "/../../docs/SAMPLE_DATA.sql",
        QCoreApplication::applicationDirPath() + "/../../../docs/SAMPLE_DATA.sql"
    };
    
    QString sqlFilePath;
    for (const QString &path : possiblePaths) {
        QFileInfo fileInfo(path);
        qDebug() << "Checking path:" << fileInfo.absoluteFilePath();
        if (fileInfo.exists()) {
            sqlFilePath = fileInfo.absoluteFilePath();
            qDebug() << "✅ Found SAMPLE_DATA.sql at:" << sqlFilePath;
            break;
        }
    }
    
    if (sqlFilePath.isEmpty()) {
        qDebug() << "ERROR: Could not find SAMPLE_DATA.sql file";
        qDebug() << "Tried these locations:";
        for (const QString &path : possiblePaths) {
            qDebug() << "  ❌" << QFileInfo(path).absoluteFilePath();
        }
        return false;
    }
    
    // Load the sample data
    bool success = loadSqlScript(sqlFilePath, db);
    
    if (success) {
        // Record that sample data has been loaded
        QSqlQuery insertStatus(db);
        insertStatus.prepare("INSERT INTO sample_data_status (file_path, database_modified) "
                           "VALUES (:path, :modified)");
        insertStatus.bindValue(":path", sqlFilePath);
        insertStatus.bindValue(":modified", databaseModified ? 1 : 0);
        
        if (!insertStatus.exec()) {
            qDebug() << "WARNING: Could not record sample data status:" << insertStatus.lastError().text();
        } else {
            qDebug() << "✅ Sample data loaded successfully and status recorded";
        }
    }
    
    return success;
}

/**
 * @brief Check if sample data has been loaded and get status
 * @return QVariantMap with status information
 */
QVariantMap getSampleDataStatus() {
    QVariantMap status;
    status["loaded"] = false;
    status["modified"] = false;
    status["client_count"] = 0;
    status["order_count"] = 0;
    status["loaded_at"] = QString();
    
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen()) {
        return status;
    }
    
    QSqlQuery query(db);
    
    // Check if sample data status table exists and has records
    if (query.exec("SELECT loaded_at, database_modified FROM sample_data_status ORDER BY id DESC LIMIT 1") 
        && query.next()) {
        status["loaded"] = true;
        status["loaded_at"] = query.value(0).toString();
        status["modified"] = query.value(1).toBool();
    }
    
    // Get current record counts
    if (query.exec("SELECT COUNT(*) FROM households") && query.next()) {
        status["client_count"] = query.value(0).toInt();
    }
    if (query.exec("SELECT COUNT(*) FROM orders") && query.next()) {
        status["order_count"] = query.value(0).toInt();
    }
    
    return status;
}

} // namespace firewood::db
