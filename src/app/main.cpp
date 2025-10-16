#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include "MainWindow.h"
#include "LoginDialog.h"
#include "database.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "Starting Firewood Bank application...";
    
    // Set application properties
    app.setApplicationName("Firewood Bank");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("Firewood Bank");
    
    // Open database connection
    QSqlDatabase db = firewood::db::openDefaultConnection();
    if (!db.isOpen()) {
        qDebug() << "CRITICAL: Failed to open database connection!";
        QMessageBox::critical(nullptr, "Database Error", 
                             "Failed to open database connection. The application cannot continue.");
        return 1;
    }
    
<<<<<<< Updated upstream
    // Show login dialog first
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        qDebug() << "Login cancelled or failed. Exiting application.";
        return 0;
    }
    
    QString username = loginDialog.getUsername();
    QString role = loginDialog.getRole();
    
    qDebug() << "Login successful. User:" << username << "Role:" << role;
    
    // Get full name from login dialog (it's loaded during authentication)
    QString fullName = username; // Default to username if full name not available
    
    // Query full name from database
    QSqlQuery query(db);
    query.prepare("SELECT full_name FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        QString dbFullName = query.value(0).toString();
        if (!dbFullName.isEmpty()) {
            fullName = dbFullName;
        }
    }
    
    // Create and show main window after successful login
    auto *window = createMainWindow(username, fullName, role);
    if (!window) {
        qDebug() << "CRITICAL: Failed to create main window!";
        QMessageBox::critical(nullptr, "Application Error", 
                             "Failed to create main window. The application cannot continue.");
        return 1;
    }
    
    window->show();
    qDebug() << "Application started successfully";
    
    return app.exec();
=======
    // Login/logout loop - allows users to logout and login again without restarting app
    while (true) {
        // Show login dialog
        LoginDialog loginDialog;
        if (loginDialog.exec() != QDialog::Accepted) {
            qDebug() << "Login cancelled or failed. Exiting application.";
            return 0;
        }
        
        QString username = loginDialog.getUsername();
        QString role = loginDialog.getRole();
        
        qDebug() << "Login successful. User:" << username << "Role:" << role;
        
        // Get full name from login dialog (it's loaded during authentication)
        QString fullName = username; // Default to username if full name not available
        
        // Query full name from database
        QSqlQuery query(db);
        query.prepare("SELECT full_name FROM users WHERE username = :username");
        query.bindValue(":username", username);
        if (query.exec() && query.next()) {
            QString dbFullName = query.value(0).toString();
            if (!dbFullName.isEmpty()) {
                fullName = dbFullName;
            }
        }
        
        // Create and show main window after successful login
        MainWindow *window = new MainWindow(username, fullName, role);
        if (!window) {
            qDebug() << "CRITICAL: Failed to create main window!";
            QMessageBox::critical(nullptr, "Application Error", 
                                 "Failed to create main window. The application cannot continue.");
            return 1;
        }
        
        // Connect logout signal to allow returning to login
        bool userLoggedOut = false;
        QObject::connect(window, &MainWindow::logoutRequested, [&userLoggedOut]() {
            userLoggedOut = true;
        });
        
        window->show();
        qDebug() << "Application started successfully";
        
        // Run the event loop for this session
        app.exec();
        
        // Clean up the window
        delete window;
        
        // If user didn't logout (closed window instead), exit the application
        if (!userLoggedOut) {
            qDebug() << "Window closed without logout. Exiting application.";
            return 0;
        }
        
        qDebug() << "User logged out. Returning to login screen...";
        // Loop continues, showing login dialog again
    }
    
    return 0;
>>>>>>> Stashed changes
}


