    -- ============================================================================
-- Firewood Bank Management System - Sample Data (Schema-Compatible)
-- Minimal, focused on split/unsplit wood and orders
-- ============================================================================

PRAGMA foreign_keys = OFF;

-- Clear existing data (keep default users created by migrations)
DELETE FROM delivery_log;
DELETE FROM orders;
DELETE FROM inventory_items;
DELETE FROM households;
DELETE FROM agencies;

-- Reset auto-increment so IDs are predictable
DELETE FROM sqlite_sequence WHERE name IN ('orders','households','inventory_items','delivery_log','agencies');

-- Agencies
INSERT INTO agencies (name, type, contact_name, phone, email, address, notes, active, created_at) VALUES
 ('Mendocino County Social Services','Government','Linda Martinez','(707) 234-5678','lmartinez@countyofmendocino.org','Ukiah, CA','Primary referral agency',1,datetime('now')),
 ('Senior Center of Laytonville','Non-profit','Betty White','(707) 984-8056','info@laytonvilleseniors.org','Laytonville, CA','Elderly assistance program',1,datetime('now'));

-- Households (minimal fields compatible with current schema)
INSERT INTO households (name, address, phone, email, created_at) VALUES
 ('Anderson Family','12345 Highway 101 N, Laytonville, CA 95454','(707) 555-1001','anderson@example.com',datetime('now', '-180 days')),
 ('Martinez Household','456 Branscomb Rd, Laytonville, CA 95454','(707) 555-1002','martinez.family@example.com',datetime('now', '-240 days')),
 ('Thompson Family','789 Bell Springs Rd, Laytonville, CA 95454','(707) 555-1003',NULL,datetime('now', '-150 days')),
 ('Chen-Rodriguez Family','321 Spyrock Rd, Laytonville, CA 95454','(707) 555-1004','chen.rodriguez@example.com',datetime('now', '-365 days')),
 ('Johnson-Lee Family','369 Bell Springs Rd, Laytonville, CA 95454','(707) 555-1009','jlee@example.com',datetime('now', '-300 days'));

-- Ensure a 'Wood' category exists
INSERT INTO inventory_categories (name, description, created_at)
SELECT 'Wood','Firewood in cords',datetime('now')
WHERE NOT EXISTS (SELECT 1 FROM inventory_categories WHERE name='Wood');

-- Split and Unsplit wood inventory (only what we need)
INSERT INTO inventory_items (category_id, item_name, quantity, unit, location, last_updated, created_at)
VALUES
 ((SELECT id FROM inventory_categories WHERE name='Wood'),'Split Firewood', 12.0,'cords','Main Yard',datetime('now'),datetime('now', '-30 days')),
 ((SELECT id FROM inventory_categories WHERE name='Wood'),'Rounds (Unsplit)', 8.0,'cords','Main Yard',datetime('now'),datetime('now', '-45 days'));

-- Orders (mix of completed and open)
INSERT INTO orders (
  household_id, order_date, requested_cords, delivered_cords, status, priority,
    delivery_date, delivery_address, delivery_notes, assigned_driver, payment_method,
  amount_paid, notes, created_by, created_at, updated_at, delivery_time, start_mileage, end_mileage, completed_date
) VALUES
  -- Completed (0.33 cords each)
  (1, date('now','-15 days'), 1.0, 0.33, 'Completed', 'Normal', date('now','-15 days'), 'Anderson address','', 'John Smith','Voucher',0.00,'', 'lead', datetime('now','-20 days'), datetime('now','-15 days'),'09:00',45230,45255,date('now','-15 days')),
  (2, date('now','-12 days'), 1.0, 0.33, 'Completed', 'Normal', date('now','-12 days'), 'Martinez address','', 'Maria Garcia','Work-for-Wood',0.00,'', 'lead', datetime('now','-18 days'), datetime('now','-12 days'),'10:30',45255,45273,date('now','-12 days')),
  (4, date('now','-10 days'), 1.0, 0.33, 'Completed', 'High',   date('now','-10 days'), 'Chen address','',     'Bob Wilson','Cash',75.00,'', 'bwilson', datetime('now','-15 days'), datetime('now','-10 days'),'08:30',45273,45298,date('now','-10 days')),
  (5, date('now','-5 days'),  1.0, 0.33, 'Completed', 'Normal', date('now','-5 days'),  'Johnson address','',  'Bob Wilson','Cash',75.00,'', 'bwilson', datetime('now','-8 days'),  datetime('now','-5 days'), '09:45',45338,45362,date('now','-5 days')),
  -- Open
  (3, date('now','-2 days'),  1.0, 0.0,  'In Progress','Normal', date('now'),          'Thompson address','', 'Bob Wilson','Cash',0.00,'En route', 'bwilson', datetime('now','-4 days'), datetime('now','-1 days'), '08:00',45388,0,NULL),
  (2, date('now','-1 days'),  1.0, 0.0,  'Scheduled',  'High',   date('now','+1 days'),'Martinez address','', 'Maria Garcia','Voucher',0.00,'Priority', 'lead', datetime('now','-1 days'), datetime('now','-1 days'), NULL,0,0,NULL),
  (4, date('now'),            0.5, 0.0,  'Pending',    'Low',    NULL,                 NULL,'',               NULL,NULL,0.00,'Small order', 'user', datetime('now'), datetime('now'), NULL,0,0,NULL);

-- Delivery log for completed orders
INSERT INTO delivery_log (order_id, driver, delivery_date, delivery_time, start_mileage, end_mileage, delivered_cords, client_name, client_address, logged_at) VALUES
 (1,'John Smith',date('now','-15 days'),'09:00',45230,45255,0.33,'Anderson Family','Anderson address',datetime('now','-15 days')),
 (2,'Maria Garcia',date('now','-12 days'),'10:30',45255,45273,0.33,'Martinez Household','Martinez address',datetime('now','-12 days')),
 (3,'Bob Wilson',date('now','-10 days'),'08:30',45273,45298,0.33,'Chen-Rodriguez Family','Chen address',datetime('now','-10 days')),
 (4,'Bob Wilson',date('now','-5 days'),'09:45',45338,45362,0.33,'Johnson-Lee Family','Johnson address',datetime('now','-5 days'));

PRAGMA foreign_keys = ON;


