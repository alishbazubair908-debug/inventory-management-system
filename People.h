#ifndef PEOPLE_H
#define PEOPLE_H

#include "Utilities.h"
#include "Structures.h"
#include "Storage.h"
#include "Products.h"

using namespace std;

class Person {
protected:
    string personID;
    string name;
    string cnic;

public:
    Person() : personID(""), name(""), cnic("") {}
    Person(string id, string n) : personID(id), name(n), cnic("") {}
    Person(string id, string n, string c) : personID(id), name(n), cnic(c) {}

    virtual ~Person() {}

    string getPersonID() const { return personID; }
    string getName() const { return name; }
    string getCNIC() const { return cnic; }
    void setName(string n) { name = n; }
    void setCNIC(string c) { cnic = c; }

    virtual string getRole() const = 0;
};

class Supplier : public Person {
private:
    string contractTerms;

public:
    Supplier() : Person("SUP-00", "Generic Supplier"), contractTerms("Net 30") {}

    Supplier(string sID, string name, string terms)
        : Person(sID, name), contractTerms(terms) {
    }

    string getSupplierID() const { return getPersonID(); }
    string getSupplierName() const { return getName(); }
    string getContractTerms() const { return contractTerms; }

    string getRole() const override { return "Supplier"; }

    void orderRestock(Product& p, int qty) {
        p.setQuantity(p.getQuantity() + qty);
        cout << "[Restock Order] Vendor '" << getName() << "' supplied "
            << qty << " units of " << p.getProductName() << endl;
    }

    string generateInvoice() const {
        return "Invoice: " + getPersonID() + " - " + getName() + " | Contract: " + contractTerms;
    }

    bool operator==(const Supplier& other) const {
        return getPersonID() == other.getPersonID();
    }

    friend ostream& operator<<(ostream& os, const Supplier& s) {
        os << "Supplier [" << s.getSupplierID() << "]: " << s.getSupplierName() << " | Terms: " << s.contractTerms;
        return os;
    }
};

class Customer : public Person {
private:
    string contactInfo;
    DynamicArray<string> purchaseHistory;
    DynamicArray<PurchasedItem> purchasedItems;
    double totalSpent = 0.0;

public:
    Customer() : Person("CUST-00", "Guest"), contactInfo("None") {}

    Customer(string cID, string name, string contact)
        : Person(cID, name), contactInfo(contact) {
    }

    ~Customer() override = default;

    string getCustomerID() const { return getPersonID(); }
    string getCustomerName() const { return getName(); }
    string getContactInfo() const { return contactInfo; }

    string getRole() const override { return "Customer"; }

    void addPurchasedItem(const PurchasedItem& item) {
        purchasedItems.push_back(item);
        ostringstream logMsg;
        logMsg << "Cleared: Purchased " << item.quantity << "x " << item.productName
            << " for $" << fixed << setprecision(2) << item.pricePaid
            << " on " << item.purchaseDate;
        purchaseHistory.push_back(logMsg.str());
        totalSpent += item.pricePaid;
    }

    const DynamicArray<PurchasedItem>& getPurchasedItems() const { return purchasedItems; }

    void viewPurchaseHistory() const {
        cout << "Cleared Purchases for " << getName() << ":" << endl;
        if (purchaseHistory.empty()) {
            cout << "  No cleared, approved purchases recorded." << endl;
        }
        for (int i = 0; i < purchaseHistory.size(); ++i) {
            cout << "  - " << purchaseHistory[i] << endl;
        }
    }

    double getTotalSpent() const { return totalSpent; }

    bool operator==(const Customer& other) const {
        return getPersonID() == other.getPersonID();
    }

    friend ostream& operator<<(ostream& os, const Customer& c) {
        os << "Customer [" << c.getCustomerID() << "]: " << c.getCustomerName() << " | Spent: $" << c.totalSpent;
        return os;
    }
};

class Employee : public Person {
protected:
    double salary;
    string shiftSchedule;
    string dateOfBirth;
    string joiningDate;
    double hourlyRate;
    int hoursWorkedThisMonth;

public:
    Employee(string empID, string name, string cnic, double sal, string shift,
        string dob, string joinDate, double rate)
        : Person(empID, name, cnic), salary(sal), shiftSchedule(shift),
        dateOfBirth(dob), joiningDate(joinDate), hourlyRate(rate), hoursWorkedThisMonth(0) {
    }

    ~Employee() override = default;

    virtual void assignTask() = 0;

    void setHoursWorked(int hrs) { hoursWorkedThisMonth = hrs; }

    virtual double calculateMonthlyWage() const {
        double base = hourlyRate * static_cast<double>(hoursWorkedThisMonth);
        if (shiftSchedule == "Night" || shiftSchedule == "night") {
            base *= 1.2;
        }
        return base;
    }

    virtual void generateReport() {
        cout << "Employee Report: " << name << " (" << personID << ")\n"
            << "  - CNIC: " << cnic << "\n"
            << "  - DOB: " << dateOfBirth << " | Joining Date: " << joiningDate << "\n"
            << "  - Shift: " << shiftSchedule << " | Contract Base: $" << salary << "\n"
            << "  - Hourly Rate: $" << hourlyRate << "/hr | Hours Worked: " << hoursWorkedThisMonth << " hrs" << endl;
    }

    string getEmployeeID() const { return getPersonID(); }
    double getSalary() const { return salary; }
    string getShiftSchedule() const { return shiftSchedule; }
    string getDOB() const { return dateOfBirth; }
    string getJoiningDate() const { return joiningDate; }
    double getHourlyRate() const { return hourlyRate; }
    int getHoursWorked() const { return hoursWorkedThisMonth; }

    bool operator==(const Employee& other) const {
        return getPersonID() == other.getPersonID();
    }

    friend ostream& operator<<(ostream& os, const Employee& emp) {
        os << "ID: " << emp.getEmployeeID() << " | " << emp.getName() << " | Shift: " << emp.shiftSchedule;
        return os;
    }
};

class Warehouse;

class FinanceManager : public Employee {
public:
    FinanceManager(string empID, string name, string cnic, double sal, string shift,
        string dob, string joinDate, double rate)
        : Employee(empID, name, cnic, sal, shift, dob, joinDate, rate) {
    }

    string getRole() const override { return "Finance Manager"; }

    void assignTask() override {
        cout << "[Task] Finance Manager " << getName() << " is auditing payroll metrics." << endl;
    }

    double calculateMonthlyWage() const override {
        return salary;
    }

    void auditTreasury(const Warehouse& wh, double capital, double operatingExpenses) const {
        double stockValue = wh.getGlobalValue();
        cout << "==========================================" << endl;
        cout << "      TREASURY & STOCK VALUE LIQUIDITY    " << endl;
        cout << "==========================================" << endl;
        cout << "Corporate Funds Capital     : $" << capital << endl;
        cout << "Accumulated Expenses logged : $" << operatingExpenses << endl;
        cout << "Inventory Stock Asset Value : $" << stockValue << endl;
        double netWorth = capital + stockValue;
        cout << "Estimated Gross Net Worth   : $" << netWorth << endl;
        if (capital < stockValue * 0.15) {
            cout << "WARNING: Capital liquid cash reserves are low compared to static stock values!" << endl;
        }
        else {
            cout << "Liquidity levels are within a secure profile limit." << endl;
        }
        cout << "==========================================" << endl;
    }

    void addOperationalExpense(double amt, double& capital, double& operatingExpenses) {
        if (capital >= amt) {
            capital -= amt;
            operatingExpenses += amt;
            cout << "[Expense Recorded] Logged expense of $" << amt << ". Capital adjusted." << endl;
        }
        else {
            cout << "[Expense Warning] Inadequate funds to process corporate transaction." << endl;
        }
    }
};

class WarehouseManager : public Employee {
private:
    DynamicArray<Storage*> managedStorages;

public:
    WarehouseManager(string empID, string name, string cnic, double sal, string shift,
        string dob, string joinDate, double rate)
        : Employee(empID, name, cnic, sal, shift, dob, joinDate, rate) {
    }

    ~WarehouseManager() override = default;

    string getRole() const override { return "Warehouse Manager"; }

    void addManagedStorage(Storage* st) {
        managedStorages.push_back(st);
    }

    void assignTask() override {
        cout << "[Work Order] Manager " << getName() << " is delegating localized tasks." << endl;
    }

    void generateReport() override {
        Employee::generateReport();
        cout << "  - Overseeing sections: " << managedStorages.size() << endl;
    }

    void approvePurchaseOrder(Supplier& sup, Product& p, int qty) {
        cout << "[Approval] Ordering " << qty
            << " units of " << p.getProductName() << " with " << sup.getSupplierName() << endl;
        sup.orderRestock(p, qty);
    }

    void auditWarehouse(Warehouse& wh) {
        cout << "[Audit Commencing] Performing complete asset audit on Warehouse: " << wh.locationID_get() << endl;
        cout << "  Total active warehouse values: $" << wh.getGlobalValue() << endl;
        wh.findShortages();
    }
};

class StockKeeper : public Employee {
private:
    int assignedStorageNumber;
    int dailyCheckCount;

public:
    StockKeeper(string empID, string name, string cnic, double sal, string shift,
        string dob, string joinDate, double rate, int storage)
        : Employee(empID, name, cnic, sal, shift, dob, joinDate, rate), assignedStorageNumber(storage), dailyCheckCount(0) {
    }

    ~StockKeeper() override = default;

    string getRole() const override { return "Stock Keeper"; }
    int getAssignedStorageNumber() const { return assignedStorageNumber; }

    void assignTask() override {
        cout << "[Task] StockKeeper " << getName() << " reports to storage section " << assignedStorageNumber
            << " for visual audit." << endl;
        dailyCheckCount++;
    }

    void generateReport() override {
        Employee::generateReport();
        cout << "  - Primary assigned Storage: " << assignedStorageNumber
            << " | Audits executed today: " << dailyCheckCount << endl;
    }

    void performExpiryCheck(Storage& sec) {
        sec.checkAllExpiries();
    }

    void restockItem(Product* p, int qty) {
        p->setQuantity(p->getQuantity() + qty);
        cout << "[Restock] Replenished " << p->getProductName()
            << " with " << qty << " units." << endl;
    }

    void flagExpiredItem(PerishableGrocery& pg) {
        if (pg.checkExpiry()) {
            cout << "[HAZARD] " << pg.getProductName() << " (ID: " << pg.getProductID()
                << ") flagged. Scheduled for safe disposal." << endl;
        }
        else {
            cout << "[Safe] Product " << pg.getProductName() << " is safe. Expiration: "
                << pg.getExpiryDate() << endl;
        }
    }
};

#endif
