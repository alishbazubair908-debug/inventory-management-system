#ifndef STORAGE_H
#define STORAGE_H

#include "Products.h"

using namespace std;

class Storage {
private:
    int storageNumber;
    int capacity;
    string allowedCategory;
    DynamicArray<unique_ptr<Product>> products;

public:
    Storage() : storageNumber(0), capacity(10), allowedCategory("Any") {}

    Storage(int number, int cap, string restriction)
        : storageNumber(number), capacity(cap), allowedCategory(restriction) {
    }

    Storage(const Storage& other)
        : storageNumber(other.storageNumber), capacity(other.capacity), allowedCategory(other.allowedCategory) {
        for (int i = 0; i < other.products.size(); ++i) {
            if (other.products[i]) {
                products.push_back(other.products[i]->clone());
            }
        }
    }

    Storage& operator=(const Storage& other) {
        if (this != &other) {
            storageNumber = other.storageNumber;
            capacity = other.capacity;
            allowedCategory = other.allowedCategory;
            products.clear();
            for (int i = 0; i < other.products.size(); ++i) {
                if (other.products[i]) {
                    products.push_back(other.products[i]->clone());
                }
            }
        }
        return *this;
    }

    Storage(Storage&&) noexcept = default;
    Storage& operator=(Storage&&) noexcept = default;
    ~Storage() = default;

    int getStorageNumber() const { return storageNumber; }
    int getProductCount() const { return products.size(); }
    int getCapacity() const { return capacity; }
    string getAllowedCategory() const { return allowedCategory; }

    bool addStock(unique_ptr<Product> p) {
        if (allowedCategory != "Any" && p->getCategory() != allowedCategory) {
            cout << "\n[Error] Cannot place item in this area!" << endl;
            cout << "  -> " << p->getProductName() << " is of category: " << p->getCategory() << endl;
            cout << "  -> Storage " << storageNumber << " only accepts: " << allowedCategory << endl;
            return false;
        }
        p->setStorageNumber(storageNumber);
        products.push_back(std::move(p));
        return true;
    }

    unique_ptr<Product> takeProduct(int index) {
        if (index < 0 || index >= products.size()) return nullptr;
        unique_ptr<Product> p = std::move(products[index]);
        products.erase(index);
        return p;
    }

    void removeStock(const string& id) {
        for (int i = products.size() - 1; i >= 0; --i) {
            if (products[i] && products[i]->getProductID() == id) {
                products.erase(i);
            }
        }
    }

    void sortByID() {
        for (int i = 0; i < products.size() - 1; ++i) {
            for (int j = 0; j < products.size() - 1 - i; ++j) {
                if (products[j] && products[j + 1] &&
                    products[j]->getProductID() > products[j + 1]->getProductID()) {
                    unique_ptr<Product> tmp = std::move(products[j]);
                    products[j] = std::move(products[j + 1]);
                    products[j + 1] = std::move(tmp);
                }
            }
        }
    }

    double getTotalValue() const {
        double total = 0.0;
        for (int i = 0; i < products.size(); ++i) {
            if (products[i]) {
                total += products[i]->calculateValue();
            }
        }
        return total;
    }

    void checkAllExpiries() {
        cout << "\n>>> Conducting Dynamic Safety and Expiry Audits inside Storage " << storageNumber << " <<<" << endl;
        bool checkedAny = false;
        for (int i = 0; i < products.size(); ++i) {
            if (products[i]) {
                if (products[i]->getSubtype() == "Perishable") {
                    cout << "  * [Perishable Check] ";
                    products[i]->checkSafety();
                    checkedAny = true;
                }
                else if (products[i]->getSubtype() == "Non-Perishable") {
                    cout << "  * [Preservation Check] ";
                    products[i]->checkSafety();
                    checkedAny = true;
                }
            }
        }
        if (!checkedAny) {
            cout << "  -> No perishable or preservation-stabilized grocery profiles found in this storage." << endl;
        }
    }

    Product* operator[](int index) {
        if (index < 0 || index >= products.size()) {
            cerr << "Error: Storage pointer index out of bounds." << endl;
            return nullptr;
        }
        return products[index].get();
    }

    const Product* operator[](int index) const {
        if (index < 0 || index >= products.size()) {
            cerr << "Error: Storage pointer index out of bounds." << endl;
            return nullptr;
        }
        return products[index].get();
    }

    Storage operator+(const Storage& other) {
        Storage merged(storageNumber, capacity + other.capacity, allowedCategory);
        for (int i = 0; i < products.size(); ++i) {
            if (products[i]) merged.addStock(products[i]->clone());
        }
        for (int i = 0; i < other.products.size(); ++i) {
            if (other.products[i]) merged.addStock(other.products[i]->clone());
        }
        return merged;
    }

    friend ostream& operator<<(ostream& os, const Storage& s) {
        os << "Storage Section " << s.storageNumber << " [" << s.allowedCategory << " category only] (" << s.products.size() << "/" << s.capacity << " Occupied):\n";
        if (s.products.empty()) {
            os << "  (Storage currently empty - awaiting employee stock entry)\n";
        }
        else {
            for (int i = 0; i < s.products.size(); ++i) {
                os << "  [Item " << i + 1 << "] ";
                s.products[i]->displayStatus();
            }
        }
        return os;
    }
};

class Warehouse {
private:
    string locationID;
    double totalSquareFootage;
    DynamicArray<Storage> storages;

public:
    Warehouse(string locID, double sqFootage)
        : locationID(locID), totalSquareFootage(sqFootage) {
    }

    ~Warehouse() = default;

    string locationID_get() const { return locationID; }
    int getStorageCount() const { return storages.size(); }

    void clearStorages() {
        storages.clear();
    }

    Storage* getStoragePtr(int index) {
        if (index < 0 || index >= storages.size()) return nullptr;
        return &storages[index];
    }

    const Storage* getStoragePtr(int index) const {
        if (index < 0 || index >= storages.size()) return nullptr;
        return &storages[index];
    }

    Storage& getStorage(int index) {
        if (index < 0 || index >= storages.size()) {
            cerr << "[FATAL] getStorage(): index " << index
                << " out of range (size=" << storages.size() << "). Aborting." << endl;
            abort();
        }
        return storages[index];
    }

    const Storage& getStorage(int index) const {
        if (index < 0 || index >= storages.size()) {
            cerr << "[FATAL] getStorage() const: index " << index
                << " out of range (size=" << storages.size() << "). Aborting." << endl;
            abort();
        }
        return storages[index];
    }

    void addStorage(Storage st) {
        storages.push_back(std::move(st));
    }

    double getGlobalValue() const {
        double total = 0.0;
        for (int i = 0; i < storages.size(); ++i) {
            total += storages[i].getTotalValue();
        }
        return total;
    }

    void removeStorageByIndex(int index) {
        if (index >= 0 && index < storages.size()) {
            storages.erase(index);
        }
    }

    void findShortages() const {
        cout << "[Low Stock Alerts] Scanning shortages in '" << locationID << "':" << endl;
        bool lowFound = false;
        for (int i = 0; i < storages.size(); ++i) {
            const Storage& st = storages[i];
            for (int j = 0; j < st.getProductCount(); ++j) {
                const Product* item = st[j];
                if (item && item->getQuantity() < 5) {
                    cout << "  - ALERT: Storage " << st.getStorageNumber()
                        << " | Item " << item->getProductName() << " (ID: "
                        << item->getProductID() << ") has quantity " << item->getQuantity() << endl;
                    lowFound = true;
                }
            }
        }
        if (!lowFound) {
            cout << "  -> Storage profiles are healthy." << endl;
        }
    }

    Product* findProductByID(const string& id) {
        for (int i = 0; i < storages.size(); ++i) {
            Storage& st = storages[i];
            for (int j = 0; j < st.getProductCount(); ++j) {
                Product* p = st[j];
                if (p && p->getProductID() == id) {
                    return p;
                }
            }
        }
        return nullptr;
    }

    const Product* findProductByID(const string& id) const {
        for (int i = 0; i < storages.size(); ++i) {
            const Storage& st = storages[i];
            for (int j = 0; j < st.getProductCount(); ++j) {
                const Product* p = st[j];
                if (p && p->getProductID() == id) {
                    return p;
                }
            }
        }
        return nullptr;
    }

    friend ostream& operator<<(ostream& os, const Warehouse& wh) {
        os << "==========================================\n"
            << "Warehouse: " << wh.locationID << " (" << wh.totalSquareFootage << " Sq.Ft.)\n"
            << "Active Storage Sections: " << wh.storages.size() << "\n"
            << "==========================================\n";
        for (int i = 0; i < wh.storages.size(); ++i) {
            os << wh.storages[i] << "\n";
        }
        return os;
    }
};

#endif
