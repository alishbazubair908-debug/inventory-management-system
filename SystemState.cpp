#include "SystemState.h"

using namespace std;

void checkOrRegisterAdmin() {
    ifstream inFile("admin_credentials.txt");
    if (!inFile.is_open()) {
        cout << "==========================================" << endl;
        cout << " FIRST-TIME SETUP: REGISTER ADMIN ACCOUNT " << endl;
        cout << "==========================================" << endl;
        cout << "No administrator record located. Setup Admin parameters:" << endl;
        cout << "Enter Admin Username (Alphabets Only): ";
        string user = readAlphaString();
        cout << "Enter Admin CNIC (Format: XXXXX-XXXXXXX-X, e.g. 35201-1234567-1): ";
        string adminCnic = readCNIC();
        cout << "Enter Admin Password (Exactly 8 Digits): ";
        string pass = readPassword();
        ofstream outFile("admin_credentials.txt");
        if (outFile.is_open()) {
            outFile << user << "\n" << adminCnic << "\n" << simpleHash(pass) << "\n";
            outFile.close();
            cout << "\nAdmin configuration stored successfully!\n" << endl;
        }
        else {
            cerr << "Disk Error: Unable to store credentials." << endl;
        }
    }
    else {
        inFile.close();
    }
}

bool performAdminLogin() {
    ifstream inFile("admin_credentials.txt");
    string savedUser, savedCnic, savedHash;
    if (inFile.is_open()) {
        getline(inFile, savedUser);
        getline(inFile, savedCnic);
        getline(inFile, savedHash);
        inFile.close();
    }
    else {
        cout << "System Error: Admin file missing." << endl;
        return false;
    }
    cout << "--- Administrator Login ---" << endl;
    cout << "Username: ";
    string user = readString();
    cout << "Password (Exactly 8 Digits): ";
    string pass = readPassword();
    if (user == savedUser && simpleHash(pass) == savedHash) {
        cout << "Access Granted.\n" << endl;
        return true;
    }
    cout << "Access Denied. Incorrect credentials.\n" << endl;
    return false;
}

bool isEmployeeIDDuplicate(const SystemState& state, const string& id) {
    for (int i = 0; i < state.employees.size(); ++i) {
        if (state.employees[i] && state.employees[i]->getEmployeeID() == id) return true;
    }
    return false;
}

bool isProductIDDuplicate(const SystemState& state, const string& id) {
    for (int i = 0; i < state.globalWarehouse.getStorageCount(); ++i) {
        const Storage& sec = state.globalWarehouse.getStorage(i);
        for (int j = 0; j < sec.getProductCount(); ++j) {
            const Product* p = sec[j];
            if (p && p->getProductID() == id) return true;
        }
    }
    return false;
}

bool isCustomerIDDuplicate(const SystemState& state, const string& id) {
    for (int i = 0; i < state.customers.size(); ++i) {
        if (state.customers[i].getCustomerID() == id) return true;
    }
    return false;
}

bool isSupplierIDDuplicate(const SystemState& state, const string& id) {
    for (int i = 0; i < state.suppliers.size(); ++i) {
        if (state.suppliers[i].getSupplierID() == id) return true;
    }
    return false;
}

// Checks employees AND admin credentials file for duplicate CNIC
// formattedCNIC should be in "XXXXX-XXXXXXX-X" form
bool isCNICDuplicate(const SystemState& state, const string& formattedCNIC) {
    // Check all registered employees
    for (int i = 0; i < state.employees.size(); ++i) {
        if (state.employees[i] && state.employees[i]->getCNIC() == formattedCNIC) return true;
    }
    // Check admin credentials file
    ifstream inFile("admin_credentials.txt");
    if (inFile.is_open()) {
        string user, adminCNIC, hash;
        getline(inFile, user);
        getline(inFile, adminCNIC);
        inFile.close();
        if (adminCNIC == formattedCNIC) return true;
    }
    return false;
}
