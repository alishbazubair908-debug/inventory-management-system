#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>

using namespace std;

struct PurchasedItem {
    string productID = "";
    string productName = "";
    int quantity = 0;
    double pricePaid = 0.0;
    string purchaseDate = "";
    int warrantyMonths = 0;
};

struct CustomerOrder {
    string orderID = "";
    string customerID = "";
    string productID = "";
    int quantity = 0;
    double totalPrice = 0.0;
    bool isApproved = false;
    bool isRejected = false;
};

struct WarrantyClaim {
    string claimID = "";
    string customerID = "";
    string productID = "";
    string claimType = "";
    double refundAmount = 0.0;
    bool isProcessed = false;
};

struct RestockRequest {
    string orderID = "";
    string supplierID = "";
    string productID = "";
    int quantityRequested = 0;
    bool isFulfilled = false;
};

#endif
