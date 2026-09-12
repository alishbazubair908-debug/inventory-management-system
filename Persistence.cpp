#include "Persistence.h"

using namespace std;

void loadSuppliersCSV(SystemState& state) {
    ifstream file("suppliers.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("ID,name") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 3) {
            state.suppliers.push_back(Supplier(tokens[0], tokens[1], tokens[2]));
        }
    }
    file.close();
}

void saveSuppliersCSV(const SystemState& state) {
    ofstream file("suppliers.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "ID,name,contract_terms\n";
    for (int i = 0; i < state.suppliers.size(); ++i) {
        const Supplier& sup = state.suppliers[i];
        file << sup.getSupplierID() << "," << sup.getSupplierName() << "," << sup.getContractTerms() << "\n";
    }
    file.close();
}

void loadCustomersCSV(SystemState& state) {
    ifstream file("customers.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("ID,name") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 3) {
            state.customers.push_back(Customer(tokens[0], tokens[1], tokens[2]));
        }
    }
    file.close();
}

void saveCustomersCSV(const SystemState& state) {
    ofstream file("customers.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "ID,name,contact,totalSpent\n";
    for (int i = 0; i < state.customers.size(); ++i) {
        const Customer& cust = state.customers[i];
        file << cust.getCustomerID() << "," << cust.getCustomerName() << "," << cust.getContactInfo() << "," << cust.getTotalSpent() << "\n";
    }
    file.close();
}

void loadEmployeesCSV(SystemState& state) {
    ifstream file("employees.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("ID,name") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 9) {
            string id = tokens[0];
            string name = tokens[1];
            string role = tokens[2];
            string dob = tokens[3];
            string joinDate = tokens[4];
            string shift = tokens[5];
            double salary = stod(tokens[6]);
            double rate = stod(tokens[7]);
            int extra = stoi(tokens[8]);
            int hours = 0;
            if (tokens.size() >= 10) {
                hours = stoi(tokens[9]);
            }
            string cnic = "";
            if (tokens.size() >= 11) {
                cnic = tokens[10];
            }
            if (role == "Warehouse Manager") {
                unique_ptr<WarehouseManager> wm = make_unique<WarehouseManager>(id, name, cnic, salary, shift, dob, joinDate, rate);
                wm->setHoursWorked(hours);
                state.employees.push_back(std::move(wm));
            }
            else if (role == "Stock Keeper") {
                unique_ptr<StockKeeper> sk = make_unique<StockKeeper>(id, name, cnic, salary, shift, dob, joinDate, rate, extra);
                sk->setHoursWorked(hours);
                state.employees.push_back(std::move(sk));
            }
            else if (role == "Finance Manager") {
                unique_ptr<FinanceManager> fm = make_unique<FinanceManager>(id, name, cnic, salary, shift, dob, joinDate, rate);
                fm->setHoursWorked(hours);
                state.employees.push_back(std::move(fm));
            }
        }
    }
    file.close();
}

void saveEmployeesCSV(const SystemState& state) {
    ofstream file("employees.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "ID,name,role,DOB,joining date,shift,salary,rate,extraField,hoursWorked,cnic\n";
    for (int i = 0; i < state.employees.size(); ++i) {
        const unique_ptr<Employee>& emp = state.employees[i];
        if (!emp) continue;
        int extra = 0;
        if (emp->getRole() == "Stock Keeper") {
            StockKeeper* sk = dynamic_cast<StockKeeper*>(emp.get());
            if (sk) extra = sk->getAssignedStorageNumber();
        }
        file << emp->getEmployeeID() << "," << emp->getName() << "," << emp->getRole() << ","
            << emp->getDOB() << "," << emp->getJoiningDate() << "," << emp->getShiftSchedule() << ","
            << emp->getSalary() << "," << emp->getHourlyRate() << "," << extra << "," << emp->getHoursWorked() << ","
            << emp->getCNIC() << "\n";
    }
    file.close();
}

void loadProductsCSV(SystemState& state) {
    ifstream file("products.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("ID,name") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 9) {
            string id = tokens[0];
            string name = tokens[1];
            double price = stod(tokens[2]);
            int qty = stoi(tokens[3]);
            string cat = tokens[4];
            string sub = tokens[5];
            int storageNum = stoi(tokens[6]);

            while (state.globalWarehouse.getStorageCount() < storageNum) {
                int nextID = state.globalWarehouse.getStorageCount() + 1;
                state.globalWarehouse.addStorage(Storage(nextID, 50, "Any"));
            }

            Barcode b(tokens[7], tokens[8]);
            try {
                if (cat == "Electronic") {
                    if (sub == "Standard" && tokens.size() >= 12) {
                        unique_ptr<ElectronicProduct> p = make_unique<ElectronicProduct>(id, name, price, qty, b, stoi(tokens[9]), stoi(tokens[10]), tokens[11]);
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                    else if (sub == "Fragile" && tokens.size() >= 14) {
                        unique_ptr<FragileElectronics> p = make_unique<FragileElectronics>(id, name, price, qty, b, stoi(tokens[9]), stoi(tokens[10]), tokens[11], stoi(tokens[12]), tokens[13]);
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                }
                else if (cat == "Grocery") {
                    if (sub == "Standard" && tokens.size() >= 12) {
                        unique_ptr<GroceryProduct> p = make_unique<GroceryProduct>(id, name, price, qty, b, stoi(tokens[9]), tokens[10] == "1", tokens[11]);
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                    else if (sub == "VegFruit" && tokens.size() >= 12) {
                        unique_ptr<VegFruitGrocery> p = make_unique<VegFruitGrocery>(id, name, price, qty, b, stoi(tokens[9]), tokens[10] == "1", tokens[11]);
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                    else if (sub == "Perishable" && tokens.size() >= 14) {
                        unique_ptr<PerishableGrocery> p = make_unique<PerishableGrocery>(id, name, price, qty, b, stoi(tokens[9]), tokens[10] == "1", tokens[11], tokens[12], stod(tokens[13]));
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                    else if (sub == "Non-Perishable" && tokens.size() >= 14) {
                        unique_ptr<NonPerishable> p = make_unique<NonPerishable>(id, name, price, qty, b, stoi(tokens[9]), tokens[10] == "1", tokens[11], stoi(tokens[12]), tokens[13]);
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                }
                else if (cat == "Clothing") {
                    if (tokens.size() >= 12) {
                        unique_ptr<ClothingProduct> p = make_unique<ClothingProduct>(id, name, price, qty, b, tokens[9], tokens[10], tokens[11]);
                        state.globalWarehouse.getStorage(storageNum - 1).addStock(std::move(p));
                    }
                }
            }
            catch (...) {}
        }
    }
    file.close();
}

void saveProductsCSV(const SystemState& state) {
    ofstream file("products.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "ID,name,price,qty,category,subtype,storageNumber,barcodeVal,barcodeFmt,fields...\n";
    for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
        const Storage& st = state.globalWarehouse.getStorage(i);
        for (int j = 0; j < st.getProductCount(); ++j) {
            const Product* p = st[j];
            if (p) {
                file << p->serialize() << "\n";
            }
        }
    }
    file.close();
}

void loadOrdersCSV(SystemState& state) {
    ifstream file("orders.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("orderID") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 7) {
            CustomerOrder ord;
            ord.orderID = tokens[0];
            ord.customerID = tokens[1];
            ord.productID = tokens[2];
            ord.quantity = stoi(tokens[3]);
            ord.totalPrice = stod(tokens[4]);
            ord.isApproved = (tokens[5] == "1");
            ord.isRejected = (tokens[6] == "1");
            state.pendingOrders.push_back(ord);
            if (ord.isApproved) {
                for (int i = 0; i < state.customers.size(); ++i) {
                    if (state.customers[i].getCustomerID() == ord.customerID) {
                        Product* p = state.globalWarehouse.findProductByID(ord.productID);
                        PurchasedItem item;
                        item.productID = ord.productID;
                        item.productName = p ? p->getProductName() : "Unknown Asset";
                        item.quantity = ord.quantity;
                        item.pricePaid = ord.totalPrice;
                        item.purchaseDate = getCurrentDateTime().substr(0, 10);
                        item.warrantyMonths = p ? p->getWarrantyMonths() : 0;
                        state.customers[i].addPurchasedItem(item);
                        break;
                    }
                }
            }
        }
    }
    file.close();
}

void saveOrdersCSV(const SystemState& state) {
    ofstream file("orders.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "orderID,customerID,productID,qty,totalPrice,isApproved,isRejected\n";
    for (int i = 0; i < state.pendingOrders.size(); ++i) {
        const CustomerOrder& ord = state.pendingOrders[i];
        file << ord.orderID << "," << ord.customerID << "," << ord.productID << ","
            << ord.quantity << "," << ord.totalPrice << "," << (ord.isApproved ? 1 : 0) << ","
            << (ord.isRejected ? 1 : 0) << "\n";
    }
    file.close();
}

void loadWarrantyClaimsCSV(SystemState& state) {
    ifstream file("warranty_claims.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("claimID") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 6) {
            WarrantyClaim cl;
            cl.claimID = tokens[0];
            cl.customerID = tokens[1];
            cl.productID = tokens[2];
            cl.claimType = tokens[3];
            cl.refundAmount = stod(tokens[4]);
            cl.isProcessed = (tokens[5] == "1");
            state.warrantyClaims.push_back(cl);
        }
    }
    file.close();
}

void saveWarrantyClaimsCSV(const SystemState& state) {
    ofstream file("warranty_claims.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "claimID,customerID,productID,claimType,refundAmount,isProcessed\n";
    for (int i = 0; i < state.warrantyClaims.size(); ++i) {
        const WarrantyClaim& cl = state.warrantyClaims[i];
        file << cl.claimID << "," << cl.customerID << "," << cl.productID << ","
            << cl.claimType << "," << cl.refundAmount << "," << (cl.isProcessed ? 1 : 0) << "\n";
    }
    file.close();
}

void loadRestockRequestsCSV(SystemState& state) {
    ifstream file("restock_requests.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("orderID") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 5) {
            RestockRequest req;
            req.orderID = tokens[0];
            req.supplierID = tokens[1];
            req.productID = tokens[2];
            req.quantityRequested = stoi(tokens[3]);
            req.isFulfilled = (tokens[4] == "1");
            state.restockRequests.push_back(req);
        }
    }
    file.close();
}

void saveRestockRequestsCSV(const SystemState& state) {
    ofstream file("restock_requests.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "orderID,supplierID,productID,qty,isFulfilled\n";
    for (int i = 0; i < state.restockRequests.size(); ++i) {
        const RestockRequest& req = state.restockRequests[i];
        file << req.orderID << "," << req.supplierID << "," << req.productID << ","
            << req.quantityRequested << "," << (req.isFulfilled ? 1 : 0) << "\n";
    }
    file.close();
}

void loadSystemLogsCSV(SystemState& state) {
    ifstream file("system_logs.csv");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("timestamp") == 0) continue;
        size_t commaPos = line.find(',');
        if (commaPos != string::npos) {
            string ts = line.substr(0, commaPos);
            string action = line.substr(commaPos + 1);
            state.systemLogs.recordActionWithTimestamp(action, ts);
        }
    }
    file.close();
}

void saveSystemLogsCSV(const SystemState& state) {
    ofstream file("system_logs.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "timestamp,action\n";
    for (int i = 0; i < state.systemLogs.getLogCount(); ++i) {
        file << state.systemLogs.getTimestamp(i) << "," << state.systemLogs.getLogEntry(i) << "\n";
    }
    file.close();
}

void loadStoragesCSV(SystemState& state) {
    state.globalWarehouse.clearStorages();
    ifstream file("storages.csv");
    if (!file.is_open()) {
        Storage Storage1(1, 10, "Electronic");
        Storage Storage2(2, 10, "Clothing");
        Storage Storage3(3, 10, "Grocery");
        state.globalWarehouse.addStorage(std::move(Storage1));
        state.globalWarehouse.addStorage(std::move(Storage2));
        state.globalWarehouse.addStorage(std::move(Storage3));
        return;
    }
    string line;
    while (getline(file, line)) {
        if (line.empty() || line.find("storageNumber") == 0) continue;
        DynamicArray<string> tokens = splitString(line, ',');
        if (tokens.size() >= 3) {
            int num = stoi(tokens[0]);
            int cap = stoi(tokens[1]);
            string cat = tokens[2];
            state.globalWarehouse.addStorage(Storage(num, cap, cat));
        }
    }
    file.close();
}

void saveStoragesCSV(const SystemState& state) {
    ofstream file("storages.csv", ios::trunc);
    if (!file.is_open()) return;
    file << "storageNumber,capacity,allowedCategory\n";
    for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
        const Storage& s = state.globalWarehouse.getStorage(i);
        file << s.getStorageNumber() << "," << s.getCapacity() << "," << s.getAllowedCategory() << "\n";
    }
    file.close();
}

void loadAllCSVFiles(SystemState& state) {
    loadStoragesCSV(state);
    loadSuppliersCSV(state);
    loadCustomersCSV(state);
    loadEmployeesCSV(state);
    loadProductsCSV(state);
    loadOrdersCSV(state);
    loadWarrantyClaimsCSV(state);
    loadRestockRequestsCSV(state);
    loadSystemLogsCSV(state);
}

void saveAllCSVFiles(const SystemState& state) {
    saveStoragesCSV(state);
    saveSuppliersCSV(state);
    saveCustomersCSV(state);
    saveEmployeesCSV(state);
    saveProductsCSV(state);
    saveOrdersCSV(state);
    saveWarrantyClaimsCSV(state);
    saveRestockRequestsCSV(state);
    saveSystemLogsCSV(state);
}

void appendProductRecord(const string& filename, const string& record) {
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << record << "\n";
        file.close();
    }
}

void generateWarehouseReport(const SystemState& state) {
    ofstream file("warehouse_report.pdf");
    if (!file.is_open()) return;
    file << "==========================================\n"
        << "        WAREHOUSE INVENTORY REPORT        \n"
        << "==========================================\n"
        << "Generated: " << getCurrentDateTime() << "\n\n"
        << state.globalWarehouse << "\n"
        << "==========================================\n"
        << "Total Asset Valuation: $" << fixed << setprecision(2) << state.globalWarehouse.getGlobalValue() << "\n";
    file.close();
    cout << "[Report Compiled] warehouse_report.pdf created." << endl;
}

void generatePayrollReport(const SystemState& state) {
    ofstream file("payroll_report.pdf");
    if (!file.is_open()) return;
    file << "==========================================\n"
        << "           MONTHLY PAYROLL REPORT         \n"
        << "==========================================\n"
        << "Generated: " << getCurrentDateTime() << "\n\n";
    double total = 0.0;
    for (int i = 0; i < state.employees.size(); ++i) {
        const unique_ptr<Employee>& emp = state.employees[i];
        if (emp) {
            double wage = emp->calculateMonthlyWage();
            total += wage;
            file << "Employee: " << emp->getName() << " (ID: " << emp->getEmployeeID() << ")\n"
                << "  - CNIC: " << emp->getCNIC() << "\n"
                << "  - Role: " << emp->getRole() << "\n"
                << "  - Net Wage: $" << fixed << setprecision(2) << wage << "\n\n";
        }
    }
    file << "==========================================\n"
        << "Corporate Treasury Funds    : $" << state.availableCapital << "\n"
        << "Accumulated Operating Costs : $" << state.totalOperatingExpenses << "\n"
        << "Payroll Expenditures Paid   : $" << total << "\n"
        << "==========================================\n";
    file.close();
    cout << "[Report Compiled] payroll_report.pdf created." << endl;
}

void generateOrdersReport(const SystemState& state) {
    ofstream file("orders_report.pdf");
    if (!file.is_open()) return;
    file << "==========================================\n"
        << "       ORDERS & WARRANTY CLAIMS REPORT    \n"
        << "==========================================\n"
        << "Generated: " << getCurrentDateTime() << "\n\n"
        << "--- CUSTOMER ORDERS ---\n";
    for (int i = 0; i < state.pendingOrders.size(); ++i) {
        const CustomerOrder& ord = state.pendingOrders[i];
        file << "Order ID: " << ord.orderID
            << " | Customer: " << ord.customerID
            << " | Product: " << ord.productID
            << " | Qty: " << ord.quantity
            << " | Total: $" << fixed << setprecision(2) << ord.totalPrice
            << " | Status: " << (ord.isApproved ? "Approved" : (ord.isRejected ? "Rejected" : "Pending")) << "\n";
    }
    file << "\n--- WARRANTY CLAIMS ---\n";
    for (int i = 0; i < state.warrantyClaims.size(); ++i) {
        const WarrantyClaim& cl = state.warrantyClaims[i];
        file << "Claim ID: " << cl.claimID
            << " | Customer: " << cl.customerID
            << " | Product: " << cl.productID
            << " | Request: " << cl.claimType
            << " | Value: $" << fixed << setprecision(2) << cl.refundAmount
            << " | Processed: " << (cl.isProcessed ? "Yes" : "No") << "\n";
    }
    file.close();
    cout << "[Report Compiled] orders_report.pdf created." << endl;
}

void generateAuditLogReport(const SystemState& state) {
    ofstream file("audit_log.pdf");
    if (!file.is_open()) return;
    file << "==========================================\n"
        << "           SYSTEM AUDIT LOG REPORT        \n"
        << "==========================================\n"
        << "Generated: " << getCurrentDateTime() << "\n\n";
    state.systemLogs.printAuditTrailToFile(file);
    file.close();
    cout << "[Report Compiled] audit_log.pdf created." << endl;
}

void generateExpiryReport(SystemState& state) {
    ofstream file("expiry_report.pdf");
    if (!file.is_open()) return;
    file << "==========================================\n"
        << "       EXPIRED / SHELF-LIFE REPORT        \n"
        << "==========================================\n"
        << "Generated: " << getCurrentDateTime() << "\n\n";
    for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
        Storage& st = state.globalWarehouse.getStorage(i);
        for (int j = 0; j < st.getProductCount(); ++j) {
            Product* p = st[j];
            if (p) {
                if (p->getSubtype() == "Perishable") {
                    PerishableGrocery* pg = dynamic_cast<PerishableGrocery*>(p);
                    if (pg) {
                        file << "Product: " << pg->getProductName() << " (ID: " << pg->getProductID() << ")\n"
                            << "  - Expiry Date: " << pg->getExpiryDate() << "\n"
                            << "  - Status: " << (pg->checkExpiry() ? "EXPIRED" : "SAFE") << "\n\n";
                    }
                }
                else if (p->getSubtype() == "Non-Perishable") {
                    NonPerishable* np = dynamic_cast<NonPerishable*>(p);
                    if (np) {
                        file << "Product: " << np->getProductName() << " (ID: " << np->getProductID() << ")\n"
                            << "  - Preservation: " << np->getStorageInstructions() << "\n\n";
                    }
                }
            }
        }
    }
    file.close();
    cout << "[Report Generated] expiry_report.pdf created successfully." << endl;
}

void generateCustomerReceipt(const CustomerOrder& ord, const string& custName) {
    string filename = "receipt_" + ord.customerID + ".pdf";
    ofstream file(filename);
    if (!file.is_open()) return;
    file << "==========================================\n"
        << "            TRANSACTION RECEIPT           \n"
        << "==========================================\n"
        << "Transaction Code: " << ord.orderID << "\n"
        << "Client Name     : " << custName << " (ID: " << ord.customerID << ")\n"
        << "Timestamp       : " << getCurrentDateTime() << "\n"
        << "==========================================\n"
        << "Purchased item ID: " << ord.productID << "\n"
        << "Units acquired   : " << ord.quantity << "\n"
        << "Total paid       : $" << fixed << setprecision(2) << ord.totalPrice << "\n"
        << "==========================================\n"
        << "Thank you for shopping with us!\n";
    file.close();
    cout << "[Receipt PDF Exported] " << filename << " generated." << endl;
}

static uint32_t _crc_table[256];
static bool _crc_ready = false;

static void _init_crc() {
    if (_crc_ready) return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        _crc_table[i] = c;
    }
    _crc_ready = true;
}

static uint32_t _crc32s(const string& d) {
    _init_crc();
    uint32_t c = 0xFFFFFFFFu;
    for (unsigned char b : d)
        c = _crc_table[(c ^ b) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

struct _ZEntry {
    string name;
    string data;
    uint32_t crc = 0;
    uint32_t off = 0;
};

static void _pu16(DynamicArray<uint8_t>& b, uint16_t v) {
    b.push_back(static_cast<uint8_t>(v & 0xFF));
    b.push_back(static_cast<uint8_t>(v >> 8));
}

static void _pu32(DynamicArray<uint8_t>& b, uint32_t v) {
    b.push_back(static_cast<uint8_t>(v & 0xFF));
    b.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    b.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    b.push_back(static_cast<uint8_t>(v >> 24));
}

static void _pstr(DynamicArray<uint8_t>& b, const string& s) {
    for (char c : s) b.push_back(static_cast<uint8_t>(c));
}

static DynamicArray<uint8_t> _makeZip(DynamicArray<_ZEntry>& es) {
    DynamicArray<uint8_t> buf;
    for (int i = 0; i < es.size(); ++i) {
        es[i].off = static_cast<uint32_t>(buf.size());
        es[i].crc = _crc32s(es[i].data);
        _pu32(buf, 0x04034b50); _pu16(buf, 20); _pu16(buf, 0); _pu16(buf, 0);
        _pu16(buf, 0); _pu16(buf, 0);
        _pu32(buf, es[i].crc);
        _pu32(buf, static_cast<uint32_t>(es[i].data.size()));
        _pu32(buf, static_cast<uint32_t>(es[i].data.size()));
        _pu16(buf, static_cast<uint16_t>(es[i].name.size())); _pu16(buf, 0);
        _pstr(buf, es[i].name); _pstr(buf, es[i].data);
    }
    uint32_t cdOff = static_cast<uint32_t>(buf.size());
    for (int i = 0; i < es.size(); ++i) {
        _pu32(buf, 0x02014b50); _pu16(buf, 20); _pu16(buf, 20);
        _pu16(buf, 0); _pu16(buf, 0); _pu16(buf, 0); _pu16(buf, 0);
        _pu32(buf, es[i].crc);
        _pu32(buf, static_cast<uint32_t>(es[i].data.size()));
        _pu32(buf, static_cast<uint32_t>(es[i].data.size()));
        _pu16(buf, static_cast<uint16_t>(es[i].name.size())); _pu16(buf, 0); _pu16(buf, 0);
        _pu16(buf, 0); _pu16(buf, 0); _pu32(buf, 0); _pu32(buf, es[i].off);
        _pstr(buf, es[i].name);
    }
    uint32_t cdSz = static_cast<uint32_t>(buf.size()) - cdOff;
    _pu32(buf, 0x06054b50); _pu16(buf, 0); _pu16(buf, 0);
    _pu16(buf, static_cast<uint16_t>(es.size())); _pu16(buf, static_cast<uint16_t>(es.size()));
    _pu32(buf, cdSz); _pu32(buf, cdOff); _pu16(buf, 0);
    return buf;
}

struct _SS {
    DynamicArray<string> strs;

    int add(const string& s) {
        for (int i = 0; i < strs.size(); i++)
            if (strs[i] == s) return i;
        strs.push_back(s);
        return strs.size() - 1;
    }

    static string escapeXml(const string& str) {
        ostringstream oss;
        for (char c : str) {
            switch (c) {
            case '&':  oss << "&amp;"; break;
            case '<':  oss << "&lt;"; break;
            case '>':  oss << "&gt;"; break;
            case '"':  oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default:   oss << c; break;
            }
        }
        return oss.str();
    }

    string xml() const {
        ostringstream o;
        o << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            << "<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\""
            << " count=\"" << strs.size() << "\" uniqueCount=\"" << strs.size() << "\">";
        for (int i = 0; i < strs.size(); ++i)
            o << "<si><t xml:space=\"preserve\">" << escapeXml(strs[i]) << "</t></si>";
        o << "</sst>";
        return o.str();
    }
};

static string _col(int c) {
    string r;
    while (c > 0) { r = static_cast<char>('A' + (c - 1) % 26) + r; c = (c - 1) / 26; }
    return r;
}

static string _ref(int r, int c) {
    return _col(c) + to_string(r);
}

static string _sc(int r, int c, int si, int s = 0) {
    ostringstream o;
    o << "<c r=\"" << _ref(r, c) << "\" t=\"s\" s=\"" << s << "\"><v>" << si << "</v></c>";
    return o.str();
}

static string _nc(int r, int c, double v, int s = 0) {
    ostringstream o;
    o << "<c r=\"" << _ref(r, c) << "\" s=\"" << s << "\"><v>"
        << fixed << setprecision(2) << v << "</v></c>";
    return o.str();
}

static string _fc(int r, int c, const string& f, int s = 0) {
    ostringstream o;
    o << "<c r=\"" << _ref(r, c) << "\" s=\"" << s << "\"><f>" << f << "</f></c>";
    return o.str();
}

static string _rO(int r) { return "<row r=\"" + to_string(r) + "\">"; }
static string _rC() { return "</row>"; }
static string _shO(const string& cw) {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">"
        + cw + "<sheetData>";
}
static string _shC() { return "</sheetData></worksheet>"; }

static string _stylesXml() {
    return
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">"
        "<numFmts count=\"1\">"
        "<numFmt numFmtId=\"164\" formatCode=\"\\$#,##0.00;(\\$#,##0.00);&quot;-&quot;\"/>"
        "</numFmts>"
        "<fonts count=\"6\">"
        "<font><sz val=\"11\"/><name val=\"Arial\"/></font>"
        "<font><sz val=\"11\"/><name val=\"Arial\"/><color rgb=\"FF0000FF\"/></font>"
        "<font><sz val=\"11\"/><name val=\"Arial\"/><b/></font>"
        "<font><sz val=\"11\"/><name val=\"Arial\"/><color rgb=\"FFFFFFFF\"/><b/></font>"
        "<font><sz val=\"12\"/><name val=\"Arial\"/><b/><color rgb=\"FF375623\"/></font>"
        "<font><sz val=\"11\"/><name val=\"Arial\"/><b/><color rgb=\"FF000000\"/></font>"
        "</fonts>"
        "<fills count=\"5\">"
        "<fill><patternFill patternType=\"none\"/></fill>"
        "<fill><patternFill patternType=\"gray125\"/></fill>"
        "<fill><patternFill patternType=\"solid\"><fgColor rgb=\"FF1F4E79\"/></patternFill></fill>"
        "<fill><patternFill patternType=\"solid\"><fgColor rgb=\"FFFFF2CC\"/></patternFill></fill>"
        "<fill><patternFill patternType=\"solid\"><fgColor rgb=\"FFE2EFDA\"/></patternFill></fill>"
        "</fills>"
        "<borders count=\"1\"><border><left/><right/><top/><bottom/><diagonal/></border></borders>"
        "<cellStyleXfs count=\"1\"><xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\"/></cellStyleXfs>"
        "<cellXfs count=\"8\">"
        "<xf numFmtId=\"0\"   fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\"/>"
        "<xf numFmtId=\"164\" fontId=\"1\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyFont=\"1\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"164\" fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"0\"   fontId=\"3\" fillId=\"2\" borderId=\"0\" xfId=\"0\" applyFont=\"1\" applyFill=\"1\" applyAlignment=\"1\"><alignment horizontal=\"center\"/></xf>"
        "<xf numFmtId=\"164\" fontId=\"5\" fillId=\"3\" borderId=\"0\" xfId=\"0\" applyFont=\"1\" applyFill=\"1\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"0\"   fontId=\"2\" fillId=\"0\" borderId=\"0\" xfId=\"0\" applyFont=\"1\"/>"
        "<xf numFmtId=\"164\" fontId=\"4\" fillId=\"4\" borderId=\"0\" xfId=\"0\" applyFont=\"1\" applyFill=\"1\" applyNumberFormat=\"1\"/>"
        "<xf numFmtId=\"0\"   fontId=\"3\" fillId=\"2\" borderId=\"0\" xfId=\"0\" applyFont=\"1\" applyFill=\"1\"/>"
        "</cellXfs>"
        "</styleSheet>";
}

void generateFinanceExcel(const SystemState& state) {
    _SS ss;

    ostringstream sales;
    sales << _shO(
        "<cols>"
        "<col min=\"1\" max=\"1\" width=\"18\" customWidth=\"1\"/>"
        "<col min=\"2\" max=\"2\" width=\"16\" customWidth=\"1\"/>"
        "<col min=\"3\" max=\"3\" width=\"16\" customWidth=\"1\"/>"
        "<col min=\"4\" max=\"4\" width=\"12\" customWidth=\"1\"/>"
        "<col min=\"5\" max=\"5\" width=\"18\" customWidth=\"1\"/>"
        "<col min=\"6\" max=\"6\" width=\"22\" customWidth=\"1\"/>"
        "</cols>"
    );
    sales << _rO(1)
        << "<c r=\"A1\" t=\"s\" s=\"7\"><v>" << ss.add("CUSTOMER SALES ORDERS") << "</v></c>"
        << _rC();
    sales << _rO(2)
        << _sc(2, 1, ss.add("Order ID"), 3)
        << _sc(2, 2, ss.add("Customer ID"), 3)
        << _sc(2, 3, ss.add("Product ID"), 3)
        << _sc(2, 4, ss.add("Quantity"), 3)
        << _sc(2, 5, ss.add("Unit Price ($)"), 3)
        << _sc(2, 6, ss.add("Total Revenue ($)"), 3)
        << _rC();
    int sRow = 3;
    for (int i = 0; i < state.pendingOrders.size(); ++i) {
        const CustomerOrder& ord = state.pendingOrders[i];
        if (!ord.isApproved) continue;
        double unit = (ord.quantity > 0) ? (ord.totalPrice / static_cast<double>(ord.quantity)) : 0.0;
        sales << _rO(sRow)
            << _sc(sRow, 1, ss.add(ord.orderID), 0)
            << _sc(sRow, 2, ss.add(ord.customerID), 0)
            << _sc(sRow, 3, ss.add(ord.productID), 0)
            << _nc(sRow, 4, static_cast<double>(ord.quantity), 0)
            << _nc(sRow, 5, unit, 1)
            << _fc(sRow, 6, "D" + to_string(sRow) + "*E" + to_string(sRow), 2)
            << _rC();
        sRow++;
    }
    int salesTotalRow = sRow;
    sales << _rO(sRow)
        << "<c r=\"E" << sRow << "\" t=\"s\" s=\"5\"><v>" << ss.add("TOTAL") << "</v></c>"
        << _fc(sRow, 6, (sRow > 3) ? "SUM(F3:F" + to_string(sRow - 1) + ")" : "0", 4)
        << _rC();
    sales << _shC();

    ostringstream payroll;
    payroll << _shO(
        "<cols>"
        "<col min=\"1\" max=\"1\" width=\"16\" customWidth=\"1\"/>"
        "<col min=\"2\" max=\"2\" width=\"22\" customWidth=\"1\"/>"
        "<col min=\"3\" max=\"3\" width=\"22\" customWidth=\"1\"/>"
        "<col min=\"4\" max=\"4\" width=\"16\" customWidth=\"1\"/>"
        "<col min=\"5\" max=\"5\" width=\"14\" customWidth=\"1\"/>"
        "<col min=\"6\" max=\"6\" width=\"14\" customWidth=\"1\"/>"
        "<col min=\"7\" max=\"7\" width=\"20\" customWidth=\"1\"/>"
        "</cols>"
    );
    payroll << _rO(1)
        << "<c r=\"A1\" t=\"s\" s=\"7\"><v>" << ss.add("EMPLOYEE PAYROLL WORKSHEET") << "</v></c>"
        << _rC();
    payroll << _rO(2)
        << _sc(2, 1, ss.add("Employee ID"), 3)
        << _sc(2, 2, ss.add("Name"), 3)
        << _sc(2, 3, ss.add("Role"), 3)
        << _sc(2, 4, ss.add("Hourly Rate ($)"), 3)
        << _sc(2, 5, ss.add("Hours Worked"), 3)
        << _sc(2, 6, ss.add("Shift Factor"), 3)
        << _sc(2, 7, ss.add("Calculated Wage ($)"), 3)
        << _rC();
    int pRow = 3;
    for (int i = 0; i < state.employees.size(); ++i) {
        const unique_ptr<Employee>& emp = state.employees[i];
        if (!emp) continue;
        double sf = (emp->getShiftSchedule() == "Night" || emp->getShiftSchedule() == "night") ? 1.2 : 1.0;
        payroll << _rO(pRow)
            << _sc(pRow, 1, ss.add(emp->getEmployeeID()), 0)
            << _sc(pRow, 2, ss.add(emp->getName()), 0)
            << _sc(pRow, 3, ss.add(emp->getRole()), 0);
        if (emp->getRole() == "Finance Manager") {
            payroll << _nc(pRow, 4, 0, 1)
                << _nc(pRow, 5, 0, 1)
                << _nc(pRow, 6, 1.0, 1)
                << _nc(pRow, 7, emp->getSalary(), 2);
        }
        else {
            payroll << _nc(pRow, 4, emp->getHourlyRate(), 1)
                << _nc(pRow, 5, static_cast<double>(emp->getHoursWorked()), 1)
                << _nc(pRow, 6, sf, 1)
                << _fc(pRow, 7,
                    "D" + to_string(pRow) +
                    "*E" + to_string(pRow) +
                    "*F" + to_string(pRow), 2);
        }
        payroll << _rC();
        pRow++;
    }
    int payrollTotalRow = pRow;
    payroll << _rO(pRow)
        << "<c r=\"F" << pRow << "\" t=\"s\" s=\"5\"><v>" << ss.add("TOTAL PAYROLL") << "</v></c>"
        << _fc(pRow, 7, (pRow > 3) ? "SUM(G3:G" + to_string(pRow - 1) + ")" : "0", 4)
        << _rC();
    payroll << _shC();

    ostringstream treasury;
    treasury << _shO(
        "<cols>"
        "<col min=\"1\" max=\"1\" width=\"32\" customWidth=\"1\"/>"
        "<col min=\"2\" max=\"2\" width=\"22\" customWidth=\"1\"/>"
        "</cols>"
    );
    treasury << _rO(1)
        << "<c r=\"A1\" t=\"s\" s=\"7\"><v>" << ss.add("CORPORATE TREASURY SUMMARY") << "</v></c>"
        << _rC();
    treasury << _rO(3)
        << "<c r=\"A3\" t=\"s\" s=\"5\"><v>" << ss.add("Capital Funds") << "</v></c>"
        << _nc(3, 2, state.availableCapital, 1)
        << _rC();
    treasury << _rO(4)
        << "<c r=\"A4\" t=\"s\" s=\"5\"><v>" << ss.add("Operating Expenses") << "</v></c>"
        << _nc(4, 2, state.totalOperatingExpenses, 1)
        << _rC();
    treasury << _rO(5)
        << "<c r=\"A5\" t=\"s\" s=\"5\"><v>" << ss.add("Total Sales Revenue") << "</v></c>"
        << _fc(5, 2, "'Sales Orders'!F" + to_string(salesTotalRow), 2)
        << _rC();
    treasury << _rO(6)
        << "<c r=\"A6\" t=\"s\" s=\"5\"><v>" << ss.add("Total Payroll Paid") << "</v></c>"
        << _fc(6, 2, "Payroll!G" + to_string(payrollTotalRow), 2)
        << _rC();
    treasury << _rO(8)
        << "<c r=\"A8\" t=\"s\" s=\"5\"><v>" << ss.add("Estimated Net Worth") << "</v></c>"
        << _fc(8, 2, "B3+B5-B4-B6", 6)
        << _rC();
    treasury << _rO(10)
        << "<c r=\"A10\" t=\"s\" s=\"5\"><v>" << ss.add("Liquidity Status") << "</v></c>"
        << "<c r=\"B10\" s=\"5\"><f>IF(B3&gt;B5*0.15,\"Healthy\",\"Low Capital\")</f></c>"
        << _rC();
    treasury << _shC();

    DynamicArray<_ZEntry> entries;
    _ZEntry e1; e1.name = "[Content_Types].xml"; e1.data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
        "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
        "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
        "<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>"
        "<Override PartName=\"/xl/worksheets/sheet1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"
        "<Override PartName=\"/xl/worksheets/sheet2.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"
        "<Override PartName=\"/xl/worksheets/sheet3.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"
        "<Override PartName=\"/xl/sharedStrings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\"/>"
        "<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>"
        "</Types>";
    entries.push_back(std::move(e1));

    _ZEntry e2; e2.name = "_rels/.rels"; e2.data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
        "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>"
        "</Relationships>";
    entries.push_back(std::move(e2));

    _ZEntry e3; e3.name = "xl/workbook.xml"; e3.data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\""
        " xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"
        "<sheets>"
        "<sheet name=\"Treasury Summary\" sheetId=\"1\" r:id=\"rId1\"/>"
        "<sheet name=\"Sales Orders\"     sheetId=\"2\" r:id=\"rId2\"/>"
        "<sheet name=\"Payroll\"          sheetId=\"3\" r:id=\"rId3\"/>"
        "</sheets>"
        "</workbook>";
    entries.push_back(std::move(e3));

    _ZEntry e4; e4.name = "xl/_rels/workbook.xml.rels"; e4.data =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
        "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\"     Target=\"worksheets/sheet1.xml\"/>"
        "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\"     Target=\"worksheets/sheet2.xml\"/>"
        "<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\"     Target=\"worksheets/sheet3.xml\"/>"
        "<Relationship Id=\"rId4\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>"
        "<Relationship Id=\"rId5\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\"        Target=\"styles.xml\"/>"
        "</Relationships>";
    entries.push_back(std::move(e4));

    _ZEntry e5; e5.name = "xl/worksheets/sheet1.xml"; e5.data = treasury.str(); entries.push_back(std::move(e5));
    _ZEntry e6; e6.name = "xl/worksheets/sheet2.xml"; e6.data = sales.str(); entries.push_back(std::move(e6));
    _ZEntry e7; e7.name = "xl/worksheets/sheet3.xml"; e7.data = payroll.str(); entries.push_back(std::move(e7));
    _ZEntry e8; e8.name = "xl/sharedStrings.xml"; e8.data = ss.xml(); entries.push_back(std::move(e8));
    _ZEntry e9; e9.name = "xl/styles.xml"; e9.data = _stylesXml(); entries.push_back(std::move(e9));

    DynamicArray<uint8_t> zip = _makeZip(entries);
    ofstream f("finance_management.xlsx", ios::binary);
    if (!f.is_open()) {
        cout << "[Error] Could not open finance_management.xlsx for writing." << endl;
        return;
    }
    for (int i = 0; i < zip.size(); ++i) {
        f.put(static_cast<char>(zip[i]));
    }
    f.close();

    cout << "[Excel Exported] finance_management.xlsx created successfully." << endl;
    cout << "  Sheet 1: Treasury Summary  (Capital, Net Worth, Liquidity check)" << endl;
    cout << "  Sheet 2: Sales Orders      (Per-order revenue, SUM total)" << endl;
    cout << "  Sheet 3: Payroll           (Wage = Rate x Hours x Shift factor)" << endl;
}
