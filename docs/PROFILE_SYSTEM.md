# Profile & Employee Directory System

## Overview

The Firewood Bank now has a comprehensive profile management system that allows:
- **All Users** - View and request changes to their own profile
- **Employees & Admins** - View other employees and their contact information
- **Admins** - Approve/reject profile change requests

## Features

### 👤 My Profile (All Users)

**Access:** Click **"👤 My Profile"** in the toolbar (available to all users)

All users (admin, employee, volunteer) can:
- View their complete profile information
- See current: username, full name, role, email, phone, availability, status
- Request changes to: email, phone, availability
- Submit change requests for admin approval

**Profile Fields:**
- **Username** - Login name (read-only)
- **Full Name** - Display name (admin can change)
- **Role** - admin/employee/volunteer (admin can change)
- **Email** - Contact email (can request change)
- **Phone** - Contact phone (can request change)
- **Availability** - When available to work (can request change)
- **Status** - Active/Inactive (admin only)

**How to Request Changes:**
1. Click "👤 My Profile" in toolbar
2. Update "New Email", "New Phone", or "New Availability" fields
3. Click "📝 Submit Change Request"
4. Admin will review and approve/reject

### 📋 Employee Directory (Employees & Admins)

**Access:** Click **"📋 Employee Directory"** in the toolbar (employees and admins only)

Employees and admins can:
- View all active employees and admins
- See phone numbers and availability
- Search by name, email, or phone
- View detailed contact information

**Directory Shows:**
- Full Name
- Role (admin shown in red/bold)
- Email
- Phone
- Availability

**Features:**
- **Search** - Real-time filtering
- **Double-click** - View detailed profile
- **Refresh** - Update directory

**Use Cases:**
- Find who to contact for specific shifts
- Check who's available on certain days
- Get phone numbers for coordination
- See all team members

### 📝 Profile Change Requests (Admins Only)

**Access:** Click **"📝 Change Requests"** in the toolbar (admin only)

Admins can:
- Review all profile change requests
- Approve changes (updates user profile immediately)
- Reject changes (notifies user)
- Filter by status (Pending/Approved/Rejected/All)
- See request history

**Request Information:**
- Who requested the change
- What field they want to change
- Old value vs new value
- When requested
- Current status
- Who reviewed it (if processed)

**Color Coding:**
- 🟠 **Pending** - Orange (needs review)
- 🟢 **Approved** - Green (change applied)
- 🔴 **Rejected** - Red (change denied)

## Database Schema

### New Table: `profile_change_requests`

Stores all profile change requests:
```sql
CREATE TABLE profile_change_requests (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    requested_by TEXT NOT NULL,
    field_name TEXT NOT NULL,
    old_value TEXT,
    new_value TEXT,
    status TEXT DEFAULT 'Pending',
    request_date TEXT DEFAULT CURRENT_TIMESTAMP,
    reviewed_by TEXT,
    reviewed_date TEXT,
    notes TEXT,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

### Modified Table: `users`

Added new columns:
- `phone TEXT` - User's phone number
- `availability TEXT` - When user is available

## Workflow

### User Requests Profile Change

1. **User** clicks "👤 My Profile"
2. **User** enters new email, phone, or availability
3. **User** clicks "📝 Submit Change Request"
4. Request is created with status "Pending"
5. **User** receives confirmation message
6. **Admin** is notified (shown in Change Requests)

### Admin Approves Change

1. **Admin** clicks "📝 Change Requests"
2. **Admin** sees pending request
3. **Admin** reviews old value vs new value
4. **Admin** clicks "✅ Approve Request"
5. User's profile is updated immediately
6. Request status changes to "Approved"
7. Tracked: who approved and when

### Admin Rejects Change

1. **Admin** clicks "📝 Change Requests"
2. **Admin** reviews request
3. **Admin** clicks "❌ Reject Request"
4. Request status changes to "Rejected"
5. User's profile remains unchanged
6. Tracked: who rejected and when

## Use Cases

### Example 1: Employee Updates Phone Number

**Scenario:** Jane (employee) got a new phone number

1. Jane logs in
2. Clicks "👤 My Profile"
3. Enters new phone in "New Phone" field
4. Clicks "📝 Submit Change Request"
5. Admin Bob reviews the request
6. Bob clicks "✅ Approve Request"
7. Jane's phone number is updated
8. Other employees see updated number in directory

### Example 2: Employee Updates Availability

**Scenario:** John (employee) changed his available days

1. John logs in
2. Clicks "👤 My Profile"
3. Updates "New Availability": "Weekends only, 9AM-6PM"
4. Submits change request
5. Admin reviews and approves
6. Other employees see John is weekend-only in directory

### Example 3: Finding Employee Contact Info

**Scenario:** Sarah needs to call Mike about a delivery

1. Sarah (employee) logs in
2. Clicks "📋 Employee Directory"
3. Searches for "Mike"
4. Sees Mike's phone: (555) 123-4567
5. Sees Mike's availability: "Mon-Fri 8AM-5PM"
6. Calls Mike during his available hours

### Example 4: Admin Reviews Pending Changes

**Scenario:** Admin reviewing end-of-week change requests

1. Admin logs in
2. Clicks "📝 Change Requests"
3. Sees 3 pending requests
4. Reviews each:
   - Email change: approve ✅
   - Phone change: approve ✅
   - Suspicious availability change: reject ❌
5. All processed, users are updated

## Permissions Summary

| Feature | Admin | Employee | Volunteer |
|---------|-------|----------|-----------|
| View My Profile | ✅ | ✅ | ✅ |
| Request Profile Changes | ✅ | ✅ | ✅ |
| View Employee Directory | ✅ | ✅ | ❌ |
| Review Change Requests | ✅ | ❌ | ❌ |
| Direct Profile Edit (via User Management) | ✅ | ❌ | ❌ |

**Note:** Admins have all employee privileges plus admin-only features.

## Benefits

### For All Users
- ✅ Self-service profile updates
- ✅ Transparency in change process
- ✅ See own information at any time
- ✅ Control over personal data

### For Employees
- ✅ Easy access to colleague contact info
- ✅ See who's available when
- ✅ Better team coordination
- ✅ Find people quickly

### For Admins
- ✅ Approve changes safely
- ✅ Review before applying
- ✅ Maintain data quality
- ✅ Audit trail of all changes
- ✅ Can reject inappropriate changes

## Tips

**For Users:**
1. Keep email and phone current for communication
2. Update availability when schedule changes
3. Be specific in availability (days and times)
4. Submit changes promptly when info changes

**For Employees:**
5. Check directory before calling colleagues
6. Respect availability times
7. Use search to find people quickly
8. Update your own info regularly

**For Admins:**
9. Review change requests regularly
10. Verify changes seem legitimate
11. Reject spam or inappropriate requests
12. Keep change request queue clear

## Security & Privacy

### What's Protected
- Users can only view their own full profile
- Users can only request changes to their own profile
- Employees see limited info (no full profile access)
- Only admins can directly edit profiles

### What's Visible
- **To Employees:** Name, role, email, phone, availability of other employees
- **To Volunteers:** Only their own profile
- **To Admins:** Everything

### Change Tracking
- All change requests are logged
- Who requested, what changed, when
- Who approved/rejected and when
- Complete audit trail

## Troubleshooting

**My change request disappeared:**
- Check with admin - it may have been processed
- Click "👤 My Profile" to see if change was applied
- If approved, your profile shows new value

**I don't see Employee Directory:**
- Feature is for employees and admins only
- Volunteers don't have access
- Contact admin if you need employee info

**Change request stuck as Pending:**
- Admin hasn't reviewed yet
- Contact admin to request review
- May take 1-2 business days

**Can't update my name or role:**
- These fields require admin direct edit
- Contact admin via User Management
- Not available through change requests

## Future Enhancements

Potential improvements:
- Email notifications when changes approved/rejected
- Bulk approve multiple requests
- Change request comments/notes
- Profile photos
- Department/team organization
- Work schedule integration
- Automatic approval for certain fields
- User notifications for pending requests

