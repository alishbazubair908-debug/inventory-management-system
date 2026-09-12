#include "Portals.h"

using namespace std;

void runAdminPortal(SystemState& state) {
    if (performAdminLogin()) {
        while (true) {
            cout << "\n=== ADMINISTRATOR CONTROL PANEL ===" << endl;
            cout << "1. Register Employee Account" << endl;
            cout << "2. Register Supplier Account" << endl;
            cout << "3. View All System Data (Warehouse & Directory)" << endl;
            cout << "4. Audit Depot & Asset Shortages" << endl;
            cout << "5. Authorize/Send Employee Payroll List" << endl;
            cout << "6. Generate System Audit Log PDF Report" << endl;
            cout << "7. Log Out Admin" << endl;
            cout << "8. Delete Storage Section (and Reassign Products)" << endl;
            cout << "9. Create New Storage Section" << endl;
            cout << "Choice: ";
            int admChoice = readInt();
            if (admChoice == 7) break;

            if (admChoice == 1) {
                cout << "Select Role (1. Warehouse Manager, 2. Stock Keeper, 3. Finance Manager): ";
                int roleSel = readInt();

                if (roleSel == 3) {
                    int currentFin = 0;
                    for (int i = 0; i < state.employees.size(); ++i) {
                        if (state.employees[i] && state.employees[i]->getRole() == "Finance Manager") {
                            currentFin++;
                        }
                    }
                    if (currentFin >= 1) {
                        cout << "Error: Only one active Finance Manager account is allowed in the system." << endl;
                        continue;
                    }
                }

                cout << "Enter Employee ID (AlphaNumeric): ";
                string eID = readValidID();
                while (isEmployeeIDDuplicate(state, eID)) {
                    cout << "Error: Employee ID already registered. Use a unique ID: ";
                    eID = readValidID();
                }

                cout << "Enter Name (Alphabets Only): ";
                string eName = readAlphaString();

                cout << "Enter CNIC (Format: XXXXX-XXXXXXX-X, e.g. 35201-1234567-1): ";
                string eCNIC = readCNIC();
                while (isCNICDuplicate(state, eCNIC)) {
                    cout << "Error: CNIC already registered in system. Enter a unique CNIC: ";
                    eCNIC = readCNIC();
                }

                cout << "Enter Date of Birth (YYYY-MM-DD): ";
                string dob = readDate();
                while (!validateDOBAge(dob)) {
                    cout << "\nError: Employee must be at least 20 years old relative to current year (2026).\n";
                    cout << "Re-enter Date of Birth (YYYY-MM-DD): ";
                    dob = readDate();
                }

                cout << "Enter Joining Date (YYYY-MM-DD): ";
                string joinDate = readDate();
                while (!validateAgeAtJoining(dob, joinDate)) {
                    cout << "\nError: Employee must be at least 20 years old at the time of joining." << endl;
                    cout << "Re-enter Joining Date (YYYY-MM-DD): ";
                    joinDate = readDate();
                }

                cout << "Enter Shift (Day/Night): ";
                string eShift = readAlphaString();

                double eSal = 0.0;
                if (roleSel == 1) {
                    cout << "Enter Monthly Base Salary ($): ";
                    eSal = readPositiveDouble();
                    cout << "Enter Employee Hourly Wage Rate ($): ";
                    double rate = readPositiveDouble();
                    state.employees.push_back(make_unique<WarehouseManager>(eID, eName, eCNIC, eSal, eShift, dob, joinDate, rate));
                    state.systemLogs.recordAction("Registered Manager: " + eName);
                    cout << "Manager account configured." << endl;
                    saveEmployeesCSV(state);
                }
                else if (roleSel == 2) {
                    cout << "Assign to Primary Storage Section (1-" << state.globalWarehouse.getStorageCount() << "): ";
                    int storageNum = readPositiveInt();
                    cout << "Enter Monthly Base Salary ($): ";
                    eSal = readPositiveDouble();
                    cout << "Enter Employee Hourly Wage Rate ($): ";
                    double rate = readPositiveDouble();
                    state.employees.push_back(make_unique<StockKeeper>(eID, eName, eCNIC, eSal, eShift, dob, joinDate, rate, storageNum));
                    state.systemLogs.recordAction("Registered StockKeeper: " + eName);
                    cout << "Stock Keeper account configured." << endl;
                    saveEmployeesCSV(state);
                }
                else if (roleSel == 3) {
                    cout << "Enter Monthly Base Salary ($): ";
                    eSal = readPositiveDouble();
                    cout << "Enter Employee Hourly Wage Rate ($15 - $150): ";
                    double rate = readDouble();
                    while (rate < 15.0 || rate > 150.0) {
                        cout << "Invalid range. Enter hourly rate between $15 and $150: ";
                        rate = readDouble();
                    }
                    state.employees.push_back(make_unique<FinanceManager>(eID, eName, eCNIC, eSal, eShift, dob, joinDate, rate));
                    state.systemLogs.recordAction("Registered Finance Manager: " + eName);
                    cout << "Finance Manager account configured." << endl;
                    saveEmployeesCSV(state);
                }
                else {
                    cout << "Invalid role choice." << endl;
                }
            }
            else if (admChoice == 2) {
                cout << "Enter Supplier ID (AlphaNumeric): ";
                string sID = readValidID();
                while (isSupplierIDDuplicate(state, sID)) {
                    cout << "Error: Supplier ID already registered. Use a unique ID: ";
                    sID = readValidID();
                }
                cout << "Enter Supplier Corporate Name (Alphabets Only): ";
                string sName = readAlphaString();
                cout << "Enter Contract Terms (e.g. Net 30): ";
                string sTerms = readString();
                state.suppliers.push_back(Supplier(sID, sName, sTerms));
                state.systemLogs.recordAction("Registered Supplier: " + sName);
                cout << "Supplier registered successfully!" << endl;
                saveSuppliersCSV(state);
            }
            else if (admChoice == 3) {
                cout << "\n--- GLOBAL WAREHOUSE CONFIGURATION ---" << endl;
                cout << state.globalWarehouse;
                cout << "\n--- REGISTERED STAFF DIRECTORY ---" << endl;
                if (state.employees.empty()) cout << "  No active employee accounts configured." << endl;
                for (int i = 0; i < state.employees.size(); ++i) {
                    if (state.employees[i]) state.employees[i]->generateReport();
                }
                cout << "\n--- REGISTERED SUPPLIER DIRECTORY ---" << endl;
                for (int i = 0; i < state.suppliers.size(); ++i) {
                    cout << "  * " << state.suppliers[i] << endl;
                }
                cout << "\n--- REGISTERED CUSTOMER DIRECTORY ---" << endl;
                if (state.customers.empty()) cout << "  No customer accounts registered." << endl;
                for (int i = 0; i < state.customers.size(); ++i) {
                    cout << "  * " << state.customers[i] << endl;
                }
                cout << "\n--- SYSTEM AUDIT LOGS ---" << endl;
                state.systemLogs.printAuditTrail();
            }
            else if (admChoice == 4) {
                state.globalWarehouse.findShortages();
            }
            else if (admChoice == 5) {
                state.payrollAuthorizedByAdmin = true;
                state.systemLogs.recordAction("Admin sent employee list to Treasury (Authorized next payroll).");
                cout << "SUCCESS: Employee list authorized and sent to Treasury." << endl;
            }
            else if (admChoice == 6) {
                generateAuditLogReport(state);
            }
            else if (admChoice == 8) {
                cout << "\n--- DECOMMISSION STORAGE SECTION ---" << endl;
                cout << "Enter Storage Number to delete: ";
                int delID = readInt();

                int delIdx = -1;
                for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
                    if (state.globalWarehouse.getStorage(i).getStorageNumber() == delID) {
                        delIdx = i; break;
                    }
                }

                if (delIdx == -1) {
                    cout << "Error: Storage section " << delID << " not found." << endl;
                }
                else {
                    Storage& sectionToDelete = state.globalWarehouse.getStorage(delIdx);
                    bool canDelete = true;

                    if (sectionToDelete.getProductCount() > 0) {
                        cout << "WARNING: Section contains " << sectionToDelete.getProductCount() << " items." << endl;
                        cout << "1. Move items to an existing section\n"
                            << "2. Create a new section and move items there\n"
                            << "3. Cancel deletion\nChoice: ";
                        int subChoice = readInt();

                        if (subChoice == 1 || subChoice == 2) {
                            int tIdx = -1;

                            if (subChoice == 2) {
                                cout << "--- CREATE NEW STORAGE SECTION (inline) ---" << endl;
                                cout << "New Storage Number: ";
                                int nID = readPositiveInt();
                                cout << "Capacity: ";
                                int nCap = readPositiveInt();
                                cout << "Category (Electronic, Clothing, Grocery, Any): ";
                                string nCat = readString();
                                state.globalWarehouse.addStorage(Storage(nID, nCap, nCat));
                                state.systemLogs.recordAction("Admin created inline Section: " + to_string(nID));
                                cout << "New Section " << nID << " created. Items will be moved there." << endl;
                                tIdx = state.globalWarehouse.getStorageCount() - 1;
                            }
                            else {
                                cout << "Enter Target Storage Number to move items to: ";
                                int tID = readInt();
                                for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
                                    if (state.globalWarehouse.getStorage(i).getStorageNumber() == tID) {
                                        tIdx = i; break;
                                    }
                                }
                            }

                            if (tIdx != -1 && tIdx != delIdx) {
                                Storage& target = state.globalWarehouse.getStorage(tIdx);
                                Storage& src = state.globalWarehouse.getStorage(delIdx);
                                while (src.getProductCount() > 0) {
                                    if (target.getProductCount() < target.getCapacity()) {
                                        unique_ptr<Product> p = src.takeProduct(0);
                                        if (!target.addStock(std::move(p))) {
                                            cout << "Category Mismatch! Cannot move all items to target section." << endl;
                                            canDelete = false; break;
                                        }
                                    }
                                    else {
                                        cout << "Target storage is FULL. Move failed." << endl;
                                        canDelete = false; break;
                                    }
                                }
                                if (canDelete) {
                                    cout << "All items shifted to Storage "
                                        << state.globalWarehouse.getStorage(tIdx).getStorageNumber()
                                        << " successfully." << endl;
                                }
                            }
                            else {
                                cout << "Invalid target section." << endl;
                                canDelete = false;
                            }
                        }
                        else {
                            canDelete = false;
                        }
                    }

                    if (canDelete) {
                        int finalDelIdx = -1;
                        for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
                            if (state.globalWarehouse.getStorage(i).getStorageNumber() == delID) {
                                finalDelIdx = i; break;
                            }
                        }
                        if (finalDelIdx != -1) {
                            state.globalWarehouse.removeStorageByIndex(finalDelIdx);
                            state.systemLogs.recordAction("Admin deleted Storage " + to_string(delID));
                            saveAllCSVFiles(state);
                            cout << "Storage Section " << delID << " has been removed." << endl;
                        }
                    }
                }
            }
            else if (admChoice == 9) {
                cout << "\n--- CREATE NEW STORAGE SECTION ---" << endl;
                cout << "New Storage Number: "; int nID = readPositiveInt();
                cout << "Capacity: "; int nCap = readPositiveInt();
                cout << "Category (Electronic, Clothing, Grocery, Any): ";
                string nCat = readString();
                state.globalWarehouse.addStorage(Storage(nID, nCap, nCat));
                state.systemLogs.recordAction("Admin created new Section: " + to_string(nID));
                saveAllCSVFiles(state);
                cout << "SUCCESS: New section is now active." << endl;
            }
        }
    }
}

void runEmployeePortal(SystemState& state) {
    cout << "Enter Employee ID: ";
    string eID = readString();
    Employee* activeEmp = nullptr;
    for (int i = 0; i < state.employees.size(); ++i) {
        if (state.employees[i] && state.employees[i]->getEmployeeID() == eID) {
            activeEmp = state.employees[i].get();
            break;
        }
    }
    if (!activeEmp) {
        cout << "No matching ID found. Admin registration required.\n" << endl;
        return;
    }

    WarehouseManager* manager = dynamic_cast<WarehouseManager*>(activeEmp);
    StockKeeper* stockKeeper = dynamic_cast<StockKeeper*>(activeEmp);
    FinanceManager* finance = dynamic_cast<FinanceManager*>(activeEmp);

    if (manager) {
        while (true) {
            cout << "\n=== WAREHOUSE MANAGER PORTAL ===" << endl;
            cout << "1. Assign Routine Task" << endl;
            cout << "2. Audit Depot & Asset Shortages / Generate Reports" << endl;
            cout << "3. Request Product Restock (via Supplier)" << endl;
            cout << "4. View Global System Logs" << endl;
            cout << "5. View My Profile Info" << endl;
            cout << "6. Logout" << endl;
            cout << "Choice: ";
            int mgrChoice = readInt();
            if (mgrChoice == 6) break;

            if (mgrChoice == 1) {
                manager->assignTask();
            }
            else if (mgrChoice == 2) {
                manager->auditWarehouse(state.globalWarehouse);
                generateWarehouseReport(state);
            }
            else if (mgrChoice == 3) {
                cout << "Enter the ID of the product to restock: ";
                string pID = readString();
                Product* p = state.globalWarehouse.findProductByID(pID);
                if (!p) {
                    cout << "Product not found." << endl;
                    continue;
                }
                cout << "Enter quantity to request: ";
                int reqQty = readPositiveInt();
                if (state.suppliers.empty()) {
                    cout << "No suppliers registered. Ask Admin to add a supplier first." << endl;
                    continue;
                }
                cout << "Available Suppliers:\n";
                for (int i = 0; i < state.suppliers.size(); ++i) {
                    cout << "  [" << i + 1 << "] ID: " << state.suppliers[i].getSupplierID()
                        << " Name: " << state.suppliers[i].getSupplierName() << endl;
                }
                cout << "Select Supplier Index: ";
                int supIdx = readInt();
                if (supIdx < 1 || supIdx > state.suppliers.size()) {
                    cout << "Invalid index." << endl;
                    continue;
                }
                RestockRequest req;
                ostringstream reqID;
                reqID << "ORD-" << state.restockRequests.size() + 100;
                req.orderID = reqID.str();
                req.supplierID = state.suppliers[supIdx - 1].getSupplierID();
                req.productID = pID;
                req.quantityRequested = reqQty;
                req.isFulfilled = false;
                state.restockRequests.push_back(req);
                state.systemLogs.recordAction("Restock request " + req.orderID + " created by Manager " + manager->getName() + ".");
                cout << "SUCCESS: Restock request " << req.orderID << " submitted to supplier." << endl;
                saveRestockRequestsCSV(state);
            }
            else if (mgrChoice == 4) {
                state.systemLogs.printAuditTrail();
            }
            else if (mgrChoice == 5) {
                manager->generateReport();
            }
        }
    }
    else if (stockKeeper) {
        while (true) {
            cout << "\n=== STOCK KEEPER PORTAL ===" << endl;
            cout << "1. Register/Record New Product Data" << endl;
            cout << "2. Perform Expiry Safety Checks on Storage / Generate Report" << endl;
            cout << "3. Flag Expired Hazards" << endl;
            cout << "4. Manually Adjust Stock (Storage restock)" << endl;
            cout << "5. Apply Discount Policy to Product" << endl;
            cout << "6. View My Profile Info" << endl;
            cout << "7. Logout" << endl;
            cout << "Choice: ";
            int skChoice = readInt();
            if (skChoice == 7) break;

            if (skChoice == 1) {
                int storageCount = state.globalWarehouse.getStorageCount();
                cout << "Select Target Storage (1-" << storageCount << "):\n";
                for (int i = 0; i < storageCount; ++i) {
                    const Storage& s = state.globalWarehouse.getStorage(i);
                    cout << "  " << (i + 1) << ". Storage " << s.getStorageNumber()
                        << " [Category: " << s.getAllowedCategory() << "] ("
                        << s.getProductCount() << "/" << s.getCapacity() << " Occupied)\n";
                }
                cout << "Choice: ";
                int destStorage = readInt();
                if (destStorage < 1 || destStorage > storageCount) {
                    cout << "Storage section does not exist." << endl;
                    continue;
                }
                Storage& activeSection = state.globalWarehouse.getStorage(destStorage - 1);
                string allowedCat = activeSection.getAllowedCategory();

                if (allowedCat != "Any") {
                    cout << "\nStorage " << activeSection.getStorageNumber()
                        << " only accepts " << allowedCat << " products.\n" << endl;
                }

                DynamicArray<int> optionMap;

                cout << "Select Category:\n";
                int displayIdx = 1;

                if (allowedCat == "Electronic" || allowedCat == "Any") {
                    cout << "  " << displayIdx++ << ". Electronic\n";
                    optionMap.push_back(1);
                    cout << "  " << displayIdx++ << ". Fragile Electronic\n";
                    optionMap.push_back(2);
                }
                if (allowedCat == "Grocery" || allowedCat == "Any") {
                    cout << "  " << displayIdx++ << ". Grocery (Standard)\n";
                    optionMap.push_back(3);
                    cout << "  " << displayIdx++ << ". Perishable Grocery\n";
                    optionMap.push_back(4);
                    cout << "  " << displayIdx++ << ". Non-Perishable Grocery\n";
                    optionMap.push_back(5);
                    cout << "  " << displayIdx++ << ". Veg/Fruit (Sold by Weight)\n";
                    optionMap.push_back(7);
                }
                if (allowedCat == "Clothing" || allowedCat == "Any") {
                    cout << "  " << displayIdx++ << ". Clothing\n";
                    optionMap.push_back(6);
                }

                cout << "Choice: ";
                int displayChoice = readInt();

                if (displayChoice < 1 || displayChoice > optionMap.size()) {
                    cout << "Invalid category selection." << endl;
                    continue;
                }

                int cat = optionMap[displayChoice - 1];

                cout << "Enter Product ID: ";
                string pID = readValidID();
                while (isProductIDDuplicate(state, pID)) {
                    cout << "Error: Product ID already registered. Use a unique ID: ";
                    pID = readValidID();
                }

                cout << "Enter Product Name: ";
                string pName = readString();
                cout << "Enter Retail Price ($): ";
                double pPrice = readPositiveDouble();
                cout << "Enter Starting Stock Quantity: ";
                int pQty = readPositiveInt();
                cout << "Enter Barcode Value: ";
                string pBarVal = readString();
                cout << "Enter Barcode format (UPC/EAN13): ";
                string pBarFormat = readString();
                Barcode newBarcode(pBarVal, pBarFormat);

                unique_ptr<Product> newProd = nullptr;

                if (cat == 1) {
                    cout << "Enter Voltage (V): ";
                    int volt = readPositiveInt();
                    cout << "Warranty (Months): ";
                    int war = readInt();
                    while (war < 0) {
                        cout << "Invalid. Warranty must be 0 or more months: ";
                        war = readInt();
                    }
                    cout << "Brand Name (Alphabets Only): ";
                    string brand = readAlphaString();
                    newProd = make_unique<ElectronicProduct>(pID, pName, pPrice, pQty, newBarcode, volt, war, brand);
                }
                else if (cat == 2) {
                    cout << "Enter Voltage (V): ";
                    int volt = readPositiveInt();
                    cout << "Warranty (Months): ";
                    int war = readInt();
                    while (war < 0) {
                        cout << "Invalid. Warranty must be 0 or more months: ";
                        war = readInt();
                    }
                    cout << "Brand Name (Alphabets Only): ";
                    string brand = readAlphaString();
                    cout << "Fragility rating (1-10): ";
                    int frag = readInt();
                    while (frag < 1 || frag > 10) {
                        cout << "Invalid. Fragility rating must be between 1 and 10: ";
                        frag = readInt();
                    }
                    cout << "Packaging Medium: ";
                    string pack = readString();
                    newProd = make_unique<FragileElectronics>(pID, pName, pPrice, pQty, newBarcode, volt, war, brand, frag, pack);
                }
                else if (cat == 3) {
                    cout << "Enter Calories: ";
                    int cal = readInt();
                    while (cal < 0) {
                        cout << "Invalid. Calories must be 0 or more: ";
                        cal = readInt();
                    }
                    cout << "Is Halal? (1 = Yes, 0 = No): ";
                    int halalVal = readInt();
                    cout << "Country of Origin (Alphabets Only): ";
                    string origin = readAlphaString();
                    newProd = make_unique<GroceryProduct>(pID, pName, pPrice, pQty, newBarcode, cal, (halalVal == 1), origin);
                }
                else if (cat == 4) {
                    cout << "Enter Calories: ";
                    int cal = readInt();
                    while (cal < 0) {
                        cout << "Invalid. Calories must be 0 or more: ";
                        cal = readInt();
                    }
                    cout << "Is Halal? (1 = Yes, 0 = No): ";
                    int halalVal = readInt();
                    cout << "Country of Origin (Alphabets Only): ";
                    string origin = readAlphaString();
                    cout << "Expiration Date (YYYY-MM-DD): ";
                    string exp = readDate();
                    cout << "Optimal Storage Temp (C): ";
                    double temp = readDouble();
                    newProd = make_unique<PerishableGrocery>(pID, pName, pPrice, pQty, newBarcode, cal, (halalVal == 1), origin, exp, temp);
                }
                else if (cat == 5) {
                    cout << "Enter Calories: ";
                    int cal = readInt();
                    while (cal < 0) {
                        cout << "Invalid. Calories must be 0 or more: ";
                        cal = readInt();
                    }
                    cout << "Is Halal? (1 = Yes, 0 = No): ";
                    int halalVal = readInt();
                    cout << "Country of Origin (Alphabets Only): ";
                    string origin = readAlphaString();
                    cout << "Shelf Life (Years): ";
                    int shelf = readPositiveInt();
                    cout << "Preservation Level: ";
                    string pres = readString();
                    newProd = make_unique<NonPerishable>(pID, pName, pPrice, pQty, newBarcode, cal, (halalVal == 1), origin, shelf, pres);
                }
                else if (cat == 6) {
                    cout << "Size (S/M/L/XL): ";
                    string sz = readString();
                    cout << "Fabric: ";
                    string fab = readString();
                    cout << "Target Gender (Alphabets Only): ";
                    string gen = readAlphaString();
                    newProd = make_unique<ClothingProduct>(pID, pName, pPrice, pQty, newBarcode, sz, fab, gen);
                }
                else if (cat == 7) {
                    cout << "Enter Calories (per 100g): ";
                    int cal = readInt();
                    while (cal < 0) {
                        cout << "Invalid. Calories must be 0 or more: ";
                        cal = readInt();
                    }
                    cout << "Is Halal? (1 = Yes, 0 = No): ";
                    int halalVal = readInt();
                    cout << "Country of Origin (Alphabets Only): ";
                    string origin = readAlphaString();
                    cout << "Enter Price per Kilogram ($): ";
                    pPrice = readPositiveDouble();
                    cout << "Enter Starting Stock in Kilograms (e.g. 20.0): ";
                    double startingKg = readPositiveDouble();
                    pQty = static_cast<int>(startingKg * 1000.0);
                    newProd = make_unique<VegFruitGrocery>(pID, pName, pPrice, pQty, newBarcode, cal, (halalVal == 1), origin);
                }

                if (newProd) {
                    cout << "Do you want to add an immediate seasonal discount to this product? (1 = Yes, 0 = No): ";
                    int addDisc = readInt();
                    if (addDisc == 1) {
                        cout << "Enter Policy Name (Alphabets Only): ";
                        string polName = readAlphaString();
                        string startDate, endDate;
                        string today = getCurrentDateTime().substr(0, 10);
                        while (true) {
                            cout << "Enter Start Date (YYYY-MM-DD) [Today or future]: ";
                            startDate = readDate();
                            if (!isDateBeforeOrEqual(today, startDate)) {
                                cout << "Error: Start date cannot be in the past (Today is " << today << "). Please re-enter.\n";
                                continue;
                            }
                            break;
                        }
                        while (true) {
                            cout << "Enter End Date (YYYY-MM-DD) [Must be >= Start Date]: ";
                            endDate = readDate();
                            if (!isDateBeforeOrEqual(startDate, endDate)) {
                                cout << "Error: End date cannot be before start date (" << startDate << "). Please re-enter.\n";
                                continue;
                            }
                            break;
                        }
                        cout << "Enter Season Name (e.g., Summer): ";
                        string sName = readAlphaString();
                        cout << "Enter Discount Rate (e.g. 0.15 for 15%): ";
                        double rate = readPositiveDouble();
                        SeasonalDiscount policy(polName, startDate, endDate, sName, rate);
                        newProd->applyDiscount(policy);
                    }

                    string serializedRecord = newProd->serialize();
                    if (activeSection.addStock(std::move(newProd))) {
                        state.systemLogs.recordAction(stockKeeper->getName() + " registered Product: " + pID);
                        cout << "SUCCESS: Product cataloged under Storage " << destStorage << "." << endl;
                        appendProductRecord("products.csv", serializedRecord);
                    }
                    else {
                        cout << "Placement failed due to storage category restrictions." << endl;
                    }
                }
                else {
                    cout << "Failed to allocate product." << endl;
                }
            }
            else if (skChoice == 2) {
                for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
                    stockKeeper->performExpiryCheck(state.globalWarehouse.getStorage(i));
                }
                generateExpiryReport(state);
            }
            else if (skChoice == 3) {
                cout << "Enter Perishable Product ID to check safety: ";
                string targetID = readString();
                Product* p = state.globalWarehouse.findProductByID(targetID);
                PerishableGrocery* pg = dynamic_cast<PerishableGrocery*>(p);
                if (pg) {
                    stockKeeper->flagExpiredItem(*pg);
                }
                else {
                    cout << "Product is not classifiable as Perishable Grocery." << endl;
                }
            }
            else if (skChoice == 4) {
                cout << "Enter Product ID: ";
                string targetID = readString();
                Product* p = state.globalWarehouse.findProductByID(targetID);
                if (p) {
                    cout << "Current quantity: " << p->getQuantity() << ". Enter restocking units: ";
                    int restockUnits = readPositiveInt();
                    stockKeeper->restockItem(p, restockUnits);
                    saveProductsCSV(state);
                }
                else {
                    cout << "Product not found." << endl;
                }
            }
            else if (skChoice == 5) {
                cout << "Enter Product ID to apply discount: ";
                string targetID = readString();
                Product* p = state.globalWarehouse.findProductByID(targetID);
                if (!p) {
                    cout << "Product not found." << endl;
                    continue;
                }
                cout << "Select Discount Type:\n  1. Bulk Discount\n  2. Seasonal Discount\nChoice: ";
                int discType = readInt();
                cout << "Enter Policy Name (Alphabets Only): ";
                string polName = readAlphaString();
                string startDate, endDate;
                string today = getCurrentDateTime().substr(0, 10);
                while (true) {
                    cout << "Enter Start Date (YYYY-MM-DD) [Today or future]: ";
                    startDate = readDate();
                    if (!isDateBeforeOrEqual(today, startDate)) {
                        cout << "Error: Start date cannot be in the past (Today is " << today << "). Please re-enter.\n";
                        continue;
                    }
                    break;
                }
                while (true) {
                    cout << "Enter End Date (YYYY-MM-DD) [Must be >= Start Date]: ";
                    endDate = readDate();
                    if (!isDateBeforeOrEqual(startDate, endDate)) {
                        cout << "Error: End date cannot be before start date (" << startDate << "). Please re-enter.\n";
                        continue;
                    }
                    break;
                }
                if (discType == 1) {
                    cout << "Enter Minimum Quantity for Bulk Discount: ";
                    int minQty = readPositiveInt();
                    cout << "Enter Discount Percentage (e.g., 15.0 for 15%): ";
                    double discPct = readPositiveDouble();
                    BulkDiscount policy(polName, startDate, endDate, minQty, discPct);
                    p->applyDiscount(policy);
                    cout << "Bulk Discount applied. Updated Product Details:\n";
                    p->displayStatus();
                }
                else if (discType == 2) {
                    cout << "Enter Season Name (e.g., Summer): ";
                    string sName = readAlphaString();
                    cout << "Enter Discount Rate (e.g., 0.10 for 10%): ";
                    double rate = readPositiveDouble();
                    SeasonalDiscount policy(polName, startDate, endDate, sName, rate);
                    p->applyDiscount(policy);
                    cout << "Seasonal Discount applied. Updated Product Details:\n";
                    p->displayStatus();
                }
                else {
                    cout << "Invalid Discount type selection." << endl;
                }
            }
            else if (skChoice == 6) {
                stockKeeper->generateReport();
            }
        }
    }
    else if (finance) {
        while (true) {
            cout << "\n=== CORPORATE FINANCE MANAGER PORTAL ===" << endl;
            cout << "1. Run Treasury & Inventory Money health Audit" << endl;
            cout << "2. Record Operational Expense transaction" << endl;
            cout << "3. Calculate & Pay Monthly Employee Wages (Interactive)" << endl;
            cout << "4. Process Pending Customer Orders" << endl;
            cout << "5. Process Warranty Claim Tickets (Replace / Refund)" << endl;
            cout << "6. Export Smart Finance Excel Worksheet (CSV)" << endl;
            cout << "7. View My Profile Info" << endl;
            cout << "8. Logout" << endl;
            cout << "Choice: ";
            int finChoice = readInt();
            if (finChoice == 8) break;

            if (finChoice == 1) {
                finance->auditTreasury(state.globalWarehouse, state.availableCapital, state.totalOperatingExpenses);
            }
            else if (finChoice == 2) {
                cout << "Enter expense transaction amount ($): ";
                double amt = readPositiveDouble();
                finance->addOperationalExpense(amt, state.availableCapital, state.totalOperatingExpenses);
                state.systemLogs.recordAction("Logged operating expense transaction of $" + to_string(amt));
                saveAllCSVFiles(state);
            }
            else if (finChoice == 3) {
                if (!state.payrollAuthorizedByAdmin) {
                    cout << "\nError: Payroll has already been processed for this cycle.\n"
                        << "Wages cannot be calculated until the Administrator sends the updated employee list." << endl;
                    continue;
                }
                cout << "\n--- AUDITING CORPORATE PAYROLL (Interactive) ---" << endl;
                double totalPayroll = 0.0;
                if (state.employees.empty()) {
                    cout << "  No active employees registered to pay." << endl;
                }
                else {
                    for (int i = 0; i < state.employees.size(); ++i) {
                        const unique_ptr<Employee>& emp = state.employees[i];
                        if (!emp) continue;
                        double wage = 0.0;
                        if (emp.get() == finance) {
                            wage = emp->calculateMonthlyWage();
                            totalPayroll += wage;
                            cout << "  * Employee: " << emp->getName()
                                << " (ID: " << emp->getEmployeeID() << " | " << emp->getRole() << ")\n"
                                << "    -> Paid standard monthly flat contract salary: $" << wage << endl;
                        }
                        else {
                            cout << "  * Enter hours worked this month for " << emp->getName()
                                << " (ID: " << emp->getEmployeeID() << " | " << emp->getRole() << "): ";
                            int hrs = readInt();
                            while (hrs < 0 || hrs > 400) {
                                cout << "Invalid hours. Enter a value between 0 and 400: ";
                                hrs = readInt();
                            }
                            emp->setHoursWorked(hrs);
                            wage = emp->calculateMonthlyWage();
                            totalPayroll += wage;
                            cout << "    -> Calculated Wage: $" << wage << " (" << hrs
                                << " hrs @ $" << emp->getHourlyRate() << "/hr | Shift: " << emp->getShiftSchedule() << ")\n";
                        }
                    }
                    cout << "\nTotal Gross Payroll Expenses: $" << totalPayroll << endl;
                    if (state.availableCapital >= totalPayroll) {
                        state.availableCapital -= totalPayroll;
                        state.totalOperatingExpenses += totalPayroll;
                        cout << "SUCCESS: Paid monthly salaries. Capital updated successfully." << endl;
                        state.systemLogs.recordAction("Processed payroll transaction of $" + to_string(totalPayroll));
                        state.payrollAuthorizedByAdmin = false;
                        generatePayrollReport(state);
                        saveAllCSVFiles(state);
                    }
                    else {
                        cout << "ERROR: Insufficient available capital to process payroll!" << endl;
                    }
                }
            }
            else if (finChoice == 4) {
                cout << "\n--- PENDING CUSTOMER ORDERS ---" << endl;
                bool found = false;
                for (int i = 0; i < state.pendingOrders.size(); ++i) {
                    const CustomerOrder& ord = state.pendingOrders[i];
                    if (!ord.isApproved && !ord.isRejected) {
                        cout << "  * Order ID: " << ord.orderID
                            << " | Customer ID: " << ord.customerID
                            << " | Product ID: " << ord.productID
                            << " | Qty: " << ord.quantity
                            << " | Total Price: $" << ord.totalPrice << endl;
                        found = true;
                    }
                }
                if (!found) {
                    cout << "  No pending customer orders found." << endl;
                    continue;
                }
                cout << "Enter Order ID to process: ";
                string targetOrdID = readString();
                CustomerOrder* selectedOrd = nullptr;
                for (int i = 0; i < state.pendingOrders.size(); ++i) {
                    if (state.pendingOrders[i].orderID == targetOrdID && !state.pendingOrders[i].isApproved && !state.pendingOrders[i].isRejected) {
                        selectedOrd = &state.pendingOrders[i];
                        break;
                    }
                }
                if (!selectedOrd) {
                    cout << "Invalid or already processed Order ID." << endl;
                    continue;
                }
                cout << "Select Action (1 to Approve, 2 to Reject): ";
                int action = readInt();
                if (action == 1) {
                    Product* p = state.globalWarehouse.findProductByID(selectedOrd->productID);
                    if (p && p->getQuantity() >= selectedOrd->quantity) {
                        p->setQuantity(p->getQuantity() - selectedOrd->quantity);
                        state.availableCapital += selectedOrd->totalPrice;
                        string currentCustName = "Guest";
                        for (int i = 0; i < state.customers.size(); ++i) {
                            if (state.customers[i].getCustomerID() == selectedOrd->customerID) {
                                PurchasedItem item;
                                item.productID = selectedOrd->productID;
                                item.productName = p->getProductName();
                                item.quantity = selectedOrd->quantity;
                                item.pricePaid = selectedOrd->totalPrice;
                                item.purchaseDate = getCurrentDateTime().substr(0, 10);
                                item.warrantyMonths = p->getWarrantyMonths();
                                state.customers[i].addPurchasedItem(item);
                                currentCustName = state.customers[i].getName();
                                break;
                            }
                        }
                        selectedOrd->isApproved = true;
                        state.systemLogs.recordAction("Finance Manager approved customer order " + selectedOrd->orderID);
                        cout << "SUCCESS: Order approved. Stock deducted, corporate funds updated, items granted." << endl;
                        generateCustomerReceipt(*selectedOrd, currentCustName);
                        saveOrdersCSV(state);
                        saveProductsCSV(state);
                        saveCustomersCSV(state);
                        generateOrdersReport(state);
                    }
                    else {
                        cout << "Failed: Insufficient warehouse stock to fulfill this order. Auto-rejecting." << endl;
                        selectedOrd->isRejected = true;
                        saveOrdersCSV(state);
                    }
                }
                else if (action == 2) {
                    selectedOrd->isRejected = true;
                    state.systemLogs.recordAction("Finance Manager rejected customer order " + selectedOrd->orderID);
                    cout << "Order successfully rejected." << endl;
                    saveOrdersCSV(state);
                }
            }
            else if (finChoice == 5) {
                cout << "\n--- WARRANTY CLAIMS QUEUE ---" << endl;
                bool found = false;
                for (int i = 0; i < state.warrantyClaims.size(); ++i) {
                    const WarrantyClaim& cl = state.warrantyClaims[i];
                    if (!cl.isProcessed) {
                        cout << "  * Claim ID: " << cl.claimID
                            << " | Customer ID: " << cl.customerID
                            << " | Product ID: " << cl.productID
                            << " | Type: " << cl.claimType
                            << " | Refund Value: $" << cl.refundAmount << endl;
                        found = true;
                    }
                }
                if (!found) {
                    cout << "  No pending warranty claims found." << endl;
                    continue;
                }
                cout << "Enter Claim ID to process: ";
                string targetClaimID = readString();
                WarrantyClaim* selectedCl = nullptr;
                for (int i = 0; i < state.warrantyClaims.size(); ++i) {
                    if (state.warrantyClaims[i].claimID == targetClaimID && !state.warrantyClaims[i].isProcessed) {
                        selectedCl = &state.warrantyClaims[i];
                        break;
                    }
                }
                if (!selectedCl) {
                    cout << "Invalid or already processed Claim ID." << endl;
                    continue;
                }
                if (selectedCl->claimType == "Replacement") {
                    Product* p = state.globalWarehouse.findProductByID(selectedCl->productID);
                    if (p && p->getQuantity() >= 1) {
                        p->setQuantity(p->getQuantity() - 1);
                        selectedCl->isProcessed = true;
                        state.systemLogs.recordAction("Approved warranty claim replacement " + selectedCl->claimID);
                        cout << "SUCCESS: Replacement item approved and deducted from warehouse stock." << endl;
                        saveWarrantyClaimsCSV(state);
                        saveProductsCSV(state);
                        generateOrdersReport(state);
                    }
                    else {
                        cout << "Failed: Product is out of stock. Suggest a refund instead." << endl;
                    }
                }
                else if (selectedCl->claimType == "Refund") {
                    if (state.availableCapital >= selectedCl->refundAmount) {
                        state.availableCapital -= selectedCl->refundAmount;
                        state.totalOperatingExpenses += selectedCl->refundAmount;
                        selectedCl->isProcessed = true;
                        state.systemLogs.recordAction("Approved warranty claim refund " + selectedCl->claimID);
                        cout << "SUCCESS: Refund approved. $" << selectedCl->refundAmount << " paid from capital." << endl;
                        saveWarrantyClaimsCSV(state);
                        generateOrdersReport(state);
                    }
                    else {
                        cout << "Failed: Insufficient corporate capital to issue refund." << endl;
                    }
                }
            }
            else if (finChoice == 6) {
                generateFinanceExcel(state);
            }
            else if (finChoice == 7) {
                finance->generateReport();
            }
        }
    }
}

void runCustomerPortal(SystemState& state) {
    cout << "=== CLIENT PORTAL ===" << endl;
    cout << "1. Sign In (Registered Customer)" << endl;
    cout << "2. Sign Up (New Registration)" << endl;
    cout << "Choice: ";
    int custPortalSel = readInt();

    Customer* activeCust = nullptr;
    if (custPortalSel == 1) {
        cout << "Enter Customer ID: ";
        string cID = readString();
        for (int i = 0; i < state.customers.size(); ++i) {
            if (state.customers[i].getCustomerID() == cID) {
                activeCust = &state.customers[i];
                break;
            }
        }
        if (!activeCust) {
            cout << "ID not verified in staff system directories.\n" << endl;
            return;
        }
    }
    else if (custPortalSel == 2) {
        cout << "Enter Unique Customer ID: ";
        string cID = readValidID();
        while (isCustomerIDDuplicate(state, cID)) {
            cout << "Error: Customer ID already registered. Use a unique ID: ";
            cID = readValidID();
        }
        cout << "Enter Your Name (Alphabets Only): ";
        string cName = readAlphaString();
        string cContact = readContactInfo();
        state.customers.push_back(Customer(cID, cName, cContact));
        activeCust = &state.customers.back();
        cout << "Registration successful! Welcome " << cName << "!\n";
        saveCustomersCSV(state);
    }
    else {
        return;
    }

    while (true) {
        cout << "\n=== WELCOME TO THE SHOPPING MENU ===" << endl;
        cout << "1. Browse Store Products" << endl;
        cout << "2. Purchase Item (Submits pending order)" << endl;
        cout << "3. View My Purchases Trail" << endl;
        cout << "4. File Warranty Claim (Replace or Refund)" << endl;
        cout << "5. Logout" << endl;
        cout << "Choice: ";
        int shopChoice = readInt();
        if (shopChoice == 5) break;

        if (shopChoice == 1) {
            cout << "\n--- BROWSE CATALOG ---" << endl;
            cout << state.globalWarehouse;
        }
        else if (shopChoice == 2) {
            cout << "Enter the Product ID you wish to purchase: ";
            string targetID = readString();
            Product* item = state.globalWarehouse.findProductByID(targetID);

            if (!item) {
                cout << "Error: Product ID does not exist in store." << endl;
            }
            else {
                if (item->getSubtype() == "VegFruit") {
                    cout << "This Veg/Fruit item is sold by weight ($" << item->getPrice() << "/kg).\n";
                    cout << "Select purchase unit:\n  1. Kilograms (kg)\n  2. Grams (g)\nChoice: ";
                    int unitSel = readInt();
                    double targetGrams = 0.0;
                    double calculatedCost = 0.0;
                    if (unitSel == 1) {
                        cout << "Enter weight in Kilograms (e.g. 1.5): ";
                        double kgVal = readPositiveDouble();
                        targetGrams = kgVal * 1000.0;
                        calculatedCost = item->getPrice() * kgVal;
                    }
                    else {
                        cout << "Enter weight in Grams (e.g. 450): ";
                        double gVal = readPositiveDouble();
                        targetGrams = gVal;
                        calculatedCost = (item->getPrice() / 1000.0) * gVal;
                    }
                    int orderQtyGrams = static_cast<int>(targetGrams);
                    if (item->getQuantity() >= orderQtyGrams) {
                        CustomerOrder ord;
                        ostringstream ordID;
                        ordID << "CUST-ORD-" << state.pendingOrders.size() + 100;
                        ord.orderID = ordID.str();
                        ord.customerID = activeCust->getCustomerID();
                        ord.productID = targetID;
                        ord.quantity = orderQtyGrams;
                        ord.totalPrice = calculatedCost;
                        ord.isApproved = false;
                        ord.isRejected = false;
                        state.pendingOrders.push_back(ord);
                        state.systemLogs.recordAction("Customer " + activeCust->getCustomerName() + " submitted pending Veg/Fruit order " + ord.orderID);
                        cout << "[Order Received] Order " << ord.orderID << " submitted successfully.\n"
                            << "  -> Weight requested: " << (targetGrams / 1000.0) << " kg | Total: $" << calculatedCost << endl;
                        saveOrdersCSV(state);
                    }
                    else {
                        double currentKgAvailable = static_cast<double>(item->getQuantity()) / 1000.0;
                        cout << "[Failed] Insufficient stock. Only " << currentKgAvailable << " kg available." << endl;
                    }
                }
                else {
                    cout << "Product: " << item->getProductName() << " | Price: $" << item->getPrice() << endl;
                    cout << "Enter quantity: ";
                    int orderQty = readPositiveInt();
                    if (item->getQuantity() >= orderQty) {
                        CustomerOrder ord;
                        ostringstream ordID;
                        ordID << "CUST-ORD-" << state.pendingOrders.size() + 100;
                        ord.orderID = ordID.str();
                        ord.customerID = activeCust->getCustomerID();
                        ord.productID = targetID;
                        ord.quantity = orderQty;
                        ord.totalPrice = item->getPrice() * orderQty;
                        ord.isApproved = false;
                        ord.isRejected = false;
                        state.pendingOrders.push_back(ord);
                        state.systemLogs.recordAction("Customer " + activeCust->getCustomerName() + " submitted pending order " + ord.orderID);
                        cout << "[Order Received] Order " << ord.orderID << " submitted successfully.\n"
                            << "  -> Awaiting Finance Manager approval. Stock is not yet deducted." << endl;
                        saveOrdersCSV(state);
                    }
                    else {
                        cout << "[Failed] Insufficient stock. Only " << item->getQuantity() << " available." << endl;
                    }
                }
            }
        }
        else if (shopChoice == 3) {
            activeCust->viewPurchaseHistory();
            cout << "Total Spent: $" << activeCust->getTotalSpent() << endl;
        }
        else if (shopChoice == 4) {
            cout << "\n--- FILE WARRANTY CLAIM ---" << endl;
            const DynamicArray<PurchasedItem>& purchases = activeCust->getPurchasedItems();
            if (purchases.empty()) {
                cout << "You have no approved purchases eligible for warranty claims." << endl;
                continue;
            }
            cout << "Eligible Purchases:\n";
            for (int i = 0; i < purchases.size(); ++i) {
                cout << "  [" << i + 1 << "] " << purchases[i].productName
                    << " (ID: " << purchases[i].productID
                    << ") | Purchased: " << purchases[i].purchaseDate
                    << " | Warranty: " << purchases[i].warrantyMonths << " Months" << endl;
            }
            cout << "Select item to claim (Index): ";
            int idx = readInt();
            if (idx < 1 || idx > purchases.size()) {
                cout << "Invalid index selection." << endl;
                continue;
            }
            const PurchasedItem& selected = purchases[idx - 1];
            if (selected.warrantyMonths == 0) {
                cout << "This product does not have an active warranty." << endl;
                continue;
            }
            string today = getCurrentDateTime().substr(0, 10);
            string warrantyEndDate = addMonthsToDate(selected.purchaseDate, selected.warrantyMonths);
            if (today > warrantyEndDate) {
                cout << "Warranty expired. (Valid until: " << warrantyEndDate << " | Today: " << today << ")" << endl;
                continue;
            }
            cout << "Warranty is VALID until: " << warrantyEndDate << endl;
            cout << "Select Option:\n  1. Replacement product\n  2. Refund (Money Back)\nChoice: ";
            int claimSel = readInt();
            if (claimSel != 1 && claimSel != 2) {
                cout << "Invalid selection." << endl;
                continue;
            }
            WarrantyClaim claim;
            ostringstream claimID;
            claimID << "CLAIM-" << state.warrantyClaims.size() + 100;
            claim.claimID = claimID.str();
            claim.customerID = activeCust->getCustomerID();
            claim.productID = selected.productID;
            claim.claimType = (claimSel == 1) ? "Replacement" : "Refund";
            claim.refundAmount = selected.pricePaid;
            claim.isProcessed = false;
            state.warrantyClaims.push_back(claim);
            state.systemLogs.recordAction("Customer " + activeCust->getCustomerName() + " filed warranty claim " + claim.claimID);
            cout << "SUCCESS: Warranty claim " << claim.claimID << " submitted. Awaiting Finance Manager review." << endl;
            saveWarrantyClaimsCSV(state);
        }
    }
}

void runSupplierPortal(SystemState& state) {
    cout << "Enter Supplier ID to Access Dashboard: ";
    string sID = readString();
    Supplier* activeSup = nullptr;
    for (int i = 0; i < state.suppliers.size(); ++i) {
        if (state.suppliers[i].getSupplierID() == sID) {
            activeSup = &state.suppliers[i];
            break;
        }
    }
    if (!activeSup) {
        cout << "Supplier ID not recognized. Ask Admin to register this supplier.\n" << endl;
        return;
    }

    while (true) {
        cout << "\n=== SUPPLIER DASHBOARD: " << activeSup->getSupplierName() << " ===" << endl;
        cout << "1. View Pending Deliveries" << endl;
        cout << "2. Fulfill Pending Delivery Order" << endl;
        cout << "3. View Contract Terms & Invoice Summary" << endl;
        cout << "4. Logout" << endl;
        cout << "Choice: ";
        int supChoice = readInt();
        if (supChoice == 4) break;

        if (supChoice == 1) {
            cout << "\n--- PENDING DELIVERY SLOTS ---" << endl;
            bool found = false;
            for (int i = 0; i < state.restockRequests.size(); ++i) {
                const RestockRequest& req = state.restockRequests[i];
                if (req.supplierID == activeSup->getSupplierID() && !req.isFulfilled) {
                    cout << "  * Order ID: " << req.orderID
                        << " | Product ID: " << req.productID
                        << " | Quantity Requested: " << req.quantityRequested << endl;
                    found = true;
                }
            }
            if (!found) {
                cout << "  No pending requests assigned from depot managers." << endl;
            }
        }
        else if (supChoice == 2) {
            cout << "Enter the Pending Order ID you wish to fulfill: ";
            string ordID = readString();
            int targetIdx = -1;
            for (int i = 0; i < state.restockRequests.size(); ++i) {
                if (state.restockRequests[i].orderID == ordID &&
                    state.restockRequests[i].supplierID == activeSup->getSupplierID() &&
                    !state.restockRequests[i].isFulfilled) {
                    targetIdx = i;
                    break;
                }
            }
            if (targetIdx == -1) {
                cout << "Active, unfulfilled Order ID matching criteria not located." << endl;
            }
            else {
                Product* p = state.globalWarehouse.findProductByID(state.restockRequests[targetIdx].productID);
                if (p) {
                    activeSup->orderRestock(*p, state.restockRequests[targetIdx].quantityRequested);
                    state.restockRequests[targetIdx].isFulfilled = true;
                    state.systemLogs.recordAction("Fulfilled Restock Order: " + ordID);
                    cout << "Delivery completed. Products updated inside warehouse stock." << endl;
                    saveRestockRequestsCSV(state);
                    saveProductsCSV(state);
                }
                else {
                    cout << "Product ID mapping missing in active warehouse slots." << endl;
                }
            }
        }
        else if (supChoice == 3) {
            cout << activeSup->generateInvoice() << endl;
        }
    }
}
