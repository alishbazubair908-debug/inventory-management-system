#ifndef PRODUCTS_H
#define PRODUCTS_H

#include "Utilities.h"

using namespace std;

class Barcode {
private:
    string barcodeValue;
    string format;

public:
    Barcode() : barcodeValue("000000000000"), format("UPC") {}

    Barcode(const string& val, const string& fmt) : format(fmt) {
        setValue(val);
    }

    void setValue(string val) {
        if (format == "EAN13" && (val.length() != 13 || val.find_first_not_of("0123456789") != string::npos)) {
            barcodeValue = "0000000000000";
        }
        else if (format == "UPC" && (val.length() != 12 || val.find_first_not_of("0123456789") != string::npos)) {
            barcodeValue = "000000000000";
        }
        else {
            barcodeValue = val;
        }
    }

    string getValue() const { return barcodeValue; }
    string getFormat() const { return format; }

    bool validate() const {
        if (format == "EAN13") return barcodeValue.length() == 13;
        if (format == "UPC") return barcodeValue.length() == 12;
        return !barcodeValue.empty();
    }

    string scan() const {
        return "[" + format + "] " + barcodeValue;
    }

    bool operator==(const Barcode& other) const {
        return (barcodeValue == other.barcodeValue && format == other.format);
    }

    friend ostream& operator<<(ostream& os, const Barcode& b) {
        os << b.scan();
        return os;
    }
};

class DiscountPolicy {
protected:
    string policyName;
    string startDate;
    string endDate;

public:
    DiscountPolicy(string name, string start, string end)
        : policyName(name), startDate(start), endDate(end) {
    }

    virtual ~DiscountPolicy() {}

    virtual double calculate(double price) = 0;

    virtual bool isActive() const;

    string getPolicyName() const { return policyName; }

    friend ostream& operator<<(ostream& os, const DiscountPolicy& dp) {
        os << "Policy: " << dp.policyName << " [" << dp.startDate << " to " << dp.endDate << "]";
        return os;
    }
};

class BulkDiscount : public DiscountPolicy {
private:
    int minQuantity;
    double discountPercent;
    int currentQuantity;

public:
    BulkDiscount(string name, string start, string end, int minQty, double percent)
        : DiscountPolicy(name, start, end), minQuantity(minQty), discountPercent(percent), currentQuantity(0) {
    }

    void setCurrentQuantity(int qty) { currentQuantity = qty; }

    double calculate(double price) override {
        if (isActive()) {
            return price * (1.0 - (discountPercent / 100.0));
        }
        return price;
    }

    bool isActive() const override {
        return DiscountPolicy::isActive() && (currentQuantity >= minQuantity);
    }
};

class SeasonalDiscount : public DiscountPolicy {
private:
    string seasonName;
    double discountRate;

public:
    SeasonalDiscount(string name, string start, string end, string season, double rate)
        : DiscountPolicy(name, start, end), seasonName(season), discountRate(rate) {
    }

    double calculate(double price) override {
        if (isActive()) {
            return price * (1.0 - discountRate);
        }
        return price;
    }

    bool isActive() const override { return DiscountPolicy::isActive(); }

    string getSeasonName() const { return seasonName; }
};

class Product {
protected:
    string productID;
    string productName;
    double price;
    double originalPrice;
    int quantity;
    Barcode barcode;
    string activeDiscountPolicyName;
    int storageNumber = 1;

public:
    Product(string id, string name, double p, int qty, Barcode b)
        : productID(id), productName(name), price(p), originalPrice(p), quantity(qty), barcode(b), activeDiscountPolicyName("None") {
    }

    virtual ~Product() {}

    virtual unique_ptr<Product> clone() const = 0;
    virtual double calculateValue() const { return price * quantity; };

    virtual void displayStatus() = 0;
    virtual double calculateRisk() = 0;
    virtual void applyDiscount(DiscountPolicy& policy) = 0;
    virtual string getCategory() const = 0;
    virtual string serialize() const = 0;

    virtual string getSubtype() const { return "Standard"; }
    virtual int getWarrantyMonths() const { return 0; }

    virtual void checkSafety() {
        cout << "[Safety Check] Product " << productName << " passes baseline standard audits." << endl;
    }

    void printDiscountInfo() const {
        if (activeDiscountPolicyName != "None" && price < originalPrice) {
            double pct = getDiscountPercent();
            cout << " [SALE: Save " << fixed << setprecision(0) << pct
                << "%! Original: $" << fixed << setprecision(2) << originalPrice
                << " | Current: $" << price << " (via " << activeDiscountPolicyName << ")]";
        }
    }

    double getDiscountPercent() const {
        if (originalPrice <= 0.0) return 0.0;
        return ((originalPrice - price) / originalPrice) * 100.0;
    }

    string getProductID() const { return productID; }
    string getProductName() const { return productName; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    Barcode getBarcode() const { return barcode; }

    void setPrice(double p) { price = p; }
    void setQuantity(int qty) { quantity = qty; }

    void setStorageNumber(int num) { storageNumber = num; }
    int getStorageNumber() const { return storageNumber; }

    bool operator==(const Product& other) const {
        return productID == other.productID;
    }

    friend ostream& operator<<(ostream& os, const Product& p) {
        os << "ID: " << p.productID << " | Name: " << p.productName
            << " | Price: $" << fixed << setprecision(2) << p.price
            << " | Qty: " << p.quantity << " | Barcode: " << p.barcode;
        if (p.activeDiscountPolicyName != "None") {
            os << " [Discounted from $" << p.originalPrice << " via " << p.activeDiscountPolicyName << "]";
        }
        return os;
    }
};

class ElectronicProduct : public Product {
protected:
    int voltage;
    int warrantyMonths;
    string brandName;

public:
    ElectronicProduct(string id, string name, double p, int qty, Barcode b,
        int volt, int warranty, string brand)
        : Product(id, name, p, qty, b), voltage(volt), warrantyMonths(warranty), brandName(brand) {
    }

    ~ElectronicProduct() override = default;

    unique_ptr<Product> clone() const override {
        return make_unique<ElectronicProduct>(*this);
    }

    bool testHardware() const { return (voltage >= 110 && voltage <= 240); }

    string getWarrantyStatus() const {
        ostringstream oss;
        oss << warrantyMonths << " Month Warranty from " << brandName;
        return oss.str();
    }

    string getCategory() const override { return "Electronic"; }
    int getWarrantyMonths() const override { return warrantyMonths; }

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Electronic,Standard," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << voltage << "," << warrantyMonths << "," << brandName;
        return oss.str();
    }

    void displayStatus() override {
        cout << "[Electronic] " << productName << " (ID: " << productID
            << ") | Brand: " << brandName << " | Voltage: " << voltage
            << "V | Price: $" << fixed << setprecision(2) << price
            << " | Stock: " << quantity;
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override {
        return (static_cast<double>(voltage) * 0.02) + (36.0 / (static_cast<double>(warrantyMonths) + 1.0));
    }

    void applyDiscount(DiscountPolicy& policy) override {
        BulkDiscount* bulk = dynamic_cast<BulkDiscount*>(&policy);
        if (bulk) {
            bulk->setCurrentQuantity(quantity);
        }
        if (policy.isActive()) {
            activeDiscountPolicyName = policy.getPolicyName();
            setPrice(policy.calculate(originalPrice));
        }
        else {
            cout << "[Notice] Discount Policy '" << policy.getPolicyName() << "' is not currently active." << endl;
        }
    }
};

class FragileElectronics : public ElectronicProduct {
private:
    int fragilityRating;
    string packagingType;

public:
    FragileElectronics(string id, string name, double p, int qty, Barcode b,
        int volt, int warranty, string brand, int fragility, string packType)
        : ElectronicProduct(id, name, p, qty, b, volt, warranty, brand),
        fragilityRating(fragility), packagingType(packType) {
    }

    unique_ptr<Product> clone() const override {
        return make_unique<FragileElectronics>(*this);
    }

    string getCategory() const override { return "Electronic"; }
    string getSubtype() const override { return "Fragile"; }

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Electronic,Fragile," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << voltage << "," << warrantyMonths << "," << brandName << ","
            << fragilityRating << "," << packagingType;
        return oss.str();
    }

    void displayStatus() override {
        cout << "[Fragile Electronic] " << productName << " (ID: " << productID
            << ") | Brand: " << brandName << " | Fragility: " << fragilityRating << "/10"
            << " | Price: $" << fixed << setprecision(2) << price;
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override {
        return ElectronicProduct::calculateRisk() * (1.0 + (static_cast<double>(fragilityRating) * 0.2));
    }

    void applyDiscount(DiscountPolicy& policy) override {
        ElectronicProduct::applyDiscount(policy);
    }

    double calculateShippingRisk() const { return static_cast<double>(fragilityRating) * 1.5; }

    string getPackagingGuide() const {
        return "Wrap with " + packagingType + " and apply 'FRAGILE' shipping labels.";
    }
};

class GroceryProduct : public Product {
protected:
    int calories;
    bool isHalal;
    string countryOfOrigin;

public:
    GroceryProduct(string id, string name, double p, int qty, Barcode b,
        int cal, bool halal, string origin)
        : Product(id, name, p, qty, b), calories(cal), isHalal(halal), countryOfOrigin(origin) {
    }

    unique_ptr<Product> clone() const override {
        return make_unique<GroceryProduct>(*this);
    }

    string getCategory() const override { return "Grocery"; }

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Grocery,Standard," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << calories << "," << (isHalal ? 1 : 0) << "," << countryOfOrigin;
        return oss.str();
    }

    void checkSafety() override {
        cout << "[Safety Check] Grocery item " << productName << " imported from "
            << countryOfOrigin << " passes standard food safety audits." << endl;
    }

    void displayStatus() override {
        cout << "[Grocery] " << productName << " (ID: " << productID
            << ") | Calories: " << calories << " kcal | Halal: "
            << (isHalal ? "Yes" : "No") << " | Price: $" << fixed << setprecision(2) << price;
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override { return isHalal ? 0.1 : 0.3; }

    void applyDiscount(DiscountPolicy& policy) override {
        BulkDiscount* bulk = dynamic_cast<BulkDiscount*>(&policy);
        if (bulk) {
            bulk->setCurrentQuantity(quantity);
        }
        if (policy.isActive()) {
            activeDiscountPolicyName = policy.getPolicyName();
            setPrice(policy.calculate(originalPrice));
        }
        else {
            cout << "[Notice] Discount Policy '" << policy.getPolicyName() << "' is not active." << endl;
        }
    }
};

class PerishableGrocery : public GroceryProduct {
private:
    string expiryDate;
    double storageTemp;

public:
    PerishableGrocery(string id, string name, double p, int qty, Barcode b,
        int cal, bool halal, string origin, string expiry, double temp)
        : GroceryProduct(id, name, p, qty, b, cal, halal, origin),
        expiryDate(expiry), storageTemp(temp) {
    }

    unique_ptr<Product> clone() const override {
        return make_unique<PerishableGrocery>(*this);
    }

    string getCategory() const override { return "Grocery"; }
    string getSubtype() const override { return "Perishable"; }

    string getExpiryDate() const { return expiryDate; }
    double getStorageTemp() const { return storageTemp; }

    bool checkExpiry() const;

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Grocery,Perishable," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << calories << "," << (isHalal ? 1 : 0) << "," << countryOfOrigin << ","
            << expiryDate << "," << storageTemp;
        return oss.str();
    }

    void checkSafety() override {
        GroceryProduct::checkSafety();
        if (checkExpiry()) {
            cout << "  -> WARNING: Product expired on " << expiryDate << "!" << endl;
        }
        else {
            cout << "  -> Product is safe. Storage environment: "
                << storageTemp << "C" << endl;
        }
    }

    void displayStatus() override {
        cout << "[Perishable Grocery] " << productName << " (ID: " << productID
            << ") | Expires: " << expiryDate << " | Temperature: "
            << storageTemp << "C | Stock: " << quantity << " | Price: $" << fixed << setprecision(2) << price;
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override {
        if (checkExpiry()) return 10.0;
        return (storageTemp < 4.0) ? 0.5 : 2.5;
    }

    void applyDiscount(DiscountPolicy& policy) override {
        GroceryProduct::applyDiscount(policy);
    }
};

class NonPerishable : public GroceryProduct {
private:
    int shelfLifeYears;
    string preservativeLevel;

public:
    NonPerishable(string id, string name, double p, int qty, Barcode b,
        int cal, bool halal, string origin, int shelfLife, string presLevel)
        : GroceryProduct(id, name, p, qty, b, cal, halal, origin),
        shelfLifeYears(shelfLife), preservativeLevel(presLevel) {
    }

    unique_ptr<Product> clone() const override {
        return make_unique<NonPerishable>(*this);
    }

    string getCategory() const override { return "Grocery"; }
    string getSubtype() const override { return "Non-Perishable"; }

    string getStorageInstructions() const {
        ostringstream oss;
        oss << "Store in dry room condition. Preservation category: " << preservativeLevel;
        return oss.str();
    }

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Grocery,Non-Perishable," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << calories << "," << (isHalal ? 1 : 0) << "," << countryOfOrigin << ","
            << shelfLifeYears << "," << preservativeLevel;
        return oss.str();
    }

    void checkSafety() override {
        cout << "[Safety Check] Non-perishable " << productName
            << " has preservation level stability check passed: " << preservativeLevel << endl;
    }

    void displayStatus() override {
        cout << "[Non-Perishable Grocery] " << productName << " (ID: " << productID
            << ") | Shelf life: " << shelfLifeYears << " years | Preservatives: "
            << preservativeLevel << " | Price: $" << fixed << setprecision(2) << price;
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override { return 0.005; }

    void applyDiscount(DiscountPolicy& policy) override {
        GroceryProduct::applyDiscount(policy);
    }
};

class VegFruitGrocery : public GroceryProduct {
public:
    VegFruitGrocery(string id, string name, double pricePerKg, int qtyGrams, Barcode b,
        int cal, bool halal, string origin)
        : GroceryProduct(id, name, pricePerKg, qtyGrams, b, cal, halal, origin) {
    }

    unique_ptr<Product> clone() const override {
        return make_unique<VegFruitGrocery>(*this);
    }

    string getCategory() const override { return "Grocery"; }
    string getSubtype() const override { return "VegFruit"; }

    double calculateValue() const override {
        return (price / 1000.0) * static_cast<double>(quantity);
    }

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Grocery,VegFruit," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << calories << "," << (isHalal ? 1 : 0) << "," << countryOfOrigin;
        return oss.str();
    }

    void displayStatus() override {
        double kg = static_cast<double>(quantity) / 1000.0;
        cout << "[Veg/Fruit] " << productName << " (ID: " << productID
            << ") | Stock: " << fixed << setprecision(2) << kg << " kg"
            << " | Price: $" << fixed << setprecision(2) << price << "/kg";
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override { return isHalal ? 0.05 : 0.2; }

    void applyDiscount(DiscountPolicy& policy) override {
        GroceryProduct::applyDiscount(policy);
    }
};

class ClothingProduct : public Product {
private:
    string size;
    string fabric;
    string gender;

public:
    ClothingProduct(string id, string name, double p, int qty, Barcode b,
        string sz, string fab, string gen)
        : Product(id, name, p, qty, b), size(sz), fabric(fab), gender(gen) {
    }

    unique_ptr<Product> clone() const override {
        return make_unique<ClothingProduct>(*this);
    }

    string getCategory() const override { return "Clothing"; }

    string fitGuide() const {
        return "Target: " + gender + " | Size: " + size + " | Material: " + fabric;
    }

    string serialize() const override {
        ostringstream oss;
        oss << productID << "," << productName << "," << price << "," << quantity << ",Clothing,Standard," << storageNumber << ","
            << barcode.getValue() << "," << barcode.getFormat() << "," << size << "," << fabric << "," << gender;
        return oss.str();
    }

    void displayStatus() override {
        cout << "[Clothing] " << productName << " (ID: " << productID
            << ") | Size: " << size << " | Material: " << fabric << " | Stock: " << quantity << " | Price: $" << fixed << setprecision(2) << price;
        printDiscountInfo();
        cout << endl;
    }

    double calculateRisk() override { return 0.15; }

    void applyDiscount(DiscountPolicy& policy) override {
        BulkDiscount* bulk = dynamic_cast<BulkDiscount*>(&policy);
        if (bulk) {
            bulk->setCurrentQuantity(quantity);
        }
        if (policy.isActive()) {
            activeDiscountPolicyName = policy.getPolicyName();
            setPrice(policy.calculate(originalPrice));
        }
        else {
            cout << "[Notice] Discount Policy '" << policy.getPolicyName() << "' is not active." << endl;
        }
    }
};

#endif
