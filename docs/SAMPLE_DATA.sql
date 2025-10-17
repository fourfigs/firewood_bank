    -- ============================================================================
-- Firewood Bank Management System - Sample Data
-- 
-- Created by Kenneth Hanks for the Northern Mendocino Ecosystem Recovery Alliance
-- 
-- This script populates the database with realistic sample data for testing
-- and demonstration purposes.
-- ============================================================================

-- Clear existing data (except default users)
DELETE FROM delivery_log;
DELETE FROM profile_change_requests;
DELETE FROM volunteer_certifications;
DELETE FROM work_schedule_signups;
DELETE FROM work_schedule;
DELETE FROM orders;
DELETE FROM inventory_items;
DELETE FROM inventory_categories;
DELETE FROM equipment_maintenance;
DELETE FROM households WHERE id > 0;
DELETE FROM agencies WHERE id > 0;
DELETE FROM users WHERE username NOT IN ('admin', 'lead', 'user', 'volunteer');

-- ============================================================================
-- ORGANIZATION SETTINGS
-- ============================================================================
INSERT OR REPLACE INTO organization_settings (id, org_name, email, phone, created_at, updated_at)
VALUES (
    1,
    'Northern Mendocino Ecosystem Recovery Alliance',
    'info@nm-era.org',
    '(707) 555-WOOD',
    datetime('now'),
    datetime('now')
);

-- ============================================================================
-- ADDITIONAL USERS (beyond defaults: admin, lead, user, volunteer)
-- ============================================================================
-- Password for all users: 'password123' (hashed with SHA-256)
-- Hash: ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f

INSERT INTO users (username, password_hash, role, full_name, email, phone, address, availability, created_at)
VALUES
    ('jsmith', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Employee', 'John Smith', 'jsmith@nm-era.org', '(707) 555-0101', '123 Pine St, Laytonville, CA 95454', 'Mon-Fri 9am-5pm', datetime('now')),
    ('mgarcia', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Employee', 'Maria Garcia', 'mgarcia@nm-era.org', '(707) 555-0102', '456 Oak Ave, Laytonville, CA 95454', 'Tue-Sat 10am-6pm', datetime('now')),
    ('bwilson', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Lead', 'Bob Wilson', 'bwilson@nm-era.org', '(707) 555-0103', '789 Cedar Rd, Laytonville, CA 95454', 'Mon-Fri 8am-4pm', datetime('now')),
    ('sjohnson', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Volunteer', 'Sarah Johnson', 'sjohnson@example.com', '(707) 555-0104', '321 Maple Dr, Laytonville, CA 95454', 'Weekends only', datetime('now')),
    ('tmiller', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Volunteer', 'Tom Miller', 'tmiller@example.com', '(707) 555-0105', '654 Birch Ln, Laytonville, CA 95454', 'Flexible schedule', datetime('now')),
    ('kdavis', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Volunteer', 'Karen Davis', 'kdavis@example.com', '(707) 555-0106', '987 Spruce Way, Laytonville, CA 95454', 'Mon, Wed, Fri mornings', datetime('now'));

-- ============================================================================
-- AGENCIES (Referral Sources)
-- ============================================================================
INSERT INTO agencies (name, contact_person, phone, email, address, notes, created_at)
VALUES
    ('Mendocino County Social Services', 'Linda Martinez', '(707) 234-5678', 'lmartinez@countyofmendocino.org', '747 S State St, Ukiah, CA 95482', 'Primary referral agency', datetime('now')),
    ('Redwood Community Services', 'James Chen', '(707) 456-7890', 'jchen@redwoodservices.org', '123 Main St, Willits, CA 95490', 'Assists low-income families', datetime('now')),
    ('Long Valley Health Center', 'Dr. Susan Brown', '(707) 984-6131', 'sbrown@longvalleyhealth.org', '76177 Covelo Rd, Covelo, CA 95428', 'Medical referrals for vulnerable clients', datetime('now')),
    ('Round Valley Indian Tribes', 'Michael Bear', '(707) 983-6126', 'mbear@rvit.org', '77826 Covelo Rd, Covelo, CA 95428', 'Tribal member assistance', datetime('now')),
    ('Senior Center of Laytonville', 'Betty White', '(707) 984-8056', 'bwhite@laytonvilleseniors.org', 'Main St, Laytonville, CA 95454', 'Elderly assistance program', datetime('now'));

-- ============================================================================
-- HOUSEHOLDS (Clients)
-- ============================================================================
INSERT INTO households (
    name, address, city, state, zip, phone, email, mailing_address,
    gate_code, delivery_directions, stove_fireplace_size,
    is_volunteer, waiver_signed, has_license, has_working_vehicle,
    participating_in_work_for_wood, stock_on_hand, order_count, last_order_date,
    referral_source, notes, created_at
)
VALUES
    -- Active clients with recent orders
    ('Anderson Family', '12345 Highway 101 N', 'Laytonville', 'CA', '95454', '(707) 555-1001', 'anderson@example.com', NULL,
     '1234', 'White gate on left, driveway is 0.5 miles up dirt road', 'Large (3+ cu ft)',
     0, 1, 0, 0, 0, 1.5, 5, date('now', '-15 days'),
     'Mendocino County Social Services', 'Elderly couple, limited mobility', datetime('now', '-180 days')),
    
    ('Martinez Household', '456 Branscomb Rd', 'Laytonville', 'CA', '95454', '(707) 555-1002', 'martinez.family@example.com', 'PO Box 123, Laytonville, CA 95454',
     NULL, 'Blue mailbox, follow signs to main house', 'Medium (2-3 cu ft)',
     0, 1, 1, 1, 1, 0.0, 8, date('now', '-7 days'),
     'Redwood Community Services', 'Participating in work-for-wood program', datetime('now', '-240 days')),
    
    ('Thompson Family', '789 Bell Springs Rd', 'Laytonville', 'CA', '95454', '(707) 555-1003', NULL, NULL,
     '5678', 'Red barn visible from road, house is behind', 'Small (1-2 cu ft)',
     0, 1, 0, 0, 0, 0.33, 3, date('now', '-30 days'),
     'Senior Center of Laytonville', 'Single parent household with 3 children', datetime('now', '-150 days')),
    
    ('Chen-Rodriguez Family', '321 Spyrock Rd', 'Laytonville', 'CA', '95454', '(707) 555-1004', 'chen.rodriguez@example.com', NULL,
     NULL, 'Look for solar panels, steep driveway in winter', 'Large (3+ cu ft)',
     0, 1, 1, 1, 0, 2.0, 12, date('now', '-3 days'),
     'Self-referred', 'Regular customer, very reliable', datetime('now', '-365 days')),
    
    ('Williams Household', '654 Dos Rios Rd', 'Covelo', 'CA', '95428', '(707) 555-1005', 'williams@example.com', NULL,
     '9999', 'Cross small bridge, stay right at fork', 'Medium (2-3 cu ft)',
     1, 1, 1, 1, 0, 1.0, 6, date('now', '-45 days'),
     'Round Valley Indian Tribes', 'Also volunteers on weekends', datetime('now', '-200 days')),
    
    -- Clients needing deliveries
    ('Brown Family', '987 Highway 162', 'Covelo', 'CA', '95428', '(707) 555-1006', NULL, 'PO Box 456, Covelo, CA 95428',
     NULL, 'Green metal gate, call when arriving', 'Large (3+ cu ft)',
     0, 1, 0, 0, 0, 0.0, 2, date('now', '-90 days'),
     'Long Valley Health Center', 'Medical referral, priority client', datetime('now', '-120 days')),
    
    ('Davis Household', '147 Oak Flat Rd', 'Laytonville', 'CA', '95454', '(707) 555-1007', 'davis.home@example.com', NULL,
     '4321', 'Yellow house, park by woodshed', 'Medium (2-3 cu ft)',
     0, 1, 1, 1, 1, 0.5, 4, date('now', '-60 days'),
     'Redwood Community Services', 'Work-for-wood participant, 12 hours credit', datetime('now', '-180 days')),
    
    ('Garcia Extended Family', '258 Wilderness Lodge Rd', 'Laytonville', 'CA', '95454', '(707) 555-1008', 'garcia.ext@example.com', NULL,
     NULL, 'Last house on road, difficult in snow', 'Large (3+ cu ft)',
     0, 1, 0, 0, 0, 0.0, 1, date('now', '-120 days'),
     'Mendocino County Social Services', 'New client, first delivery', datetime('now', '-30 days')),
    
    -- Volunteers who also receive wood
    ('Johnson-Lee Family', '369 Bell Springs Rd', 'Laytonville', 'CA', '95454', '(707) 555-1009', 'jlee@example.com', NULL,
     '7777', 'Metal archway entrance, follow gravel drive', 'Medium (2-3 cu ft)',
     1, 1, 1, 1, 0, 1.5, 7, date('now', '-20 days'),
     'Self-referred', 'Volunteer driver, helps with deliveries', datetime('now', '-300 days')),
    
    ('Miller-White Household', '741 Branscomb Rd', 'Laytonville', 'CA', '95454', '(707) 555-1010', 'mwhite@example.com', NULL,
     NULL, 'Blue house with red roof, easy access', 'Small (1-2 cu ft)',
     1, 1, 1, 1, 0, 0.67, 5, date('now', '-10 days'),
     'Self-referred', 'Volunteer for wood splitting', datetime('now', '-250 days'));

-- ============================================================================
-- INVENTORY CATEGORIES
-- ============================================================================
INSERT INTO inventory_categories (name, description, created_at)
VALUES
    ('Wood', 'Firewood in various forms', datetime('now')),
    ('Safety Equipment', 'Personal protective equipment', datetime('now')),
    ('Chainsaw Supplies', 'Chainsaw maintenance and fuel', datetime('now')),
    ('Tools', 'General tools and equipment', datetime('now')),
    ('Vehicle Supplies', 'Delivery vehicle maintenance', datetime('now'));

-- ============================================================================
-- INVENTORY ITEMS
-- ============================================================================
INSERT INTO inventory_items (category_id, name, quantity, unit, location, notes, last_updated, created_at)
VALUES
    -- Wood inventory
    (1, 'Split Firewood', 45.5, 'cords', 'Main Yard - Covered Area', 'Seasoned oak and fir, ready for delivery', datetime('now'), datetime('now', '-30 days')),
    (1, 'Rounds (Unsplit)', 12.0, 'cords', 'Main Yard - Open Area', 'Needs splitting, various hardwoods', datetime('now'), datetime('now', '-60 days')),
    (1, 'Kindling', 150, 'bundles', 'Covered Shed B', 'Small split pieces for fire starting', datetime('now'), datetime('now', '-15 days')),
    (1, 'Green Wood (Unseasoned)', 8.5, 'cords', 'Back Lot', 'Needs 6+ months seasoning', datetime('now'), datetime('now', '-90 days')),
    
    -- Safety Equipment
    (2, 'Safety Glasses', 45, 'pairs', 'Storage Room A', 'ANSI Z87.1 certified', datetime('now'), datetime('now', '-120 days')),
    (2, 'Work Gloves', 60, 'pairs', 'Storage Room A', 'Leather palm, various sizes', datetime('now'), datetime('now', '-120 days')),
    (2, 'Ear Protection (Muffs)', 25, 'units', 'Storage Room A', 'NRR 30dB rating', datetime('now'), datetime('now', '-120 days')),
    (2, 'Hard Hats', 18, 'units', 'Storage Room A', 'ANSI Z89.1 Type I', datetime('now'), datetime('now', '-120 days')),
    (2, 'Chainsaw Chaps', 12, 'pairs', 'Storage Room A', 'Various sizes, inspect before use', datetime('now'), datetime('now', '-120 days')),
    (2, 'First Aid Kits', 5, 'kits', 'Storage Room A, Office', 'Check expiration dates monthly', datetime('now'), datetime('now', '-30 days')),
    
    -- Chainsaw Supplies
    (3, 'Bar Oil', 45, 'gallons', 'Fuel Shed', 'For chainsaw bar lubrication', datetime('now'), datetime('now', '-45 days')),
    (3, 'Mixed Gas (2-cycle)', 25, 'gallons', 'Fuel Shed', '50:1 mix, use within 30 days', datetime('now'), datetime('now', '-10 days')),
    (3, 'Unmixed Gas', 15, 'gallons', 'Fuel Shed', 'Ethanol-free recommended', datetime('now'), datetime('now', '-10 days')),
    (3, 'Chainsaw Chains (18")', 8, 'units', 'Tool Shed', 'Sharpened and ready', datetime('now'), datetime('now', '-60 days')),
    (3, 'Chainsaw Chains (20")', 6, 'units', 'Tool Shed', 'Sharpened and ready', datetime('now'), datetime('now', '-60 days')),
    (3, 'Chainsaw Files', 12, 'units', 'Tool Shed', 'Various sizes for sharpening', datetime('now'), datetime('now', '-90 days')),
    (3, 'Spark Plugs', 20, 'units', 'Tool Shed', 'Assorted sizes', datetime('now'), datetime('now', '-90 days')),
    
    -- Tools
    (4, 'Splitting Mauls', 8, 'units', 'Tool Shed', '6-8 lb heads, check handles', datetime('now'), datetime('now', '-120 days')),
    (4, 'Wedges (Steel)', 15, 'units', 'Tool Shed', 'For splitting large rounds', datetime('now'), datetime('now', '-120 days')),
    (4, 'Cant Hooks', 4, 'units', 'Tool Shed', 'For log rolling', datetime('now'), datetime('now', '-120 days')),
    (4, 'Measuring Tapes (25ft)', 6, 'units', 'Tool Shed, Office', 'For cord measurement', datetime('now'), datetime('now', '-90 days')),
    (4, 'Wheelbarrows', 5, 'units', 'Main Yard', 'Check tires weekly', datetime('now'), datetime('now', '-180 days')),
    
    -- Vehicle Supplies
    (5, 'Engine Oil (5W-30)', 12, 'quarts', 'Garage', 'For delivery trucks', datetime('now'), datetime('now', '-30 days')),
    (5, 'Diesel Fuel', 80, 'gallons', 'Fuel Tank', 'Main delivery truck fuel', datetime('now'), datetime('now', '-5 days')),
    (5, 'Tie-Down Straps', 20, 'units', 'Delivery Trucks', 'Check for wear before each use', datetime('now'), datetime('now', '-60 days')),
    (5, 'Tarps', 10, 'units', 'Garage', 'For covering loads', datetime('now'), datetime('now', '-90 days'));

-- ============================================================================
-- EQUIPMENT MAINTENANCE
-- ============================================================================
INSERT INTO equipment_maintenance (
    equipment_name, service_hours_current, next_service_hours,
    alert_threshold, last_service_date, notes, created_at
)
VALUES
    ('Log Splitter - Primary', 142, 250, 25, date('now', '-45 days'), 'Hydraulic log splitter, 27-ton capacity. Oil changed at 95 hours.', datetime('now', '-365 days')),
    ('Log Splitter - Backup', 78, 250, 25, date('now', '-120 days'), 'Older 22-ton model, use when primary is down. Needs greasing.', datetime('now', '-365 days')),
    ('Chainsaw - Stihl MS 461', 38, 100, 10, date('now', '-30 days'), 'Professional grade, sharp chain. Check air filter soon.', datetime('now', '-200 days')),
    ('Chainsaw - Husqvarna 572XP', 52, 100, 10, date('now', '-20 days'), 'Powerful saw for large logs. Recent carb cleaning.', datetime('now', '-180 days')),
    ('Chainsaw - Stihl MS 271', 91, 100, 10, date('now', '-60 days'), 'General purpose saw. Due for service soon!', datetime('now', '-300 days')),
    ('Delivery Truck - Ford F-350', 3250, 5000, 250, date('now', '-15 days'), 'Main delivery vehicle. Oil change and tire rotation completed. Next service includes transmission check.', datetime('now', '-1825 days')),
    ('Utility Truck - Chevy 2500', 1890, 5000, 250, date('now', '-90 days'), 'Backup delivery truck. Needs brake inspection soon.', datetime('now', '-1460 days')),
    ('Tractor - John Deere 3025E', 412, 500, 50, date('now', '-60 days'), 'Used for moving wood piles and loading. Hydraulic fluid changed.', datetime('now', '-730 days'));

-- ============================================================================
-- WORK SCHEDULE (Work Days for Volunteers)
-- ============================================================================
INSERT INTO work_schedule (
    date, start_time, end_time, activity_type, location, max_volunteers,
    filled_slots, notes, created_by, created_at
)
VALUES
    -- Upcoming work days
    (date('now', '+2 days'), '08:00', '12:00', 'Wood Splitting', 'Main Yard', 6, 3, 'Bring work gloves and water. Lunch provided.', 'admin', datetime('now', '-7 days')),
    (date('now', '+5 days'), '09:00', '15:00', 'Loading & Delivery', 'Main Yard', 4, 2, 'Help load trucks and ride along for deliveries.', 'bwilson', datetime('now', '-5 days')),
    (date('now', '+9 days'), '08:00', '14:00', 'Wood Processing', 'Main Yard', 8, 5, 'Splitting and stacking day. Heavy work!', 'admin', datetime('now', '-3 days')),
    (date('now', '+12 days'), '10:00', '13:00', 'Inventory & Maintenance', 'Tool Shed', 3, 1, 'Organize tools, sharpen chains, general maintenance.', 'bwilson', datetime('now', '-2 days')),
    (date('now', '+16 days'), '08:00', '12:00', 'Wood Splitting', 'Main Yard', 6, 2, 'Regular splitting day. All skill levels welcome.', 'admin', datetime('now', '-1 days')),
    
    -- Past work days
    (date('now', '-5 days'), '08:00', '12:00', 'Wood Splitting', 'Main Yard', 6, 6, 'Completed - Great turnout!', 'admin', datetime('now', '-12 days')),
    (date('now', '-12 days'), '09:00', '15:00', 'Delivery Day', 'Various Locations', 5, 5, 'Completed - 8 deliveries made', 'bwilson', datetime('now', '-19 days')),
    (date('now', '-19 days'), '08:00', '14:00', 'Wood Processing', 'Main Yard', 8, 7, 'Completed - Processed 3 cords', 'admin', datetime('now', '-26 days'));

-- ============================================================================
-- WORK SCHEDULE SIGNUPS
-- ============================================================================
INSERT INTO work_schedule_signups (work_schedule_id, username, signed_up_at)
VALUES
    -- Upcoming work day signups
    (1, 'sjohnson', datetime('now', '-6 days')),
    (1, 'tmiller', datetime('now', '-5 days')),
    (1, 'volunteer', datetime('now', '-4 days')),
    
    (2, 'kdavis', datetime('now', '-4 days')),
    (2, 'tmiller', datetime('now', '-3 days')),
    
    (3, 'sjohnson', datetime('now', '-2 days')),
    (3, 'volunteer', datetime('now', '-2 days')),
    (3, 'kdavis', datetime('now', '-1 days')),
    (3, 'tmiller', datetime('now', '-1 days')),
    (3, 'jsmith', datetime('now', '-1 days')),
    
    (4, 'tmiller', datetime('now', '-1 days')),
    
    (5, 'sjohnson', datetime('now', '-12 hours')),
    (5, 'volunteer', datetime('now', '-6 hours'));

-- ============================================================================
-- VOLUNTEER CERTIFICATIONS
-- ============================================================================
INSERT INTO volunteer_certifications (
    username, certification_name, issue_date, expiration_date, notes, created_at
)
VALUES
    ('sjohnson', 'First Aid & CPR', date('now', '-180 days'), date('now', '+185 days'), 'Red Cross certified', datetime('now', '-180 days')),
    ('sjohnson', 'Chainsaw Safety Training', date('now', '-90 days'), date('now', '+275 days'), 'Completed OSHA training', datetime('now', '-90 days')),
    
    ('tmiller', 'Chainsaw Safety Training', date('now', '-120 days'), date('now', '+245 days'), 'Basic certification', datetime('now', '-120 days')),
    ('tmiller', 'Forklift Operation', date('now', '-200 days'), date('now', '+165 days'), 'Valid for tractor operation', datetime('now', '-200 days')),
    ('tmiller', 'First Aid & CPR', date('now', '-60 days'), date('now', '+305 days'), 'Red Cross certified', datetime('now', '-60 days')),
    
    ('volunteer', 'First Aid & CPR', date('now', '-150 days'), date('now', '+215 days'), 'American Heart Association', datetime('now', '-150 days')),
    
    ('kdavis', 'Chainsaw Safety Training', date('now', '-30 days'), date('now', '+335 days'), 'Advanced certification', datetime('now', '-30 days'));

-- ============================================================================
-- ORDERS (Work Orders)
-- ============================================================================
INSERT INTO orders (
    household_id, date_received, requested_cords, delivered_cords, status, priority,
    delivery_date, delivery_address, delivery_notes, assigned_driver, payment_method,
    amount_paid, delivery_time, start_mileage, end_mileage, completed_date,
    notes, created_by, created_at, updated_at
)
VALUES
    -- Completed orders (for delivery log)
    (1, date('now', '-15 days'), 1.0, 0.33, 'Completed', 'Normal',
     date('now', '-15 days'), '12345 Highway 101 N, Laytonville, CA 95454', 'White gate on left, driveway is 0.5 miles up dirt road',
     'John Smith', 'Voucher', 0.00, '09:00 AM', 45230.5, 45255.2, date('now', '-15 days'),
     'Delivery completed successfully', 'lead', datetime('now', '-20 days'), datetime('now', '-15 days')),
    
    (2, date('now', '-12 days'), 1.0, 0.33, 'Completed', 'Normal',
     date('now', '-12 days'), '456 Branscomb Rd, Laytonville, CA 95454', 'Blue mailbox, follow signs to main house',
     'Maria Garcia', 'Work-for-Wood', 0.00, '10:30 AM', 45255.2, 45272.8, date('now', '-12 days'),
     'Client helped unload, credited 2 hours', 'lead', datetime('now', '-18 days'), datetime('now', '-12 days')),
    
    (4, date('now', '-10 days'), 1.0, 0.33, 'Completed', 'High',
     date('now', '-10 days'), '321 Spyrock Rd, Laytonville, CA 95454', 'Look for solar panels, steep driveway',
     'Bob Wilson', 'Cash', 75.00, '08:30 AM', 45272.8, 45298.5, date('now', '-10 days'),
     'Regular customer, paid in full', 'bwilson', datetime('now', '-15 days'), datetime('now', '-10 days')),
    
    (10, date('now', '-8 days'), 1.0, 0.33, 'Completed', 'Normal',
     date('now', '-8 days'), '741 Branscomb Rd, Laytonville, CA 95454', 'Blue house with red roof, easy access',
     'John Smith', 'Free', 0.00, '02:00 PM', 45298.5, 45315.0, date('now', '-8 days'),
     'Volunteer benefit delivery', 'jsmith', datetime('now', '-12 days'), datetime('now', '-8 days')),
    
    (3, date('now', '-7 days'), 1.0, 0.33, 'Completed', 'Normal',
     date('now', '-7 days'), '789 Bell Springs Rd, Laytonville, CA 95454', 'Red barn visible from road',
     'Maria Garcia', 'Check', 70.00, '11:15 AM', 45315.0, 45338.7, date('now', '-7 days'),
     'Check #1234, deposited', 'mgarcia', datetime('now', '-10 days'), datetime('now', '-7 days')),
    
    (9, date('now', '-5 days'), 1.0, 0.33, 'Completed', 'Normal',
     date('now', '-5 days'), '369 Bell Springs Rd, Laytonville, CA 95454', 'Metal archway entrance, follow gravel drive',
     'Bob Wilson', 'Cash', 75.00, '09:45 AM', 45338.7, 45362.3, date('now', '-5 days'),
     'Volunteer driver, paid for own wood', 'bwilson', datetime('now', '-8 days'), datetime('now', '-5 days')),
    
    (4, date('now', '-3 days'), 1.0, 0.33, 'Completed', 'Normal',
     date('now', '-3 days'), '321 Spyrock Rd, Laytonville, CA 95454', 'Look for solar panels',
     'John Smith', 'Credit Card', 80.00, '10:00 AM', 45362.3, 45388.1, date('now', '-3 days'),
     'Card processed successfully', 'jsmith', datetime('now', '-5 days'), datetime('now', '-3 days')),
    
    -- In Progress orders
    (5, date('now', '-2 days'), 1.0, 0.0, 'In Progress', 'Normal',
     date('now'), '654 Dos Rios Rd, Covelo, CA 95428', 'Cross small bridge, stay right at fork',
     'Bob Wilson', 'Cash', 0.00, '08:00 AM', 45388.1, 0, NULL,
     'Currently en route', 'bwilson', datetime('now', '-4 days'), datetime('now', '-1 hours')),
    
    -- Scheduled orders
    (6, date('now', '-3 days'), 1.0, 0.0, 'Scheduled', 'High',
     date('now', '+1 days'), '987 Highway 162, Covelo, CA 95428', 'Green metal gate, call when arriving',
     'Maria Garcia', 'Voucher', 0.00, NULL, 0, 0, NULL,
     'Priority medical referral', 'lead', datetime('now', '-3 days'), datetime('now', '-2 days')),
    
    (7, date('now', '-2 days'), 1.0, 0.0, 'Scheduled', 'Normal',
     date('now', '+2 days'), '147 Oak Flat Rd, Laytonville, CA 95454', 'Yellow house, park by woodshed',
     'John Smith', 'Work-for-Wood', 0.00, NULL, 0, 0, NULL,
     'Has 12 hours work credit', 'jsmith', datetime('now', '-2 days'), datetime('now', '-1 days')),
    
    (8, date('now', '-1 days'), 1.5, 0.0, 'Scheduled', 'Normal',
     date('now', '+3 days'), '258 Wilderness Lodge Rd, Laytonville, CA 95454', 'Last house on road, difficult in snow',
     'Bob Wilson', 'Voucher', 0.00, NULL, 0, 0, NULL,
     'New client, first delivery - extra wood approved', 'lead', datetime('now', '-1 days'), datetime('now', '-6 hours')),
    
    -- Pending orders
    (1, date('now'), 1.0, 0.0, 'Pending', 'Normal',
     NULL, NULL, NULL, NULL, NULL, 0.00, NULL, 0, 0, NULL,
     'Request received, needs scheduling', 'user', datetime('now', '-3 hours'), datetime('now', '-3 hours')),
    
    (2, date('now'), 1.0, 0.0, 'Pending', 'Normal',
     NULL, NULL, NULL, NULL, NULL, 0.00, NULL, 0, 0, NULL,
     'Work-for-wood program, has credit', 'user', datetime('now', '-2 hours'), datetime('now', '-2 hours')),
    
    (10, date('now'), 0.5, 0.0, 'Pending', 'Low',
     NULL, NULL, NULL, NULL, NULL, 0.00, NULL, 0, 0, NULL,
     'Small supplemental order', 'user', datetime('now', '-1 hours'), datetime('now', '-1 hours'));

-- ============================================================================
-- DELIVERY LOG (Auto-generated from completed orders above)
-- ============================================================================
INSERT INTO delivery_log (
    order_id, driver, delivery_date, delivery_time, start_mileage, end_mileage,
    delivered_cords, client_name, client_address, logged_at
)
VALUES
    (1, 'John Smith', date('now', '-15 days'), '09:00 AM', 45230.5, 45255.2, 0.33,
     'Anderson Family', '12345 Highway 101 N, Laytonville, CA 95454', datetime('now', '-15 days')),
    
    (2, 'Maria Garcia', date('now', '-12 days'), '10:30 AM', 45255.2, 45272.8, 0.33,
     'Martinez Household', '456 Branscomb Rd, Laytonville, CA 95454', datetime('now', '-12 days')),
    
    (3, 'Bob Wilson', date('now', '-10 days'), '08:30 AM', 45272.8, 45298.5, 0.33,
     'Chen-Rodriguez Family', '321 Spyrock Rd, Laytonville, CA 95454', datetime('now', '-10 days')),
    
    (4, 'John Smith', date('now', '-8 days'), '02:00 PM', 45298.5, 45315.0, 0.33,
     'Miller-White Household', '741 Branscomb Rd, Laytonville, CA 95454', datetime('now', '-8 days')),
    
    (5, 'Maria Garcia', date('now', '-7 days'), '11:15 AM', 45315.0, 45338.7, 0.33,
     'Thompson Family', '789 Bell Springs Rd, Laytonville, CA 95454', datetime('now', '-7 days')),
    
    (6, 'Bob Wilson', date('now', '-5 days'), '09:45 AM', 45338.7, 45362.3, 0.33,
     'Johnson-Lee Family', '369 Bell Springs Rd, Laytonville, CA 95454', datetime('now', '-5 days')),
    
    (7, 'John Smith', date('now', '-3 days'), '10:00 AM', 45362.3, 45388.1, 0.33,
     'Chen-Rodriguez Family', '321 Spyrock Rd, Laytonville, CA 95454', datetime('now', '-3 days'));

-- ============================================================================
-- PROFILE CHANGE REQUESTS
-- ============================================================================
INSERT INTO profile_change_requests (
    username, field_name, current_value, requested_value, reason, status,
    requested_at, reviewed_at, reviewed_by
)
VALUES
    -- Pending requests
    ('sjohnson', 'phone', '(707) 555-0104', '(707) 555-9999', 'Got new phone number', 'Pending',
     datetime('now', '-2 days'), NULL, NULL),
    
    ('tmiller', 'availability', 'Flexible schedule', 'Mon-Thu 9am-3pm', 'New job, more structured schedule', 'Pending',
     datetime('now', '-1 days'), NULL, NULL),
    
    -- Approved requests
    ('kdavis', 'email', 'oldkdavis@example.com', 'kdavis@example.com', 'Updated email address', 'Approved',
     datetime('now', '-10 days'), datetime('now', '-8 days'), 'admin'),
    
    ('volunteer', 'address', '123 Old St, Laytonville, CA', '456 New Ave, Laytonville, CA', 'Moved to new house', 'Approved',
     datetime('now', '-15 days'), datetime('now', '-14 days'), 'admin'),
    
    -- Rejected request
    ('jsmith', 'role', 'Employee', 'Lead', 'Request promotion', 'Rejected',
     datetime('now', '-20 days'), datetime('now', '-18 days'), 'admin');

-- ============================================================================
-- SUMMARY STATISTICS
-- ============================================================================
SELECT '============================================================================';
SELECT 'DATABASE POPULATED WITH SAMPLE DATA';
SELECT '============================================================================';
SELECT '';
SELECT 'SUMMARY:';
SELECT '- Users: ' || COUNT(*) || ' total' FROM users;
SELECT '- Households: ' || COUNT(*) || ' clients' FROM households;
SELECT '- Agencies: ' || COUNT(*) || ' referral agencies' FROM agencies;
SELECT '- Orders: ' || COUNT(*) || ' total orders' FROM orders;
SELECT '  * Completed: ' || COUNT(*) FROM orders WHERE status = 'Completed';
SELECT '  * In Progress: ' || COUNT(*) FROM orders WHERE status = 'In Progress';
SELECT '  * Scheduled: ' || COUNT(*) FROM orders WHERE status = 'Scheduled';
SELECT '  * Pending: ' || COUNT(*) FROM orders WHERE status = 'Pending';
SELECT '- Inventory Categories: ' || COUNT(*) FROM inventory_categories;
SELECT '- Inventory Items: ' || COUNT(*) FROM inventory_items;
SELECT '- Equipment: ' || COUNT(*) || ' tracked items' FROM equipment_maintenance;
SELECT '- Work Schedule: ' || COUNT(*) || ' work days' FROM work_schedule;
SELECT '- Volunteer Signups: ' || COUNT(*) FROM work_schedule_signups;
SELECT '- Certifications: ' || COUNT(*) FROM volunteer_certifications;
SELECT '- Delivery Log: ' || COUNT(*) || ' completed deliveries' FROM delivery_log;
SELECT '- Profile Change Requests: ' || COUNT(*) FROM profile_change_requests;
SELECT '';
SELECT 'Total Wood Delivered: ' || ROUND(SUM(delivered_cords), 2) || ' cords' FROM orders WHERE status = 'Completed';
SELECT 'Total Revenue: $' || ROUND(SUM(amount_paid), 2) FROM orders WHERE status = 'Completed';
SELECT 'Total Miles Driven: ' || ROUND(SUM(total_miles), 2) || ' miles' FROM delivery_log;
SELECT '';
SELECT 'Ready to test! Login with any of these accounts:';
SELECT '  - admin / admin (Administrator)';
SELECT '  - lead / lead (Team Lead)';
SELECT '  - user / user (Employee)';
SELECT '  - volunteer / volunteer (Volunteer)';
SELECT '  - jsmith / password123 (Employee)';
SELECT '  - bwilson / password123 (Lead)';
SELECT '============================================================================';

