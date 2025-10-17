#include "Authorization.h"

namespace firewood::core {

bool Authorization::hasPermission(const QString &role, Permission permission)
{
    QString roleLower = role.toLower();
    
    // Admin has all permissions
    if (roleLower == "admin") {
        return true;
    }
    
    // Lead permissions (between Admin and Employee)
    // Leads can do everything except manage users and system settings
    if (roleLower == "lead") {
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
    
    // Employee/User permissions
    // NOTE: Employees can VIEW inventory but NOT EDIT
    if (roleLower == "employee" || roleLower == "user") {
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
    
    // Volunteer permissions (very restricted)
    if (roleLower == "volunteer") {
        switch (permission) {
            case Permission::ViewOwnInfo:
            case Permission::EditOwnInfo:
                return true;
            default:
                return false;
        }
    }
    
    return false;
}

bool Authorization::isAdmin(const QString &role)
{
    return role.toLower() == "admin";
}

bool Authorization::isLead(const QString &role)
{
    return role.toLower() == "lead";
}

bool Authorization::isEmployee(const QString &role)
{
    QString roleLower = role.toLower();
    return roleLower == "employee" || roleLower == "user";
}

bool Authorization::isVolunteer(const QString &role)
{
    return role.toLower() == "volunteer";
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

