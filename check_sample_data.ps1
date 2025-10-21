# Script to check what sample data was loaded into the database

$dbPath = "$env:LOCALAPPDATA\firewood_bank\firewood_bank.sqlite"

Write-Host "==================================================" -ForegroundColor Cyan
Write-Host "   FIREWOOD BANK DATABASE INSPECTION TOOL" -ForegroundColor Cyan
Write-Host "==================================================" -ForegroundColor Cyan
Write-Host ""

if (Test-Path $dbPath) {
    Write-Host "✅ Database found at: $dbPath" -ForegroundColor Green
    Write-Host ""
    
    # Check if sqlite3 is available
    $sqlite3Path = Get-Command sqlite3 -ErrorAction SilentlyContinue
    
    if ($sqlite3Path) {
        Write-Host "📊 DATABASE STATISTICS:" -ForegroundColor Yellow
        Write-Host "----------------------------------------" -ForegroundColor Yellow
        
        # Count households
        $householdCount = sqlite3 $dbPath "SELECT COUNT(*) FROM households;"
        Write-Host "👥 Households: $householdCount" -ForegroundColor White
        
        # Count orders
        $orderCount = sqlite3 $dbPath "SELECT COUNT(*) FROM orders;"
        Write-Host "📋 Orders: $orderCount" -ForegroundColor White
        
        # Count inventory items
        $inventoryCount = sqlite3 $dbPath "SELECT COUNT(*) FROM inventory_items;"
        Write-Host "📦 Inventory Items: $inventoryCount" -ForegroundColor White
        
        # Count delivery logs
        $deliveryCount = sqlite3 $dbPath "SELECT COUNT(*) FROM delivery_log;"
        Write-Host "🚚 Delivery Logs: $deliveryCount" -ForegroundColor White
        
        # Count agencies
        $agencyCount = sqlite3 $dbPath "SELECT COUNT(*) FROM agencies;"
        Write-Host "🏢 Agencies: $agencyCount" -ForegroundColor White
        
        Write-Host ""
        Write-Host "📋 SAMPLE HOUSEHOLDS:" -ForegroundColor Yellow
        Write-Host "----------------------------------------" -ForegroundColor Yellow
        sqlite3 $dbPath "SELECT id, name, address FROM households LIMIT 5;" -separator " | "
        
        Write-Host ""
        Write-Host "📦 SAMPLE INVENTORY:" -ForegroundColor Yellow
        Write-Host "----------------------------------------" -ForegroundColor Yellow
        sqlite3 $dbPath "SELECT name, quantity, unit, location FROM inventory_items LIMIT 10;" -separator " | "
        
        Write-Host ""
        Write-Host "📋 ORDER BREAKDOWN:" -ForegroundColor Yellow
        Write-Host "----------------------------------------" -ForegroundColor Yellow
        sqlite3 $dbPath "SELECT status, COUNT(*) as count FROM orders GROUP BY status;" -separator " | "
        
    } else {
        Write-Host "⚠️  SQLite3 command not found in PATH" -ForegroundColor Red
        Write-Host "Installing sqlite3..." -ForegroundColor Yellow
        
        # Try to use winget to install sqlite
        try {
            winget install sqlite.sqlite --silent
            Write-Host "✅ SQLite3 installed! Please run this script again." -ForegroundColor Green
        } catch {
            Write-Host "Please install SQLite3 manually or use DB Browser for SQLite" -ForegroundColor Yellow
            Write-Host "Database location: $dbPath" -ForegroundColor Cyan
        }
    }
    
} else {
    Write-Host "❌ Database not found!" -ForegroundColor Red
    Write-Host "Expected location: $dbPath" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Please:" -ForegroundColor Yellow
    Write-Host "  1. Run the Firewood Bank application" -ForegroundColor White
    Write-Host "  2. Login as admin / admin" -ForegroundColor White
    Write-Host "  3. Go to Tools → Load Sample Data" -ForegroundColor White
    Write-Host "  4. Run this script again" -ForegroundColor White
}

Write-Host ""
Write-Host "==================================================" -ForegroundColor Cyan

