#include "Authorization.h"

namespace firewood::core {

bool Authorization::hasPermission(const QString &userType, Permission permission)
{
    QString typeLower = userType.toLower();
    
    // Admin has all permissions
    if (typeLower == "admin") {
        return true;
    }
    
    // Lead permissions (between Admin and Employee)
    // Leads can do everything except manage users and system settings
    if (typeLower == "lead") {
        switch (permission) {
            case Permission::ManageUsers:
            case Permission::ManageSystemSettings:
                return false;  // Only admins can do these
            case Permission::EditInventory:  // KEY: Leads CAN edit inventory
            case Permission::ManageAllInventory:
            case Permission::ViewClients:
            case Permission::EditClients:
            case Permission::ManageAllClients:
            case Permission::AddOrders:
            case Permission::EditOrders:
            case Permission::ManageAllOrders:
            case Permission::ViewInventory:
            case Permission::ManageAgencies:
            case Permission::ManageAllVolunteers:
            case Permission::ViewAllReports:
                return true;
            default:
                return false;
        }
    }
    
    // Employee permissions
    // NOTE: Employees can VIEW inventory but NOT EDIT
    if (typeLower == "employee") {
        switch (permission) {
            case Permission::ViewClients:
            case Permission::EditClients:
            case Permission::AddOrders:
            case Permission::EditOrders:
            case Permission::ViewInventory:  // Can view
                return true;
            case Permission::EditInventory:   // CANNOT edit (only Leads and Admins)
            case Permission::ManageAllInventory:
                return false;
            default:
                return false;
        }
    }
    
    // Volunteer permissions (can be both client and volunteer)
    if (typeLower == "volunteer") {
        switch (permission) {
            case Permission::ViewOwnInfo:
            case Permission::EditOwnInfo:
            case Permission::ViewClients:  // Can view client info (including their own)
                return true;
            default:
                return false;
        }
    }
    
    // Client permissions (very restricted - no login access)
    if (typeLower == "client") {
        return false;  // Clients cannot login, so no permissions
    }
    
    return false;
}

bool Authorization::isAdmin(const QString &userType)
{
    return userType.toLower() == "admin";
}

bool Authorization::isLead(const QString &userType)
{
    return userType.toLower() == "lead";
}

bool Authorization::isEmployee(const QString &userType)
{
    return userType.toLower() == "employee";
}

bool Authorization::isVolunteer(const QString &userType)
{
    return userType.toLower() == "volunteer";
}

bool Authorization::isClient(const QString &userType)
{
    return userType.toLower() == "client";
}

bool Authorization::canLogin(const QString &userType)
{
    QString typeLower = userType.toLower();
    // Only admin, lead, employee, and volunteer can login
    // Clients cannot login (they don't have usernames/passwords)
    return typeLower == "admin" || typeLower == "lead" || 
           typeLower == "employee" || typeLower == "volunteer";
}

QString Authorization::getUserTypeDisplayName(const QString &userType)
{
    QString typeLower = userType.toLower();
    if (typeLower == "admin") return "Administrator";
    if (typeLower == "lead") return "Team Lead";
    if (typeLower == "employee") return "Employee";
    if (typeLower == "volunteer") return "Volunteer";
    if (typeLower == "client") return "Client";
    return userType; // Return original if unknown
}

QString Authorization::getPermissionDescription(Permission permission)
{
    switch (permission) {
        case Permission::ManageUsers:
            return "Manage Users";
        case Permission::ManageAllClients:
            return "Manage All Clients";
        case Permission::ManageAgencies:
            return "Manage Agencies";
        case Permission::ManageAllVolunteers:
            return "Manage All Volunteers";
        case Permission::ManageAllInventory:
            return "Manage All Inventory";
        case Permission::ManageAllOrders:
            return "Manage All Orders";
        case Permission::ViewAllReports:
            return "View All Reports";
        case Permission::ManageSystemSettings:
            return "Manage System Settings";
        case Permission::ViewClients:
            return "View Clients";
        case Permission::EditClients:
            return "Edit Clients";
        case Permission::AddOrders:
            return "Add Orders";
        case Permission::EditOrders:
            return "Edit Orders";
        case Permission::ViewInventory:
            return "View Inventory";
        case Permission::EditInventory:
            return "Edit Inventory";
        case Permission::ViewOwnInfo:
            return "View Own Information";
        case Permission::EditOwnInfo:
            return "Edit Own Information";
        default:
            return "Unknown Permission";
    }
}

} // namespace firewood::core

