# 📦 StockGuard — Inventory & Warehouse Management System

A comprehensive, multi-role console-based C++ application simulating a full retail & warehouse operation — from stock management and employee payroll to customer checkout and supplier fulfilment.

## ✨ Features

### 🔐 Multi-Role Portal System
- **Administrator Portal** — full system oversight and account management
- **Employee Portal** — role-based access for Warehouse Manager, Stock Keeper, and Finance Manager
- **Customer Checkout Portal** — sign up/sign in and shop directly from the system
- **Supplier Fulfilment Portal** — manage deliveries and contract invoices

### 👤 Admin Capabilities
- Register employee and supplier accounts with role assignment
- View all system data (warehouse & directory) in one place
- Audit depot & asset shortages across all storage sections
- Authorize and send employee payroll lists
- Generate a full **System Audit Log PDF Report**
- Create and delete storage sections, with automatic product reassignment

### 🏭 Warehouse Manager & Stock Keeper
- Assign routine tasks to staff
- Move items between storage sections
- View and generate depot stock shortage reports
- Request product restocks (with a $5,000 transaction limit for managers, unlimited for higher roles)
- View global system logs

### 📋 Stock & Product Management
- Register new products across 7 categories (Electronic, Fragile Electronic, Grocery, Perishable Grocery, Non-Perishable, Clothing, Veg/Fruit sold by weight)
- Automated expiry safety checks with hazard flagging for expired stock
- Manual stock adjustments and restocking
- Bulk & Seasonal discount policy engine

### 💰 Finance Manager
- Treasury & inventory financial health audits
- Record operational expense transactions
- Interactive monthly employee wage calculation & payment
- Process pending customer orders
- Handle warranty claims (replacement or refund)
- Export a full **Smart Finance Excel Worksheet (.xlsx)** report

### 🛒 Customer Experience
- Browse store products, purchase by unit (including weighted items like kg/g for produce)
- Track full purchase history
- File warranty claims for replacement or refund

### 🚚 Supplier Portal
- View and fulfil pending delivery orders
- View contract terms and invoice summaries

## 🖥️ How to Run

```bash
g++ inventory.cpp -o StockGuard
./StockGuard
```

## 📋 Main Menu
Administrator Portal
Employee Portal (StockKeeper / Manager / Finance)
Customer Checkout Portal
Supplier Fulfilment Portal
Exit Application

## 🛠️ Tech Used

- **Language:** C++
- **Concepts:** role-based access control, file I/O, manual PDF report generation, manual .xlsx (ZIP/XML) generation, discount & expiry-tracking logic, ANSI terminal colors

## 📌 Notes

Built as an advanced C++ project simulating a real-world retail & warehouse ecosystem — combining role-based portals, financial operations, inventory lifecycle management, and self-contained report generation (PDF & Excel) with no external dependencies.
