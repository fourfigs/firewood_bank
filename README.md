# 🔥 Firewood Bank Management System

A comprehensive desktop application for managing community firewood distribution, built with Qt6 and C++17.

## 🌲 About

**Firewood Bank Management System** is a production-ready application designed for the **Northern Mendocino Ecosystem Recovery Alliance (NMERA)** to manage their Community Firewood Bank program. The system provides:

- 📊 **Real-time statistics dashboard** - Track households served, wood delivered, expenses
- 🚚 **Complete delivery tracking** - Mileage logging, auto-inventory updates, delivery logs
- 📦 **Automated inventory management** - Categories, alerts, equipment maintenance
- 👥 **Client and household management** - Comprehensive profiles, volunteer tracking
- 📋 **Work order system** - Auto-population, status tracking, driver assignment
- 🔐 **Role-based access control** - 4 roles: Admin/Lead/Employee/Volunteer
- 💰 **Financial tracking** - Expenses, income, grant reporting
- 📤 **Export capabilities** - CSV exports for accounting and grant applications

## 🛠️ Prerequisites

### Required Software
- **Qt 6.6.3+** (Widgets, Sql modules) - [Download from Qt.io](https://www.qt.io/download)
- **CMake 3.21+** - [Download from cmake.org](https://cmake.org/download/)
- **C++17 compiler:**
  - Windows: MSVC 2019+ (included with Visual Studio)
  - macOS: Xcode Command Line Tools
  - Linux: GCC 7+ or Clang 5+
- **SQLite** (included with Qt)

### System Requirements
- **Windows:** Windows 10/11 (64-bit recommended)
- **macOS:** macOS 10.15+ 
- **Linux:** Ubuntu 18.04+ or equivalent
- **RAM:** 4GB minimum, 8GB recommended
- **Storage:** 500MB for application + database

## 🚀 Quick Start

### 1. Install Qt
Download and install Qt 6.6.3+ from [qt.io](https://www.qt.io/download). Make sure to include:
- Qt Widgets
- Qt SQL
- CMake integration

### 2. Clone and Build
```powershell
# Clone the repository
git clone [repository-url]
cd firewood_bank

# Windows (PowerShell)
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/msvc2019_64"
cmake --build build --config Release

# macOS
cmake -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.6.3/macos"
cmake --build build --config Release

# Linux
cmake -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.6.3/gcc_64"
cmake --build build --config Release
```

### 3. Run the Application
```powershell
# Windows
.\build\bin\Release\firewood_bank.exe

# macOS/Linux
./build/bin/firewood_bank
```

## 🔑 Default Login Credentials

**⚠️ SECURITY WARNING:** Change these passwords immediately in production!

- **Admin:** `admin` / `admin` - Full system access
- **Lead:** `lead` / `lead` - Operational management + inventory editing  
- **Employee:** `user` / `user` - Daily operations, view-only inventory
- **Volunteer:** `volunteer` / `volunteer` - Personal profile only

## 🧪 Testing with Sample Data

### Load Sample Data for Testing
1. **Start the application** and login as `admin` / `admin`
2. **Look for "Load Sample Data" button** in the toolbar or Tools menu
3. **Click "Load Sample Data"** - this will populate the database with:
   - 25+ realistic client households
   - Sample work orders and deliveries
   - Inventory items and categories
   - Volunteer hours and certifications
   - Equipment maintenance records

### Verify Sample Data Loaded
Run the verification script:
```powershell
# Windows PowerShell
.\check_sample_data.ps1

# This will show counts of all loaded data
```

### Sample Data Status Tracking
The system tracks whether sample data has been loaded and prevents duplicate loading:
- ✅ **First time:** Loads all sample data
- ⚠️ **Already loaded:** Shows warning and option to reload
- 🔄 **Modified database:** Detects changes and asks for confirmation

### Reset Database (if needed)
```powershell
# Stop the application first, then:
Remove-Item "$env:APPDATA\Firewood Bank\firewood_bank.sqlite" -Force

# Restart application - fresh database with migrations will be created
```

## 📊 How to Use the System

### 1. First Login (Admin Setup)
1. Login as `admin` / `admin`
2. **IMMEDIATELY change the admin password** (My Profile → Change Password)
3. Load sample data for testing
4. Create additional user accounts as needed

### 2. Daily Operations Workflow
1. **Employees** login and manage clients/orders
2. **Leads** review delivery logs and manage inventory
3. **Volunteers** sign up for work days and log hours
4. **Admin** manages users and exports reports

### 3. Key Workflows
- **New Client:** Clients tab → Add Client → Fill profile
- **New Order:** Work Orders tab → Add Order → Select client (auto-fills)
- **Complete Delivery:** Edit order → Status: "Completed" → Enter mileage
- **Check Inventory:** Inventory tab → View stock levels and alerts
- **Export for Grants:** Tools menu → Export → Choose CSV format

## 🎯 Features by User Role

### 👑 **Admin** (Full Access)
- ✅ User management (create, edit, delete accounts)
- ✅ Direct profile editing (bypass change requests)
- ✅ Statistics dashboard with real-time metrics
- ✅ Complete delivery logs with mileage tracking
- ✅ All export capabilities (CSV, reports)
- ✅ System configuration and sample data loading

### 🎖️ **Lead** (Operational Management)
- ✅ Statistics dashboard and delivery logs
- ✅ **Inventory editing** (key difference from Employee)
- ✅ Equipment maintenance tracking
- ✅ Employee directory access
- ✅ Order management and client oversight
- ✅ Export capabilities for reporting

### 👤 **Employee** (Daily Operations)
- ✅ Client and household management
- ✅ Work order creation and processing
- ✅ **View-only inventory** access
- ✅ Profile change requests (admin approval)
- ❌ Cannot edit inventory levels

### 🙋 **Volunteer** (Self-Service)
- ✅ Personal profile management
- ✅ Work day sign-ups and hour logging
- ✅ Certification tracking
- ✅ Profile change requests
- ❌ No access to client data or operations

## 📁 Project Structure
```
firewood_bank/
├── src/
│   ├── app/           - Application entry point (main.cpp)
│   ├── ui/            - All UI components and dialogs
│   ├── core/          - Authorization and utilities
│   └── db/            - Database management (10 migrations)
├── docs/              - Comprehensive documentation
│   ├── PROGRAM_INDEX.md           - Complete system overview
│   ├── IMPROVEMENT_CHECKLIST.md   - Development roadmap
│   ├── SECURITY_OBFUSCATION_PLAN.md - Security guide
│   └── SAMPLE_DATA.sql            - Test data
└── build/             - CMake build output
```

## 📚 Documentation
Essential documentation in `docs/` directory:
- **`PROGRAM_INDEX.md`** - Complete system analysis and database schema
- **`IMPROVEMENT_CHECKLIST.md`** - Priority roadmap for enhancements
- **`FINAL_FEATURE_LIST.md`** - Complete feature matrix by role
- **`SECURITY_OBFUSCATION_PLAN.md`** - Security hardening guide
- **`DELIVERY_TRACKING_SYSTEM.md`** - Delivery workflow guide

## 🔧 Technology Stack
- **Language:** C++17
- **Framework:** Qt 6.6.3 (Widgets, SQL)
- **Database:** SQLite with **11 migrations** (auto-upgrading schema)
- **Build System:** CMake 3.21+
- **Platform:** Cross-platform (Windows, macOS, Linux)
- **Security:** Role-based access control, prepared statements

## 🚨 Troubleshooting

### Sample Data Not Loading
If "Load Sample Data" fails:
1. **Check file location:** Ensure `docs/SAMPLE_DATA.sql` exists
2. **Check console output:** Look for error messages in terminal
3. **Database permissions:** Ensure write access to `%APPDATA%\Firewood Bank\`
4. **Reset database:** Delete database file and restart (see above)

### Build Issues
```powershell
# Common Qt path issues:
# Windows - check Qt installation path
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/msvc2019_64"

# macOS - check Qt installation
cmake -B build -DCMAKE_PREFIX_PATH="/usr/local/Qt-6.6.3"

# Linux - install Qt development packages
sudo apt-get install qt6-base-dev qt6-tools-dev cmake
```

### Runtime Issues
- **Database locked:** Close all instances of the application
- **Permission denied:** Run as administrator (Windows) or check file permissions
- **Missing Qt libraries:** Ensure Qt bin directory is in PATH

### Getting Help
1. Check `docs/PROGRAM_INDEX.md` for complete system overview
2. Review `docs/IMPROVEMENT_CHECKLIST.md` for known issues
3. Contact developer (see below)

## 🔒 Security Notes

**CRITICAL:** This system contains sensitive client information. Before production use:

1. **Change ALL default passwords** immediately
2. **Enable database encryption** (see `docs/SECURITY_OBFUSCATION_PLAN.md`)
3. **Set up automated backups** with encryption
4. **Restrict file system access** to database location
5. **Review audit logging** requirements

Current security level: **Basic (40%)** - Functional but needs hardening for production.

## 📈 Development Status

- ✅ **Core Features:** Complete and tested
- ✅ **Database Schema:** 10 migrations, stable
- ✅ **User Interface:** Polished with role-based access
- ⚠️ **Security:** Basic implementation, needs hardening
- 🔄 **Sample Data Loading:** Known issues, being improved
- 📋 **Bookkeeping Tab:** Planned enhancement

See `docs/IMPROVEMENT_CHECKLIST.md` for complete development roadmap.

## 📄 License
See `LICENSE` file for details.

---

## 🎯 About the Developer

**This program was created by Kenneth Hanks exclusively for the Northern Mendocino Ecosystem Recovery Alliance.**

If you would like a version tailored for your non-profit organization, you can contact Kenneth at **humboldtfleamarket@gmail.com**

Everything Kenneth does for NMERA has been a donation, including his time helping with the firewood bank. If you would like to buy him some caffeine to fuel more community-focused development, his CashApp is **$pseutheo** (crypto addresses available by request).

*Building software that helps communities thrive.* 🌲🔥🪵

**Cheers!**
