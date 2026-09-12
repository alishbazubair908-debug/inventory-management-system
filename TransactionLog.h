#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include "Utilities.h"

using namespace std;

template<typename T>
class TransactionLog {
private:
    string logID;
    DynamicArray<T> logHistory;
    DynamicArray<string> timestamps;

public:
    TransactionLog() : logID("LOG-DEFAULT") {}
    TransactionLog(string id) : logID(id) {}

    void recordAction(T item) {
        logHistory.push_back(item);
        timestamps.push_back(getCurrentDateTime());
    }

    void recordActionWithTimestamp(T item, string ts) {
        logHistory.push_back(item);
        timestamps.push_back(ts);
    }

    int getLogCount() const { return logHistory.size(); }
    T getLogEntry(int idx) const { return logHistory[idx]; }
    string getTimestamp(int idx) const { return timestamps[idx]; }

    void printAuditTrail() const {
        cout << ">>> TIME-STAMPED AUDIT TRAIL FOR " << logID << " <<<" << endl;
        for (int i = 0; i < logHistory.size(); ++i) {
            cout << "  " << i + 1 << ". [" << timestamps[i] << "] " << logHistory[i] << endl;
        }
    }

    void printAuditTrailToFile(ofstream& file) const {
        file << ">>> TIME-STAMPED AUDIT TRAIL FOR " << logID << " <<<\n";
        for (int i = 0; i < logHistory.size(); ++i) {
            file << "  " << i + 1 << ". [" << timestamps[i] << "] " << logHistory[i] << "\n";
        }
    }

    T getEntry(int index) const {
        if (index < 0 || index >= logHistory.size()) {
            cerr << "Error: Out of bounds. Returning default instantiated type." << endl;
            return T();
        }
        return logHistory[index];
    }

    void clearLog() {
        logHistory.clear();
        timestamps.clear();
    }

    friend ostream& operator<<(ostream& os, const TransactionLog<T>& tl) {
        os << "Log [" << tl.logID << "] holds " << tl.logHistory.size() << " operations.";
        return os;
    }
};

#endif
