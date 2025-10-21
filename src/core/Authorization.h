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
     * @brief Check if a user type has a specific permission
     */
    static bool hasPermission(const QString &userType, Permission permission);
    
    /**
     * @brief Check if user type is admin
     */
    static bool isAdmin(const QString &userType);
    
    /**
     * @brief Check if user type is lead
     */
    static bool isLead(const QString &userType);
    
    /**
     * @brief Check if user type is employee
     */
    static bool isEmployee(const QString &userType);
    
    /**
     * @brief Check if user type is volunteer
     */
    static bool isVolunteer(const QString &userType);
    
    /**
     * @brief Check if user type is client
     */
    static bool isClient(const QString &userType);
    
    /**
     * @brief Check if user can login (has username/password)
     */
    static bool canLogin(const QString &userType);
    
    /**
     * @brief Get display name for user type
     */
    static QString getUserTypeDisplayName(const QString &userType);
    
    /**
     * @brief Get user-friendly permission description
     */
    static QString getPermissionDescription(Permission permission);
};

} // namespace firewood::core

