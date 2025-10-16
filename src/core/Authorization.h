#pragma once

#include <QString>

namespace firewood::core {

/**
 * @brief Authorization helper for role-based access control
 */
class Authorization {
public:
    enum class Permission {
        // Admin permissions
        ManageUsers,
        ManageAllClients,
        ManageAgencies,
        ManageAllVolunteers,
        ManageAllInventory,
        ManageAllOrders,
        ViewAllReports,
        ManageSystemSettings,
        
        // Employee/User permissions
        ViewClients,
        EditClients,
        AddOrders,
        EditOrders,
        ViewInventory,
        EditInventory,
        
        // Volunteer permissions
        ViewOwnInfo,
        EditOwnInfo
    };
    
    /**
     * @brief Check if a role has a specific permission
     */
    static bool hasPermission(const QString &role, Permission permission);
    
    /**
     * @brief Check if user is admin
     */
    static bool isAdmin(const QString &role);
    
    /**
     * @brief Check if user is employee
     */
    static bool isEmployee(const QString &role);
    
    /**
     * @brief Check if user is volunteer
     */
    static bool isVolunteer(const QString &role);
    
    /**
     * @brief Get user-friendly permission description
     */
    static QString getPermissionDescription(Permission permission);
};

} // namespace firewood::core

