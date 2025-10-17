# 🔧 Sample Data Loading - Troubleshooting Guide

## Problem: Sample Data Not Appearing After Load

If you click "Load Sample Data" and don't see the data populate after refreshing, follow this guide.

---

## ✅ **What I Just Fixed:**

### 1. **Enhanced Debugging Output**
The system now prints detailed information to the console:
- Current working directory
- Application directory
- Each path it checks for SAMPLE_DATA.sql
- Whether the file exists at each location
- Number of SQL statements executed
- Success/failure count

### 2. **Added Missing Includes**
Added `QFileInfo` and `QCoreApplication` includes to `database.cpp` to fix path resolution.

### 3. **Better Error Messages**
The error dialog now shows:
- Expected file location
- Troubleshooting steps
- Link to check console output

---

## 🔍 **How to Diagnose the Issue:**

###Step 1: Check Console Output**

When you click "Load Sample Data", the console will show:

```
=== SAMPLE DATA LOAD START ===
Current directory: C:/Users/humbo/firewood_bank/build/bin/Release
Application path: C:/Users/humbo/firewood_bank/build/bin/Release
Loading sample data...
Current working directory: C:/Users/humbo/firewood_bank/build/bin/Release
Application directory: C:/Users/humbo/firewood_bank/build/bin/Release
Checking: C:/Users/humbo/firewood_bank/docs/SAMPLE_DATA.sql
✅ Found sample data file at: C:/Users/humbo/firewood_bank/docs/SAMPLE_DATA.sql
Loading SQL script from: C:/Users/humbo/firewood_bank/docs/SAMPLE_DATA.sql
Read 45237 bytes from SQL file
Found 432 SQL statements to execute
...
SQL Script Results:
  ✅ Success: 127
  ⚠️ Failed/Skipped: 12
  ℹ️ Comments/Empty: 293
✅ Transaction committed successfully
loadSampleData result: SUCCESS
Sample data loaded from SQL file, refreshing views...
Refreshing households model...
  Households now has 10 rows
Refreshing orders model...
  Orders now has 14 rows
Refreshing inventory model...
  Inventory now has 27 rows
=== SAMPLE DATA LOAD COMPLETE ===
```

---

## ❌ **Common Problems & Solutions:**

### Problem 1: "Could not find SAMPLE_DATA.sql"

**Console shows:**
```
ERROR: Could not find SAMPLE_DATA.sql in any expected location
Tried locations:
  - C:/Users/humbo/firewood_bank/docs/SAMPLE_DATA.sql [NOT FOUND]
  - ...
```

**Solution:**
1. Verify the file exists:
   ```powershell
   ls C:\Users\humbo\firewood_bank\docs\SAMPLE_DATA.sql
   ```

2. If missing, the file should be in your repository at `docs/SAMPLE_DATA.sql`

3. Try running from the project root:
   ```powershell
   cd C:\Users\humbo\firewood_bank
   .\build\bin\Release\firewood_bank.exe
   ```

---

### Problem 2: "File found but no data appears"

**Console shows:**
```
✅ Found sample data file
SQL Script Results:
  ✅ Success: 0
  ⚠️ Failed/Skipped: 432
```

**Possible Causes:**
- File is corrupted or has syntax errors
- Database constraints preventing insertion
- Data already exists (UNIQUE constraints)

**Solution:**
1. Check if data already exists:
   - Look at the Clients tab - are there already clients?
   - If yes, the data might have loaded before!

2. Try the "Clear ALL Data" function first (Admin only):
   - Admin Tools → Clear ALL Data
   - Then reload sample data

3. Check for specific error messages in console:
   ```
   WARNING: Failed to execute statement: INSERT INTO households...
     Error: UNIQUE constraint failed: households.id
   ```
   This means the data is already there!

---

### Problem 3: "Success message but tables still empty"

**Console shows:**
```
✅ Success: 127
Households now has 0 rows
```

**Solution:**
1. Press **F5** to refresh
2. Switch between tabs to trigger refresh
3. Restart the application
4. Check if you're logged in as the right user (Admin can see all tabs)

---

##🎯 **Quick Test:**

### **Method 1: From Project Root**
```powershell
cd C:\Users\humbo\firewood_bank
.\build\bin\Release\firewood_bank.exe
```

### **Method 2: Check File Exists**
```powershell
Test-Path "C:\Users\humbo\firewood_bank\docs\SAMPLE_DATA.sql"
```
Should return `True`

### **Method 3: Check File Size**
```powershell
(Get-Item "C:\Users\humbo\firewood_bank\docs\SAMPLE_DATA.sql").Length
```
Should show around **44-46 KB**

---

## 📊 **What the Sample Data Includes:**

When successful, you'll see:

### Clients Tab:
- Anderson Family
- Brown Family  
- Chen Family
- Davis Family
- Evans Family
- Foster Family
- Garcia Family
- Harris Family
- Iwasaki Family
- Johnson Family

### Orders Tab:
- 14 orders with various statuses
- Completed, In Progress, Scheduled, Pending
- Different dates and amounts

### Inventory Tab:
- **Wood**: Cords, Logs, Kindling
- **Safety Equipment**: Gloves, Glasses, Ear Protection, Hard Hats
- **Chainsaw Supplies**: Saws, Bars, Chains, Oil, Gas

---

## 🔄 **After Loading:**

1. **Dashboard** - Should show real statistics
2. **Clients Tab** - Should have 10+ entries
3. **Orders Tab** - Should have 14+ entries
4. **Inventory Tab** - Should have 25+ items
5. **Press F5** - Refreshes all views

---

## 🛠️ **Advanced Troubleshooting:**

### Check Database Directly:

```powershell
# Install SQLite tools if you haven't
# Then open the database:
sqlite3 "C:\Users\humbo\AppData\Roaming\FirewoodBank\firewood.db"

# Check if data exists:
SELECT COUNT(*) FROM households;
SELECT COUNT(*) FROM orders;
SELECT COUNT(*) FROM inventory_items;

# Exit:
.quit
```

---

## 📞 **Still Having Issues?**

1. **Compile a fresh build:**
   ```powershell
   cmake --build build --config Release
   ```

2. **Run with console visible:**
   - Open PowerShell
   - Navigate to: `cd C:\Users\humbo\firewood_bank`
   - Run: `.\build\bin\Release\firewood_bank.exe`
   - Watch the console output when you click "Load Sample Data"

3. **Copy the console output** and we can diagnose from there!

---

## ✅ **Expected Behavior:**

```
User clicks: Admin Tools → Load Sample Data
  ↓
Warning dialog: "This will populate your database..."
  ↓
User clicks: Yes
  ↓
Console shows: Detailed loading progress
  ↓
Success dialog: "✅ Sample Data Loaded!"
  ↓
Tables refresh and show new data
  ↓
Dashboard shows real statistics
```

---

## 💡 **Pro Tips:**

1. **First Time Setup:** Perfect for demos and testing
2. **Already Have Data?** Use "Clear ALL Data" first
3. **Training Session?** Load sample data before showing staff
4. **Testing Features?** Sample data has variety (completed orders, pending, etc.)
5. **Grant Demo?** Sample data shows the system in action!

---

**Created for the NMERA Firewood Bank Management System**

*Helping communities stay warm, one debug at a time!* 🌲🔥🪵


