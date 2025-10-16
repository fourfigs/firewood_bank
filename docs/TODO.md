# Firewood Bank  TODO

1. Confirm stack and architecture (Qt 6 Widgets, C++17, SQLite, CMake); define modules and naming conventions; choose crypto (Argon2, libsodium) and outline data retention/privacy policies.
2. Initialize repository structure and CMake project; create folders `app`, `ui`, `core`, `db`, `tests` and a minimal runnable shell (main window, navigation, theme).
3. Configure configuration management and logging (rotating logs, log levels, paths) and environment handling for dev/prod builds.
4. Implement SQLite schema versioning and a migration runner executed at app startup.
5. Design database schema: clients, volunteers, hours, work_for_wood (credits/ledger), inventory (batches, inspections, movements), orders (reservations, shipments), assets, maintenance, audit, reports; add indices, FKs, constraints.
6. Seed development data and fixtures to support local testing and demos.
7. Implement authentication: password hashing with Argon2, role-based authorization (admin, dispatcher, yard, driver, volunteer), session management.
8. Implement encryption at rest for sensitive fields using libsodium; integrate secure key storage strategy and automated field-level encryption/decryption.
9. Implement audit logging for key entity changes (who, when, what) and surface basic audit views.
10. Build Clients module: CRUD UI, validation, search/filters/pagination; encrypt gate codes and sensitive notes; manage referral sources.
11. Build Volunteers & Hours: volunteer CRUD, activity/hour logging, licenses/equipment/trainings/waivers, emergency contacts.
12. Build Work-for-Wood credits: define hourly rate policy, implement credit ledger, redemptions, and linkage to orders/inventory issuance.
13. Build Inventory: items/categories/units, batch tracking, inspections (moisture), movements (intake/adjust/reserve/pick/scrap), min/target levels and alerts.
14. Build Orders & Deliveries: order creation (requested cords, priority), reservations against ready inventory, scheduling/assignment, delivery tracking, proof of delivery, partials.
15. Build Assets & Maintenance: assets for chainsaws/splitters/PPE, maintenance schedules/reminders, maintenance logs and costs.
16. Reporting & Exports: wood delivered by period, households served, volunteer hours, expenses/inventory spend; CSV export for reports.
17. Backup and Import: encrypted backups (manual and scheduled), CSV import templates, validation and import preview flows.
18. Sync scaffold: design sync-friendly IDs and change tracking, local changelog, conflict strategy, and an abstraction layer for future transport.
19. Testing & QA: unit tests for core/db, UI tests for key flows, fixtures/factories; establish baseline coverage and CI jobs.
20. Packaging: Windows installer (MSIX/NSIS) and macOS app bundle (notarized); automate packaging scripts.
21. Documentation: user guide and onboarding, developer setup and contribution guide, security model and backup procedures.
22. Final QA, performance tuning, and first stable release.
