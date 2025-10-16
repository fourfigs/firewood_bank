#include "database.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
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
        
        // Create default admin user (username: admin, password: admin)
        // Password hash is SHA-256 of "admin"
        QSqlQuery insertAdmin(db);
        insertAdmin.prepare("INSERT INTO users (username, password_hash, role, full_name, email, active) "
                           "VALUES (:username, :password_hash, :role, :full_name, :email, :active)");
        insertAdmin.bindValue(":username", "admin");
        // SHA-256 hash of "admin": 8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918
        insertAdmin.bindValue(":password_hash", "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918");
        insertAdmin.bindValue(":role", "admin");
        insertAdmin.bindValue(":full_name", "System Administrator");
        insertAdmin.bindValue(":email", "admin@firewoodbank.org");
        insertAdmin.bindValue(":active", 1);
        
        if (!insertAdmin.exec()) {
            qDebug() << "ERROR: Failed to create default admin user:" << insertAdmin.lastError().text();
            db.rollback();
            return;
        }
        
        qDebug() << "Default admin user created successfully";
        
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

}


