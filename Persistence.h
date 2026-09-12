#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "SystemState.h"

using namespace std;

void loadSuppliersCSV(SystemState& state);
void saveSuppliersCSV(const SystemState& state);
void loadCustomersCSV(SystemState& state);
void saveCustomersCSV(const SystemState& state);
void loadEmployeesCSV(SystemState& state);
void saveEmployeesCSV(const SystemState& state);
void loadProductsCSV(SystemState& state);
void saveProductsCSV(const SystemState& state);
void loadOrdersCSV(SystemState& state);
void saveOrdersCSV(const SystemState& state);
void loadWarrantyClaimsCSV(SystemState& state);
void saveWarrantyClaimsCSV(const SystemState& state);
void loadRestockRequestsCSV(SystemState& state);
void saveRestockRequestsCSV(const SystemState& state);
void loadSystemLogsCSV(SystemState& state);
void saveSystemLogsCSV(const SystemState& state);
void loadStoragesCSV(SystemState& state);
void saveStoragesCSV(const SystemState& state);
void loadAllCSVFiles(SystemState& state);
void saveAllCSVFiles(const SystemState& state);
void appendProductRecord(const string& filename, const string& record);

void generateWarehouseReport(const SystemState& state);
void generatePayrollReport(const SystemState& state);
void generateOrdersReport(const SystemState& state);
void generateAuditLogReport(const SystemState& state);
void generateExpiryReport(SystemState& state);
void generateCustomerReceipt(const CustomerOrder& ord, const string& custName);
void generateFinanceExcel(const SystemState& state);

#endif
