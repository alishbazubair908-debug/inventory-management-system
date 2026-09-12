#ifndef PORTALS_H
#define PORTALS_H

#include "SystemState.h"
#include "Persistence.h"

using namespace std;

void runAdminPortal(SystemState& state);
void runEmployeePortal(SystemState& state);
void runCustomerPortal(SystemState& state);
void runSupplierPortal(SystemState& state);

#endif
