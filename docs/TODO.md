# Firewood Bank  TODO

## High-level outline
- Confirm stack and architecture (Qt 6 C++ + SQLite)
- Initialize Qt project with CMake and repository structure
- Implement database layer and migrations
- Implement secure auth, roles, and audit logging
- Build Clients module (CRUD, search, encrypted fields)
- Build Volunteers and Hours module
- Build Work-for-Wood credits module
- Build Orders and Deliveries workflow
- Build Inventory module
- Build Assets and Maintenance module
- Implement Reporting dashboards and exports
- Implement Backup, Export, and Import
- Scaffold Sync for future multi-device support
- Testing and QA coverage
- Packaging for Windows and macOS
- Documentation for users and developers

## Detailed tasks
### Architecture
- Choose Qt Widgets vs QML and finalize UI approach
- Select crypto libraries (Argon2, libsodium) and key storage
- Define repo structure, modules, and naming conventions
- Define data retention, backups, and privacy policies

### Project init
- Create CMake project files and Qt app skeleton
- Add folder structure: app, db, ui, core, tests
- Implement main window, navigation, and theme
- Configure logging, configuration, and environment handling

### Database
- Create SQLite schema versioning and migration runner
- Design tables for clients, volunteers, orders, inventory
- Design tables for assets, maintenance, reports, audit
- Create indices, constraints, and foreign keys
- Add seed data and fixtures for development

### Auth
- Implement password hashing with Argon2
- Add role-based authorization and permissions checks
- Encrypt sensitive fields at rest using libsodium
- Implement audit log for key entity changes
- Build login, logout, and session management

### Clients
- Implement Clients CRUD UI and forms validation
- Add client search, filters, and pagination
- Encrypt gate codes, notes, and sensitive directions
- Add referral sources management and linking

### Volunteers & Hours
- Implement Volunteers CRUD and attributes
- Implement hour logging and activities
- Track licenses, trucks, equipment, trainings, waivers
- Add emergency contact and compliance fields

### Work-for-Wood
- Define hourly rate policy and conversion to credits
- Implement credit balance ledger and redemptions
- Link credits to orders and inventory issuance

### Orders & Deliveries
- Implement Orders creation with requested cords and priority
- Add scheduling, assignment, and delivery tracking
- Capture delivered cords and proof of delivery

### Inventory
- Implement Inventory items, categories, and units
- Implement inventory transactions and reasons
- Track wood on hand split and not split
- Implement min/target levels and alerts

### Assets & Maintenance
- Implement Assets for chainsaws, splitters, PPE
- Implement maintenance schedules and reminders
- Track maintenance logs and costs

### Reports
- Build reports: wood delivered by period
- Build reports: households served and unique clients
- Build reports: volunteer hours by period
- Build reports: expenses and inventory spend
- Add CSV export for reports

### Backup/Import
- Implement encrypted backups and scheduled backup tasks
- Add CSV import templates for initial data entry
- Add data validation and import preview

### Sync
- Design sync-friendly IDs and change tracking
- Implement local changelog and conflict strategy
- Abstract data access for future sync transport

### Testing
- Set up unit tests for core and db
- Set up UI tests for key flows
- Add test fixtures and factories

### Packaging
- Package Windows installer (MSIX or NSIS)
- Package macOS app bundle and notarize
- Automate packaging with scripts

### Documentation
- Write user guide and onboarding docs
- Write developer setup and contribution guide
- Document security model and backup procedures

