#include "Utilities.h"
#include "SystemState.h"
#include "Persistence.h"
#include "Portals.h"

using namespace std;

int main() {
    checkOrRegisterAdmin();

    SystemState state;
    loadAllCSVFiles(state);

    while (true) {
        cout << "==========================================" << endl;
        cout << "        INVENTORY SYSTEM PORTAL           " << endl;
        cout << "==========================================" << endl;
        cout << "1. Administrator Portal" << endl;
        cout << "2. Employee Portal (StockKeeper / Manager)" << endl;
        cout << "3. Customer checkout Portal" << endl;
        cout << "4. Supplier Fulfilment Portal" << endl;
        cout << "5. Exit Application" << endl;
        cout << "Select Portal Access (1-5): ";

        int mainSel = readInt();
        if (mainSel == 5) {
            saveAllCSVFiles(state);
            cout << "Exiting application. All changes successfully written to disk." << endl;
            break;
        }

        if (mainSel == 1) {
            runAdminPortal(state);
        }
        else if (mainSel == 2) {
            runEmployeePortal(state);
        }
        else if (mainSel == 3) {
            runCustomerPortal(state);
        }
        else if (mainSel == 4) {
            runSupplierPortal(state);
        }
    }

    return 0;
}
