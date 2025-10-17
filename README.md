# 🔥 Firewood Bank Management System

A comprehensive desktop application for managing community firewood distribution, built with Qt6 and C++17.

## 🌲 About

**Firewood Bank Management System** is a production-ready application designed for the **Northern Mendocino Ecosystem Recovery Alliance** to manage their Community Firewood Bank program. The system provides:

- 📊 Real-time statistics dashboard
- 🚚 Complete delivery tracking with mileage logging
- 📦 Automated inventory management
- 👥 Client and household management
- 📋 Work order system with auto-population
- 🔐 Role-based access control (Admin/Lead/Employee/Volunteer)
- 🔧 Equipment maintenance tracking
- 📤 CSV export for accounting and grant reporting

## Prerequisites
- Qt 6.6.3+ (Widgets, Sql modules)
- CMake 3.21+
- C++17 compiler (MSVC 2019+ on Windows, Clang/GCC on macOS/Linux)
- SQLite (included with Qt)

## Build

### Windows (PowerShell)
```powershell
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/msvc2019_64"
cmake --build build --config Release
```

### macOS
```bash
cmake -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.6.3/macos"
cmake --build build --config Release
```

### Linux
```bash
cmake -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.6.3/gcc_64"
cmake --build build --config Release
```

## Run
```powershell
# Windows
.\build\bin\Release\firewood_bank.exe

# macOS/Linux
./build/bin/firewood_bank
```

## Default Login Credentials
- **Admin:** `admin` / `admin`
- **Lead:** `lead` / `lead`
- **Employee:** `user` / `user`
- **Volunteer:** `volunteer` / `volunteer`

## Project Structure
```
firewood_bank/
├── src/
│   ├── app/       - Application entry point
│   ├── ui/        - All UI components and dialogs
│   ├── core/      - Core utilities and authorization
│   └── db/        - Database management and migrations
├── docs/          - Comprehensive documentation
└── build/         - CMake build output
```

## Features

### For Administrators
- Full system access and configuration
- User management
- Direct profile editing
- Statistics dashboard
- Delivery log with export

### For Leads
- Statistics dashboard
- Inventory editing
- Delivery log access
- Order management
- Employee directory

### For Employees
- Client management
- Work order processing
- View-only inventory access

### For Volunteers
- Personal profile management
- Work day sign-ups
- Hours logging
- Certification tracking

## Documentation
See the `docs/` directory for comprehensive documentation:
- `FINAL_FEATURE_LIST.md` - Complete feature matrix
- `SESSION_COMPLETE.md` - Implementation summary
- `DELIVERY_TRACKING_SYSTEM.md` - Delivery workflow guide
- `VISUAL_ENHANCEMENTS_SUMMARY.md` - UI design guide

## Technology Stack
- **Language:** C++17
- **Framework:** Qt 6.6.3 (Widgets, SQL)
- **Database:** SQLite with 9 migrations
- **Build System:** CMake 3.21+
- **Platform:** Cross-platform (Windows, macOS, Linux)

## License
See `LICENSE` file for details.

---

## 🎯 About the Developer

**This program was created by Kenneth Hanks exclusively for the Northern Mendocino Ecosystem Recovery Alliance.**

If you would like a version tailored for your non-profit organization, you can contact Kenneth at **humboldtfleamarket@gmail.com**

Everything Kenneth does for NMERA has been a donation, including his time helping with the firewood bank. If you would like to buy him some caffeine to fuel more community-focused development, his CashApp is **$pseutheo** (crypto addresses available by request).

*Building software that helps communities thrive.* 🌲🔥🪵

Cheers!
