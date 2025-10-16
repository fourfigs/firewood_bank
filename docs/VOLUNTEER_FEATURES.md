# Volunteer Profile & Work Day Sign-Up System

## Overview

The Firewood Bank now has a comprehensive volunteer management system that provides volunteers with their own personalized profile dashboard and the ability to sign up for scheduled work days.

## Key Features Implemented

### 📋 Volunteer Profile Card

When volunteers log in, they see a complete profile view with all their information:

#### Personal Information
- **Name** - Volunteer's full name
- **Physical Address** - Home address
- **Mailing Address** - If different from physical address
- **Phone Number** - Contact phone
- **Email** - Email address
- **Availability** - When they're available to volunteer (read-only, updated by office)

#### Status & Requirements
- ☑️ **Waiver Signed** - Shows if liability waiver is on file
- ☑️ **Driver's License** - Has valid driver's license
- ☑️ **Working Vehicle** - Has access to a vehicle

#### Volunteer Hours Log
- Complete history of volunteer hours
- Columns: Date, Hours, Activity
- **Total hours displayed** at bottom
- Shows last 20 volunteer sessions

#### Certifications
- Display of all volunteer certifications
- Columns: Certification Name, Issue Date, Expiration Date
- **+ Add Certification** button - volunteers can self-report certifications
  - Chainsaw safety
  - First aid
  - CPR
  - Forklift operation
  - Any other relevant certifications

### 📅 Work Day Sign-Up System

#### Available Work Days Table
Shows all upcoming work days with:
- **Date** - When the work day is scheduled
- **Time** - Start and end time (e.g., "09:00 - 15:00")
- **Activity** - Type of work (Wood Splitting, Deliveries, Chainsaw Work, etc.)
- **Location** - Where the work takes place
- **Slots** - Visual indicator showing filled/total slots
  - 🟢 Green: Plenty of spots available
  - 🟡 Yellow: Only 1-2 spots left
  - 🔴 Red: FULL
- **My Status** - Shows if volunteer is signed up

#### Sign-Up Actions
- ✅ **Sign Up for Selected Work Day** - Register for a work day
  - Confirmation dialog shows date, time, and activity
  - Prevents double sign-up
  - Warns if work day is full
  
- ❌ **Cancel My Sign-Up** - Remove yourself from a work day
  - Confirmation required
  - Only works if you're currently signed up
  - Frees up slot for other volunteers

- 🔄 **Refresh** - Reload all data to see latest updates

## Database Schema

### New Tables

#### `work_schedule`
Stores scheduled work days.
```sql
- id (Primary Key)
- work_date (Date of work day)
- start_time (e.g., "09:00")
- end_time (e.g., "15:00")
- activity_type (Wood Splitting, Deliveries, etc.)
- description (Detailed description)
- location (Where work happens)
- volunteer_slots (Total spots available)
- slots_filled (Current sign-ups)
- created_by (Who scheduled it)
- created_at, updated_at
```

#### `work_schedule_signups`
Tracks volunteer sign-ups.
```sql
- id (Primary Key)
- schedule_id (FK to work_schedule)
- household_id (FK to households - the volunteer)
- signup_date (When they signed up)
- status (Confirmed, Cancelled, Completed)
- notes
- UNIQUE constraint prevents double sign-up
```

#### `volunteer_certifications`
Stores volunteer certifications.
```sql
- id (Primary Key)
- household_id (FK to households)
- certification_name (e.g., "Chainsaw Safety")
- issue_date
- expiration_date
- notes
- created_at
```

### Modified Tables

#### `households`
Added new column:
- `availability` (TEXT) - Stores volunteer's general availability

## Sample Work Days

The system pre-populates with three sample work days:

1. **Wood Splitting** (7 days from now)
   - Time: 09:00 - 15:00
   - Location: Main Yard
   - Description: Split and stack firewood for upcoming deliveries
   - Slots: 6

2. **Deliveries** (10 days from now)
   - Time: 08:00 - 12:00
   - Location: Various Locations
   - Description: Help deliver firewood to households
   - Slots: 4

3. **Chainsaw Work** (14 days from now)
   - Time: 10:00 - 14:00
   - Location: North Lot
   - Description: Cut and process logs (chainsaw certification required)
   - Slots: 3

## User Experience

### For Volunteers

1. **Login** with volunteer credentials (username: `volunteer`, password: `volunteer`)
2. **View Profile** - See all personal information and status
3. **Check Hours** - Review volunteer hour history and total
4. **Manage Certifications** - View and add certifications
5. **Browse Work Days** - See all upcoming opportunities
6. **Sign Up** - Register for work days with available slots
7. **Track Sign-Ups** - See which days you're signed up for
8. **Cancel** - Remove yourself if plans change

### For Admins/Employees

Admins and employees can (future enhancement):
- Create new work days
- Edit work day details
- View who's signed up
- Cancel work days
- Mark work days as completed
- Record volunteer hours from work days

## Integration with Inventory System

### Admin Can Add New Inventory Items ✓

The admin already has the ability to add new inventory items through the `InventoryDialog`:

1. Click "📦 Add Inventory" in toolbar
2. Select category (Wood, Safety Equipment, Chainsaw Supplies)
3. Click "+ New Item" to create a new item type
4. Enter name, quantity, unit, location
5. Save

This allows the system to grow organically as new item types are needed.

## Files Created/Modified

### New Files
- `src/ui/VolunteerProfileWidget.h`
- `src/ui/VolunteerProfileWidget.cpp`
- `docs/VOLUNTEER_FEATURES.md` (this file)

### Modified Files
- `src/db/database.cpp` - Added Migration 7
- `src/ui/MainWindow.cpp` - Integrated volunteer profile view
- `src/ui/CMakeLists.txt` - Added new source files

## Testing the System

### As a Volunteer
1. Launch application
2. Login: `volunteer` / `volunteer`
3. You'll see "👤 My Profile" tab (not Dashboard)
4. View your complete profile information
5. Scroll down to see available work days
6. Select a work day and click "✅ Sign Up"
7. Confirm the sign-up
8. See your status change to "✓ Signed Up"
9. Click "🔄 Refresh" to see updated slot counts
10. Try to cancel and sign up again

### As Admin/Employee
1. Login: `admin` / `admin` or `user` / `user`
2. You'll see the standard "🏠 Dashboard"
3. Access to all inventory, clients, and orders
4. Can add new inventory items with "+ New Item"

## Future Enhancements

Potential improvements:
- **Admin Work Day Management** - Create/edit/cancel work days from UI
- **Notifications** - Email reminders for upcoming work days
- **Automatic Hour Logging** - Record hours when work day is completed
- **Waitlist** - Join waitlist when work day is full
- **Calendar View** - Visual calendar of work days
- **Recurring Work Days** - Schedule repeating work days
- **Skill Matching** - Match volunteers to appropriate activities based on certifications
- **Check-in System** - QR code or PIN for volunteers to check in on work day
- **Weather Integration** - Automatic notifications if work day cancelled due to weather

## Security & Permissions

- **Volunteers** - Can only view/edit their own profile and certifications
- **Volunteers** - Can sign up/cancel for work days
- **Volunteers** - CANNOT view other volunteers' information
- **Volunteers** - CANNOT edit personal info (address, phone, etc.) - must contact office
- **Employees** - Can view dashboards and manage inventory/clients/orders
- **Admins** - Full access to all system features

## Migration Notes

- Migration 7 runs automatically on first launch after update
- Creates 3 new tables and 1 new column
- Populates sample work days for testing
- Safe to run on existing databases - only adds, never removes
- Schema version updated from 6 to 7

