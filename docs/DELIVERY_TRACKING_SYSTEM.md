# 🚚 Delivery Tracking System

## 📊 **NEW! Statistics Dashboard (Leads & Admins Only)**

When **Leads** or **Admins** log in, they now see a prominent statistics banner at the top of the dashboard showing:

### **👥 HOUSEHOLDS SERVED**
- Total count of active households

### **🪵 WOOD DELIVERED**
- **This Week**: Cords delivered (Monday-Sunday)
- **This Month**: Cords delivered this calendar month
- **This Year**: Cords delivered year-to-date
- **All Time**: Total cords ever delivered

### **💰 EXPENSES**
- **This Week**: Total payments received
- **This Month**: Total payments this month
- **This Year**: Total payments year-to-date
- **All Time**: Total revenue all time

> **Note**: Statistics only count **COMPLETED** orders!

---

## 🚛 **Delivery Tracking (Database Ready)**

### **Migration 9 Applied**
The database now tracks:
- `delivery_time` - Time driver departs for delivery
- `start_mileage` - Odometer reading at start
- `end_mileage` - Odometer reading at completion
- `completed_date` - When order was marked complete

### **Delivery Log Table**
New `delivery_log` table automatically created with:
- Driver name
- Delivery date and time
- Start/end mileage
- **Calculated total miles** (auto-computed!)
- Delivered cords
- Client name and address
- Timestamp

---

## 📝 **How It Will Work (Implementation Pending)**

### **Step 1: Order Created**
- Client requests wood (e.g., 3 cords)
- Status: "Pending"

### **Step 2: Delivery Scheduled**
- Driver assigned
- Delivery date set
- Status: "Scheduled"

### **Step 3: Driver Departs**
- Driver inputs departure time
- Driver inputs starting odometer reading
- Status: "In Progress"

### **Step 4: Delivery Completed**
- Driver inputs ending odometer reading
- Date auto-fills (current date)
- **Delivered cords calculated**: Each delivery ≈ 1/3 cord
- Status changes to "Completed"
- **Triggers automatic actions:**
  1. ✅ Delivery logged to `delivery_log` table
  2. ✅ Inventory auto-decremented (1/3 cord of split wood removed)
  3. ✅ Statistics updated (wood delivered, expenses)

---

## 🔢 **Auto-Inventory Update Logic**

When an order is marked **"Completed"**:

```
Delivered Cords = 0.33 cords (1/3 cord per delivery)

Inventory Update:
- Find "Split Wood" inventory item
- Subtract 0.33 cords from volume_cords
- If inventory < minimum threshold → Alert admin
```

---

## 📋 **Delivery Log for Leads**

Leads and Admins can view the **Delivery Log** showing:

| Driver | Date | Time | Start Miles | End Miles | Total Miles | Wood Delivered | Client |
|--------|------|------|-------------|-----------|-------------|----------------|--------|
| John D | 10/16 | 9:00 AM | 45,230 | 45,255 | 25 mi | 0.33 cords | Smith Family |
| Jane R | 10/16 | 2:30 PM | 38,100 | 38,118 | 18 mi | 0.33 cords | Jones Home |

This helps track:
- 🚗 Mileage reimbursement
- 📊 Delivery efficiency
- 👷 Driver activity
- 🪵 Wood distribution patterns

---

## 🎯 **Remaining Implementation Tasks**

### ✅ **COMPLETED**
1. ✅ Database migration 9 (delivery tracking fields)
2. ✅ Statistics dashboard UI (leads/admins only)
3. ✅ Statistics calculations (week/month/year/all-time)
4. ✅ Upcoming work orders list (date, client, contact)
5. ✅ Delivery log table structure

### 🔲 **TO DO**
1. 🔲 Add delivery tracking fields to WorkOrderDialog UI
2. 🔲 Implement auto-inventory update on order completion
3. 🔲 Create Delivery Log dialog/view for leads
4. 🔲 Add minimum inventory threshold alerts

---

## 🌟 **User Roles & Statistics Access**

| Role | Can View Statistics | Can View Delivery Log | Can Edit Inventory |
|------|---------------------|------------------------|-------------------|
| **Admin** | ✅ Yes | ✅ Yes | ✅ Yes |
| **Lead** | ✅ Yes | ✅ Yes | ✅ Yes |
| **Employee** | ❌ No | ❌ No | ❌ No (view only) |
| **Volunteer** | ❌ No | ❌ No | ❌ No |

---

## 🔥 **Visual Design**

The statistics panel uses:
- **Fiery orange gradient** header
- **Forest green** for household count
- **Success green** for all-time totals
- **Amber/gold** for expenses
- Large, bold numbers for easy readability
- Organized 3-column grid layout

---

## 💾 **Database Schema Changes**

### **orders table** (updated):
```sql
ALTER TABLE orders ADD COLUMN delivery_time TEXT;
ALTER TABLE orders ADD COLUMN start_mileage REAL DEFAULT 0;
ALTER TABLE orders ADD COLUMN end_mileage REAL DEFAULT 0;
ALTER TABLE orders ADD COLUMN completed_date TEXT;
```

### **delivery_log table** (new):
```sql
CREATE TABLE delivery_log (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  order_id INTEGER NOT NULL,
  driver TEXT NOT NULL,
  delivery_date TEXT NOT NULL,
  delivery_time TEXT,
  start_mileage REAL NOT NULL,
  end_mileage REAL NOT NULL,
  total_miles REAL GENERATED ALWAYS AS (end_mileage - start_mileage) STORED,
  delivered_cords REAL NOT NULL,
  client_name TEXT,
  client_address TEXT,
  logged_at TEXT DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE
);
```

---

## 🚀 **How to Test**

1. **Delete old database** to apply migration:
   ```powershell
   Remove-Item "$env:APPDATA\Roaming\FirewoodBank\firewood.db"
   ```

2. **Login as Lead or Admin**:
   - `lead` / `lead` - See statistics dashboard
   - `admin` / `admin` - See statistics dashboard

3. **Employees see NO statistics**:
   - `user` / `user` - No statistics panel (by design)

4. **Create test data**:
   - Add some households
   - Create work orders
   - Mark orders as "Completed" with delivery dates
   - Watch statistics update automatically!

---

## 📈 **Future Enhancements**

- **Export delivery log to CSV** for accounting
- **Driver performance metrics** (deliveries per day, avg miles)
- **Fuel cost calculator** (miles × cost per mile)
- **Monthly delivery reports** for grant applications
- **Inventory alerts** when stock is low
- **Automatic reorder notifications**

---

*This system helps Northern Mendocino Ecosystem Recovery Alliance track their impact and manage resources efficiently!* 🌲🔥🪵

