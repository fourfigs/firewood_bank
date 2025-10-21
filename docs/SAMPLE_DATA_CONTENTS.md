# 📊 Sample Data Contents Summary

## **What Gets Loaded When You Run "Load Sample Data"**

---

## 👥 **USERS** (6 new users)
Password for all: `password123`

1. **jsmith** - Employee - John Smith
2. **mgarcia** - Employee - Maria Garcia  
3. **bwilson** - Lead - Bob Wilson
4. **sjohnson** - Volunteer - Sarah Johnson
5. **tmiller** - Volunteer - Tom Miller
6. **kdavis** - Volunteer - Karen Davis

---

## 🏢 **AGENCIES** (5 referral sources)

1. **Mendocino County Social Services** - Linda Martinez - Primary referral agency
2. **Redwood Community Services** - James Chen - Assists low-income families
3. **Long Valley Health Center** - Dr. Susan Brown - Medical referrals
4. **Round Valley Indian Tribes** - Michael Bear - Tribal member assistance
5. **Senior Center of Laytonville** - Betty White - Elderly assistance

---

## 🏠 **HOUSEHOLDS** (10 client families)

### Active Clients:
1. **Anderson Family** - Laytonville - Elderly couple, limited mobility
2. **Martinez Household** - Laytonville - Work-for-wood participant
3. **Thompson Family** - Laytonville - Single parent with 3 children
4. **Chen-Rodriguez Family** - Laytonville - Regular customer, very reliable
5. **Williams Household** - Covelo - Also volunteers on weekends

### Clients Needing Deliveries:
6. **Brown Family** - Covelo - Medical referral, priority client
7. **Davis Household** - Laytonville - Work-for-wood participant
8. **Garcia Extended Family** - Laytonville - New client, first delivery

### Volunteers Who Also Receive Wood:
9. **Johnson-Lee Family** - Laytonville - Volunteer driver
10. **Miller-White Household** - Laytonville - Volunteer for wood splitting

---

## 📦 **INVENTORY CATEGORIES** (5 categories)

1. **Wood** - Firewood in various forms
2. **Safety Equipment** - Personal protective equipment
3. **Chainsaw Supplies** - Chainsaw maintenance and fuel
4. **Tools** - General tools and equipment
5. **Vehicle Supplies** - Delivery vehicle maintenance

---

## 🪵 **INVENTORY ITEMS** (25+ items)

### Wood:
- Split Firewood (Dry) - 12.5 cords
- Split Firewood (Partially Seasoned) - 8.0 cords
- Rounds (Unsplit) - 15.0 cords
- Kindling Bundles - 50 bundles

### Safety Equipment:
- Safety Glasses - 15 pairs
- Work Gloves - 25 pairs
- Hard Hats - 8 units
- Ear Protection - 12 pairs
- Chainsaw Chaps - 6 pairs
- First Aid Kits - 3 kits

### Chainsaw Supplies:
- Chainsaw Chains - 8 chains
- Bar Oil - 12 gallons
- 2-Cycle Mix Gas - 8 gallons
- Air Filters - 10 filters
- Spark Plugs - 15 plugs

### Tools:
- Splitting Mauls (8 lb) - 6 units
- Splitting Wedges - 12 units
- Log Carriers - 8 units
- Measuring Tapes - 5 units
- Chainsaws (Operational) - 6 units

### Vehicle Supplies:
- Regular Gasoline - 15 gallons
- Motor Oil (10W-30) - 12 quarts

---

## 🔧 **EQUIPMENT MAINTENANCE** (5 pieces)

1. **Log Splitter** - 45 hours / Next service: 50 hours
2. **Chainsaw #1 (Stihl MS 291)** - 120 hours / Next service: 150 hours
3. **Chainsaw #2 (Husqvarna 450)** - 85 hours / Next service: 100 hours
4. **Delivery Truck (Ford F-350)** - 45,230 miles / Next service: 48,000 miles
5. **Utility Trailer** - Recent service / No issues

---

## 📅 **WORK SCHEDULE** (3 upcoming events)

1. **Wood Splitting Day** - Next Saturday - 6 volunteer slots
2. **Delivery Routes** - Next Monday - 4 volunteer slots
3. **Chainsaw Workshop** - Next Friday - 8 volunteer slots

---

## 🎓 **VOLUNTEER CERTIFICATIONS** (8 certifications)

Various certifications for volunteers including:
- Chainsaw Safety Certification
- First Aid/CPR
- CDL (for delivery drivers)
- Forklift Operation
- Food Handler (for community events)

---

## 📋 **ORDERS** (14 work orders)

### Completed Orders (4):
- Recent deliveries with full mileage tracking
- Payment methods: Cash, Check, Work-for-Wood

### In Progress (2):
- Currently out for delivery today

### Scheduled (5):
- Deliveries scheduled for next week
- Various priority levels

### Pending (3):
- New requests awaiting scheduling

---

## 🚚 **DELIVERY LOG** (5 completed deliveries)

Historical delivery records with:
- Driver names
- Mileage tracking (start/end odometer)
- Delivery dates and times
- Cords delivered
- Client information

---

## 📝 **PROFILE CHANGE REQUESTS** (2 sample requests)

- Example pending profile change requests
- Shows approval workflow

---

## 🔢 **TOTAL RECORDS**

- **Users:** 6 (+ 4 default = 10 total)
- **Agencies:** 5
- **Households:** 10
- **Inventory Categories:** 5
- **Inventory Items:** 25+
- **Equipment:** 5 pieces
- **Work Schedule:** 3 events
- **Volunteer Certifications:** 8
- **Orders:** 14
- **Delivery Logs:** 5
- **Profile Requests:** 2

---

## 📍 **DATABASE LOCATION**

The database will be created at:
```
C:\Users\[YourUsername]\AppData\Local\firewood_bank\firewood_bank.sqlite
```

---

## ✅ **HOW TO LOAD THIS DATA**

1. Run the Firewood Bank application
2. Login as **admin** / **admin**
3. Go to **Tools** → **Load Sample Data**
4. Click **Yes** to confirm
5. Success message will show all loaded data!

---

## 🔍 **HOW TO VERIFY THE DATA**

Run the PowerShell script:
```powershell
.\check_sample_data.ps1
```

Or manually check the database at the location above using:
- DB Browser for SQLite (recommended)
- SQLite command line tool
- Any SQLite viewer

---

**All data is realistic and ready for testing/demos!** 🌲🔥🪵

