# 📤 Import/Export Guide - Firewood Bank Management System

## Overview

The Firewood Bank Management System now includes comprehensive export functionality to help you share data with Excel, Google Sheets, Word, and other applications. This is especially useful for grant reporting, accounting, and backup purposes.

---

## ✅ Features Completed

### 1. **Export Clients/Households to CSV**
- **Access:** File → Export → Export Clients to CSV (Ctrl+Shift+C)
- **Includes:**
  - Name, phone, email
  - Physical and mailing addresses
  - Gate codes and delivery directions
  - Stove size
  - Volunteer status and requirements
  - Work-for-wood participation
  - Wood credit and balance
  - Order count and last order date
  - Referral source and notes

### 2. **Export Work Orders to CSV**
- **Access:** File → Export → Export Orders to CSV (Ctrl+Shift+O)
- **Includes:**
  - Order ID and client name
  - Dates (received, delivery, completed)
  - Requested vs. delivered cords
  - Status and priority
  - Driver assignment
  - Payment method and amount
  - Mileage tracking (start, end, total)
  - Notes and creator info
- **Perfect for:** Grant reports, financial tracking, mileage reimbursement

### 3. **Export Inventory to CSV**
- **Access:** File → Export → Export Inventory to CSV (Ctrl+Shift+V)
- **Includes:**
  - Category and item name
  - Quantity and unit
  - Location
  - Notes and timestamps
- **Useful for:** Supply ordering, inventory audits, budget planning

### 4. **Duplicate Detection (New!)**
- When adding a new client, the system automatically checks for duplicates by:
  - Name (case-insensitive)
  - Address (case-insensitive)
- If a match is found, you can:
  - ✅ **Yes** - Switch to editing the existing household
  - ❌ **No** - Add as a new household anyway
  - 🚫 **Cancel** - Go back and review
- Prevents accidental duplicate entries!

### 5. **Load Sample Data (Admin Only)**
- **Access:** Admin Tools → Load Sample Data
- Populates database with realistic test data:
  - 10 sample households/clients
  - 14 work orders (various statuses)
  - 25+ inventory items
  - 8 equipment records
  - Volunteer signups and certifications
  - 7 delivery log entries
- Perfect for training and demonstrations!

---

## 📋 How to Use

### Exporting Data

#### **Method 1: Menu Bar (Recommended)**
1. Click **File** menu
2. Hover over **📤 Export**
3. Choose what to export:
   - **👥 Export Clients to CSV**
   - **📋 Export Orders to CSV**
   - **📦 Export Inventory to CSV**
4. Choose where to save the file
5. Click **Save**

#### **Method 2: Keyboard Shortcuts**
- **Ctrl+Shift+C** - Export Clients
- **Ctrl+Shift+O** - Export Orders
- **Ctrl+Shift+V** - Export Inventory

### Opening Exported Files

#### **In Microsoft Excel:**
1. Open Excel
2. Click **File → Open**
3. Select your exported CSV file
4. Data will automatically populate in columns

#### **In Google Sheets:**
1. Go to Google Drive
2. Click **New → File Upload**
3. Upload your CSV file
4. Right-click the uploaded file
5. Choose **Open with → Google Sheets**

#### **In LibreOffice/OpenOffice Calc:**
1. Open Calc
2. Click **File → Open**
3. Select your CSV file
4. Confirm import settings if prompted

#### **In Microsoft Word (for mail merge, reports):**
1. Open Word
2. Click **Mailings → Select Recipients → Use an Existing List**
3. Browse to your CSV file
4. Use for mail merge, labels, or import into tables

---

## 🎯 Use Cases

### **Grant Reporting**
Export work orders to show:
- Number of households served
- Total cords delivered
- Geographic distribution
- Timeline of deliveries

**Steps:**
1. Export orders to CSV
2. Open in Excel/Google Sheets
3. Create pivot tables and charts
4. Copy data into grant application

### **Mileage Reimbursement**
Export orders with complete mileage data:
- Driver name
- Date of delivery
- Start and end odometer readings
- Total miles driven
- Client address

**Steps:**
1. Export orders to CSV
2. Filter by driver and date range
3. Calculate total reimbursement (miles × rate)
4. Submit to accounting

### **Client Contact Lists**
Export clients for:
- Phone trees
- Newsletter mailing lists
- Emergency notifications
- Thank you cards

**Steps:**
1. Export clients to CSV
2. Import into mail merge software
3. Generate letters or labels

### **Inventory Audits**
Export inventory for:
- Physical inventory counts
- Ordering supplies
- Budget planning
- Insurance documentation

**Steps:**
1. Export inventory to CSV
2. Print for physical audit
3. Update quantities
4. Compare against original

### **Board Reports**
Create professional reports with:
- Statistics dashboard screenshots
- Exported data tables
- Charts and graphs

**Steps:**
1. Export relevant data
2. Create visualizations in Excel
3. Copy into PowerPoint or Word
4. Present to board/funders

---

## 📊 CSV File Format

### Clients Export Includes:
```
ID, Name, Phone, Email, Address, Mailing Address, Gate Code, Stove Size,
Is Volunteer, Waiver Signed, Has License, Has Vehicle, Works for Wood,
Wood Credit Received, Credit Balance, Stock on Hand, Order Count, 
Last Order Date, Referral Source, Notes, Created At
```

### Orders Export Includes:
```
Order ID, Client Name, Date Received, Requested Cords, Delivered Cords, 
Status, Priority, Delivery Date, Delivery Address, Assigned Driver, 
Payment Method, Amount Paid, Delivery Time, Start Mileage, End Mileage, 
Total Miles, Completed Date, Notes, Created By, Created At
```

### Inventory Export Includes:
```
ID, Category, Item Name, Quantity, Unit, Location, Notes, 
Last Updated, Created At
```

---

## 🔐 Data Privacy & Security

### **What's Included in Exports:**
- ✅ Public business information
- ✅ Contact details
- ✅ Order history
- ✅ Delivery information

### **What's NOT Included:**
- ❌ User passwords
- ❌ Session tokens
- ❌ System configuration

### **Best Practices:**
1. **Store exported files securely** - Don't email unencrypted CSV files with client data
2. **Delete after use** - Remove exports when no longer needed
3. **Use password protection** - If storing in cloud, use encrypted folders
4. **Follow your privacy policy** - Ensure exports comply with your data policies

---

## 🚀 Future Enhancements (Planned)

### **PDF Export**
- Professional formatted reports
- Header with organization logo
- Summary statistics
- Individual client cards

### **Import from CSV**
- Bulk client import
- Order import from external systems
- Inventory updates via CSV

### **Filtered Exports**
- Export only specific date ranges
- Export by status (pending, completed)
- Export by driver or location

### **Scheduled Exports**
- Automatic weekly/monthly exports
- Email exports to administrators
- Backup to cloud storage

### **Excel Templates**
- Pre-formatted grant report templates
- Mileage reimbursement worksheets
- Board presentation templates

---

## 💡 Tips & Tricks

### **Tip 1: Consistent File Naming**
Exported files automatically include the date:
```
Firewood_Bank_Clients_2025-10-17.csv
Firewood_Bank_Orders_2025-10-17.csv
Firewood_Bank_Inventory_2025-10-17.csv
```

### **Tip 2: Quick Grant Reports**
1. Export orders to CSV
2. Open in Excel
3. Use built-in pivot tables:
   - Rows: Month
   - Values: Count of Orders, Sum of Delivered Cords
4. Insert chart → Done!

### **Tip 3: Backup Strategy**
Export all data monthly:
1. First of each month
2. Export clients, orders, and inventory
3. Store in secure location
4. Keep 12 months of history

### **Tip 4: Google Sheets Collaboration**
1. Export to CSV
2. Upload to Google Drive
3. Share with board members (view only)
4. Everyone sees latest data

### **Tip 5: Mailing Labels**
1. Export clients to CSV
2. Open in Word
3. Use **Mailings → Labels**
4. Select CSV as data source
5. Print on Avery labels

---

## 🆘 Troubleshooting

### **Problem: Can't open CSV file**
- **Solution:** Make sure you have Excel, Google Sheets, or LibreOffice installed
- **Alternative:** Right-click file → Open With → Choose application

### **Problem: Special characters look wrong**
- **Solution:** CSV uses UTF-8 encoding. In Excel, use **Data → Get External Data → From Text** and select UTF-8

### **Problem: Dates format incorrectly**
- **Solution:** Excel may auto-format dates. Select column → Format Cells → Text before importing

### **Problem: Too many columns**
- **Solution:** Hide unused columns in Excel. Select column → Right-click → Hide

### **Problem: File won't save to specific location**
- **Solution:** Check folder permissions. Try saving to Desktop first, then move file

---

## 📞 Support

If you need help with exports:
1. Check this guide first
2. Review the keyboard shortcuts (F1 in app)
3. Contact your system administrator
4. Email: humboldtfleamarket@gmail.com

---

## 🌟 Export Data Summary

When you export, you'll see a confirmation showing:
- **Number of records exported**
- **File location**
- **Suggested uses**

Example:
```
✅ Export Successful

Exported 45 clients to:
C:/Users/YourName/Firewood_Bank_Clients_2025-10-17.csv

You can now:
• Open in Excel
• Import to Google Sheets
• Open in any spreadsheet software
```

---

**Created by Kenneth Hanks for the Northern Mendocino Ecosystem Recovery Alliance**

*Everything Kenneth does for NMERA has been a donation. If you'd like to support his work, CashApp: $pseutheo*

🌲🔥🪵

