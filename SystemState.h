#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include "Utilities.h"
#include "Structures.h"
#include "Storage.h"
#include "People.h"
#include "TransactionLog.h"

using namespace std;

class SystemState {
public:
    double availableCapital = 500000.00;
    double totalOperatingExpenses = 0.0;
    bool payrollAuthorizedByAdmin = true;

    Warehouse globalWarehouse;
    DynamicArray<unique_ptr<Employee>> employees;
    DynamicArray<Customer> customers;
    DynamicArray<Supplier> suppliers;
    DynamicArray<RestockRequest> restockRequests;
    DynamicArray<CustomerOrder> pendingOrders;
    DynamicArray<WarrantyClaim> warrantyClaims;
    TransactionLog<string> systemLogs;

    SystemState() : globalWarehouse("Central-Depot-01", 30000.0), systemLogs("SYSTEM-AUDITS") {}
};

void checkOrRegisterAdmin();
bool performAdminLogin();

bool isEmployeeIDDuplicate(const SystemState& state, const string& id);
bool isProductIDDuplicate(const SystemState& state, const string& id);
bool isCustomerIDDuplicate(const SystemState& state, const string& id);
bool isSupplierIDDuplicate(const SystemState& state, const string& id);
bool isCNICDuplicate(const SystemState& state, const string& formattedCNIC);

#endif
