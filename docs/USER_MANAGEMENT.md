# User Management System

## Overview

Admins now have comprehensive user management capabilities including viewing profiles, editing user details, adding new users, and removing/deactivating users.

## Features

### 🔍 View All Users

The User Management dialog displays all users in a searchable table with:
- **Username** - Login username
- **Full Name** - Display name
- **Role** - admin (red), employee (blue), or volunteer
- **Email** - Contact email
- **Status** - ✓ Active or ✗ Inactive
- **Last Login** - Last login timestamp or "Never"

### 🔎 Search Functionality

- **Real-time search** across username, full name, role, and email
- Type in the search box to filter users instantly
- Search is case-insensitive

### 👁️ View User Profile

Double-click any user or click "👁️ View Profile" to see:

**Profile Information:**
- Username
- Full Name
- Role
- Email
- Status (Active/Inactive)
- Account Created Date
- Last Login Date

**Volunteer Information (if applicable):**
- Name
- Phone
- Email
- Address

### ✏️ Edit User

Click "✏️ Edit User" to modify:
- **Username** - Change login name
- **Full Name** - Update display name
- **Role** - Change between admin, employee, volunteer
- **Email** - Update email address
- **Status** - Activate or deactivate account
- **Password** - Optionally change password (leave blank to keep current)

**Notes:**
- Username must be unique
- Changing role immediately affects permissions
- Deactivated users cannot login

### ➕ Add New User

Click "➕ Add User" to create:
- **Username** - Required, must be unique
- **Full Name** - Display name
- **Role** - admin, employee, or volunteer
- **Email** - Contact email
- **Password** - Required, will be hashed with SHA-256

New users are automatically set to "Active" status.

### 🗑️ Remove User

Click "🗑️ Remove User" to deactivate:
- Requires confirmation
- **Deactivates** the account (does not delete)
- User cannot login but records are preserved
- Can be reactivated by editing the user

**Why deactivate instead of delete?**
- Preserves data integrity
- Maintains audit trail
- Can be reversed if needed
- Historical records remain intact

### 🔄 Refresh

Click "🔄 Refresh" to reload the user list with latest data.

## Access Control

**Who can use User Management?**
- ✅ **Admins** - Full access to all features
- ❌ **Employees** - No access to user management
- ❌ **Volunteers** - No access to user management

## How to Access

### As Admin:

1. Login with admin credentials
2. Click **"👥 Manage Users"** in the toolbar
3. User Management dialog opens

## Use Cases

### Viewing All Users
1. Click "👥 Manage Users"
2. Browse the list of all users
3. Use search to find specific users
4. Check who's active/inactive
5. See when users last logged in

### Viewing a User Profile
1. Open User Management
2. Find the user in the table
3. Double-click the user row OR
4. Select the user and click "👁️ View Profile"
5. Review their complete information
6. Close profile when done

### Editing User Details
1. Open User Management
2. Select a user
3. Click "✏️ Edit User"
4. Modify any fields as needed
5. Optionally change password
6. Click "💾 Save"
7. User is updated immediately

### Adding a New Employee
1. Open User Management
2. Click "➕ Add User"
3. Enter username (e.g., "jdoe")
4. Enter full name (e.g., "John Doe")
5. Select role: **employee**
6. Enter email
7. Set initial password
8. Click "💾 Create User"
9. New employee can now login

### Adding a New Volunteer
1. Open User Management
2. Click "➕ Add User"
3. Enter username (should match their household name)
4. Enter full name
5. Select role: **volunteer**
6. Enter email
7. Set initial password
8. Click "💾 Create User"
9. Volunteer can now login to see their profile

### Deactivating a User
1. Open User Management
2. Select the user to remove
3. Click "🗑️ Remove User"
4. Confirm the action
5. User is deactivated (marked inactive)
6. User cannot login
7. Their records remain in the system

### Reactivating a User
1. Open User Management
2. Find the inactive user (shows "✗ Inactive")
3. Click "✏️ Edit User"
4. Check the "Active" checkbox
5. Click "💾 Save"
6. User can now login again

### Changing User Roles
1. Open User Management
2. Select user to promote/demote
3. Click "✏️ Edit User"
4. Change Role dropdown
   - Promote to admin for full access
   - Demote to employee for limited access
   - Change to volunteer for profile-only access
5. Click "💾 Save"
6. Role change takes effect on next login

### Resetting User Password
1. Open User Management
2. Select user who forgot password
3. Click "✏️ Edit User"
4. Enter new password in "New Password" field
5. Click "💾 Save"
6. Inform user of their new password
7. User can login with new password

## Security Features

### Password Security
- All passwords are hashed using **SHA-256**
- Passwords are never stored in plain text
- Passwords are not displayed in the interface
- Password changes require entering new password

### Role-Based Access
- Admins can manage all users
- Employees and volunteers cannot access user management
- Changing a user's role immediately affects their permissions

### Audit Trail
- Created date is recorded for all users
- Last login is tracked
- User records are never deleted (only deactivated)
- Maintains complete history

## Database

### Users Table
All user data is stored in the `users` table:
```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL DEFAULT 'volunteer',
    full_name TEXT,
    email TEXT,
    active INTEGER NOT NULL DEFAULT 1,
    created_at TEXT DEFAULT CURRENT_TIMESTAMP,
    last_login TEXT
);
```

### Roles
- **admin** - Full system access, can manage users
- **employee** - Can manage clients, orders, inventory
- **volunteer** - Can only view their own profile

## Tips

1. **Use descriptive usernames** - Makes it easier to identify users
2. **Keep emails updated** - Useful for future communication features
3. **Deactivate, don't delete** - Preserves data integrity
4. **Regular audits** - Periodically review active users
5. **Strong passwords** - Encourage users to use secure passwords
6. **Role assignment** - Assign the minimum role needed for user's tasks

## Troubleshooting

**User can't login after creation:**
- Check if user is marked "Active"
- Verify username is correct
- Ensure password was set correctly
- Try editing user and setting new password

**Can't find a specific user:**
- Use the search box at the top
- Check if user is in the list (may be inactive)
- Refresh the list

**Changes not appearing:**
- Click "🔄 Refresh" to reload
- Close and reopen User Management dialog
- Logout and login again to see role changes

## Future Enhancements

Potential improvements:
- Email notifications for password resets
- Two-factor authentication
- Password complexity requirements
- Password expiration policies
- Activity logging (who modified what)
- Bulk user import/export
- User groups and teams
- Permission customization
- Session management
- Force logout of specific users

