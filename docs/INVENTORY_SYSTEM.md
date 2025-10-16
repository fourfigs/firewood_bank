# Inventory System - Enhanced with Subcategories

## Overview

The Firewood Bank inventory system has been enhanced to support multiple inventory categories and equipment maintenance tracking.

## Database Schema

### New Tables

#### `inventory_categories`
Stores inventory category types.
- `id` - Primary key
- `name` - Category name (unique)
- `description` - Category description
- `created_at` - Creation timestamp

**Default Categories:**
1. **Wood** - Firewood inventory (cords, species, location)
2. **Safety Equipment** - Safety gear for volunteers and staff
3. **Chainsaw Supplies** - Tools and supplies for chainsaw operation

#### `inventory_items`
Stores individual inventory items.
- `id` - Primary key
- `category_id` - Foreign key to `inventory_categories`
- `item_name` - Name of the item
- `quantity` - Current quantity
- `unit` - Unit of measurement (units, cords, gallons, etc.)
- `location` - Storage location
- `notes` - Additional notes
- `last_updated` - Last update timestamp
- `created_at` - Creation timestamp

**Default Chainsaw Supplies Items:**
- Chainsaws
- Extra Bars
- Extra Chains
- 2-Cycle Oil
- Bar Oil
- Mixed Gas
- Unmixed Gas

**Default Safety Equipment Items:**
- Safety Glasses
- Hearing Protection
- Work Gloves
- Chainsaw Chaps
- Hard Hats
- First Aid Kits

#### `equipment_maintenance`
Tracks equipment service schedules and maintenance.
- `id` - Primary key
- `equipment_name` - Name of equipment (e.g., "Log Splitter")
- `current_hours` - Current operating hours
- `next_service_hours` - Hours at which next service is due
- `last_service_date` - Date of last service
- `last_service_notes` - Notes from last service
- `alert_threshold_hours` - Hours before service to trigger alert (default: 5)
- `notes` - General equipment notes
- `created_at` - Creation timestamp
- `updated_at` - Last update timestamp

**Default Equipment:**
- Log Splitter (service every 50 hours)

## User Interface

### Inventory Dialog
**File:** `src/ui/InventoryDialog.h`, `src/ui/InventoryDialog.cpp`

Features:
- Select inventory category
- Select or add new item within category
- Set quantity and unit
- Specify location
- Add notes
- Auto-detect existing items to prevent duplicates

### Equipment Maintenance Dialog
**File:** `src/ui/EquipmentMaintenanceDialog.h`, `src/ui/EquipmentMaintenanceDialog.cpp`

Features:
- Track current operating hours
- Set next service interval
- Visual alerts for upcoming/overdue service
  - ✓ Green: Service in more than 5 hours
  - ⚠️ Yellow: Service needed soon (within threshold)
  - ⚠️ Red: Service overdue
- Record service completion with date and notes
- Quick service recording button

### MainWindow Integration

**New Toolbar Actions (Admin/Employee only):**
- 📦 **Add Inventory** - Add or update inventory items
- ✏️ **Edit Inventory** - Edit selected inventory item
- 🔧 **Equipment** - Manage equipment maintenance schedules

**Inventory Tab:**
- Displays all inventory items with category names
- Double-click to edit item
- Shows: Category, Item Name, Quantity, Unit, Location, Notes

## Workflow Examples

### Adding Chainsaw Supplies

1. Click "📦 Add Inventory" in toolbar
2. Select "Chainsaw Supplies" category
3. Select item (e.g., "Mixed Gas") or click "+ New Item"
4. Enter quantity (e.g., 5.5)
5. Select unit (gallons)
6. Enter location (e.g., "Fuel Storage")
7. Click "💾 Save"

### Tracking Log Splitter Maintenance

1. Click "🔧 Equipment" in toolbar
2. Select "Log Splitter" from list
3. Update current hours (e.g., 45.0)
4. Set next service hours (e.g., 50.0)
5. System displays: "⚠️ Service needed soon! 5.0 hours remaining"
6. When service is completed:
   - Click "✅ Record Service Completed"
   - Enter service interval (e.g., 50 hours)
   - Enter service notes
   - Next service automatically calculated
7. Click "💾 Save"

### Managing Wood Inventory

1. Click "📦 Add Inventory" in toolbar
2. Select "Wood" category
3. Click "+ New Item" to add specific wood type
4. Enter name (e.g., "Oak - Split - Seasoned")
5. Enter quantity in cords
6. Specify location (e.g., "North Storage")
7. Click "💾 Save"

## Migration

The system automatically migrates existing inventory data from the old `inventory` table to the new `inventory_items` table under the "Wood" category. Old records are preserved and converted to the format: "Species - Form" (e.g., "Oak - Split").

## Permissions

- **View Inventory:** Admin, Employee
- **Edit Inventory:** Admin, Employee
- **Equipment Maintenance:** Admin, Employee
- **Volunteers:** No inventory access (dashboard only)

## Future Enhancements

Potential improvements:
- Inventory transaction history (additions/removals)
- Low stock alerts with configurable thresholds
- Barcode/QR code scanning for quick updates
- Inventory reports (usage over time, turnover rates)
- Equipment maintenance history log
- Automated service reminders via email
- Inventory valuation and cost tracking
- Multi-location management with transfer tracking

