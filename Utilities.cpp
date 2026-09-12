#include "Utilities.h"

using namespace std;

string trim(const string& str) {
    if (str.empty()) return str;
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool isAllAlphaOrSpace(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ')) {
            return false;
        }
    }
    return true;
}

void flushInput() {
    cin.clear();
    while (cin.get() != '\n' && cin.good());
}

string readString() {
    string val;
    getline(cin, val);
    return val;
}

string readAlphaString() {
    string val;
    while (true) {
        getline(cin, val);
        if (isAllAlphaOrSpace(val)) {
            break;
        }
        cout << "Invalid. Use alphabetic characters and spaces only: ";
    }
    return val;
}

bool isValidDate(const string& str) {
    if (str.length() != 10) return false;
    if (str[4] != '-' || str[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (str[i] < '0' || str[i] > '9') return false;
    }
    int y = (str[0] - '0') * 1000 + (str[1] - '0') * 100 + (str[2] - '0') * 10 + (str[3] - '0');
    int m = (str[5] - '0') * 10 + (str[6] - '0');
    int d = (str[8] - '0') * 10 + (str[9] - '0');
    if (m < 1 || m > 12) return false;
    if (d < 1) return false;
    bool isLeap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    int maxDays = 31;
    if (m == 4 || m == 6 || m == 9 || m == 11) {
        maxDays = 30;
    }
    else if (m == 2) {
        maxDays = isLeap ? 29 : 28;
    }
    return d <= maxDays;
}

bool isDateBeforeOrEqual(const string& date1, const string& date2) {
    return date1 <= date2;
}

string readDate() {
    string input;
    while (true) {
        getline(cin, input);
        if (isValidDate(input)) {
            break;
        }
        cout << "Invalid date or calendar boundaries. Format as YYYY-MM-DD: ";
    }
    return input;
}

int parseYear(const string& dateStr) {
    if (dateStr.length() < 4) return 0;
    int yr = 0;
    for (int i = 0; i < 4; ++i) {
        if (dateStr[i] >= '0' && dateStr[i] <= '9') {
            yr = yr * 10 + (dateStr[i] - '0');
        }
    }
    return yr;
}

string addMonthsToDate(const string& dateStr, int months) {
    if (dateStr.length() != 10) return dateStr;
    int y = parseYear(dateStr);
    int m = (dateStr[5] - '0') * 10 + (dateStr[6] - '0');
    int d = (dateStr[8] - '0') * 10 + (dateStr[9] - '0');
    m += months;
    while (m > 12) {
        m -= 12;
        y += 1;
    }
    ostringstream oss;
    oss << y << "-" << setw(2) << setfill('0') << m << "-" << setw(2) << setfill('0') << d;
    return oss.str();
}

bool validateAgeAtJoining(const string& dob, const string& joinDate) {
    int dobYear = parseYear(dob);
    int joinYear = parseYear(joinDate);
    return (joinYear - dobYear >= 20);
}

bool validateDOBAge(const string& dob) {
    int dobYear = parseYear(dob);
    int currentYear = 2026;
    return (currentYear - dobYear >= 20);
}

string readPassword() {
    string pass;
    while (true) {
        getline(cin, pass);
        if (pass.length() != 8) {
            cout << "Invalid. Password must be exactly 8 digits: ";
            continue;
        }
        bool onlyDigits = true;
        for (char c : pass) {
            if (c < '0' || c > '9') {
                onlyDigits = false;
                break;
            }
        }
        if (!onlyDigits) {
            cout << "Invalid. Password must contain numeric digits only: ";
            continue;
        }
        break;
    }
    return pass;
}

// Strips dashes: "35201-1234567-1" -> "3520112345671"
string stripCNIC(const string& formatted) {
    string raw = "";
    for (char c : formatted) {
        if (c != '-') raw += c;
    }
    return raw;
}

// Formats raw digits: "3520112345671" -> "35201-1234567-1"
string formatCNIC(const string& raw) {
    if (raw.length() != 13) return raw;
    return raw.substr(0, 5) + "-" + raw.substr(5, 7) + "-" + raw.substr(12, 1);
}

// Accepts raw 13-digit OR formatted XXXXX-XXXXXXX-X
bool isValidCNIC(const string& str) {
    string raw = stripCNIC(str);
    if (raw.length() != 13) return false;
    for (char c : raw) {
        if (c < '0' || c > '9') return false;
    }
    // Pakistani CNIC: first 5 digits = province+district code (cannot be 00000)
    if (raw.substr(0, 5) == "00000") return false;
    // Last digit cannot be 0 (check digit is 1-9)
    if (raw[12] == '0') return false;
    return true;
}

// Reads and returns CNIC in formatted "XXXXX-XXXXXXX-X" form
string readCNIC() {
    string val;
    while (true) {
        getline(cin, val);
        // Trim whitespace
        while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
        while (!val.empty() && (val.back() == ' ' || val.back() == '\r')) val.pop_back();

        string raw = stripCNIC(val);

        if (raw.length() != 13) {
            cout << "Invalid. Enter CNIC as XXXXX-XXXXXXX-X (e.g. 35201-1234567-1): ";
            continue;
        }
        bool onlyDigits = true;
        for (char c : raw) {
            if (c < '0' || c > '9') { onlyDigits = false; break; }
        }
        if (!onlyDigits) {
            cout << "Invalid. CNIC digits only, format XXXXX-XXXXXXX-X: ";
            continue;
        }
        if (raw.substr(0, 5) == "00000") {
            cout << "Invalid. First 5 digits cannot be 00000: ";
            continue;
        }
        if (raw[12] == '0') {
            cout << "Invalid. Last digit cannot be 0: ";
            continue;
        }
        // Always store in formatted form
        return formatCNIC(raw);
    }
}

int readInt() {
    int val;
    while (!(cin >> val)) {
        cin.clear();
        string dummy;
        getline(cin, dummy);
        cout << "Invalid. Please enter an integer: ";
    }
    flushInput();
    return val;
}

int readPositiveInt() {
    int val;
    while (true) {
        val = readInt();
        if (val > 0) break;
        cout << "Invalid. Please enter a positive integer (> 0): ";
    }
    return val;
}

double readDouble() {
    double val;
    while (!(cin >> val)) {
        cin.clear();
        string dummy;
        getline(cin, dummy);
        cout << "Invalid. Please enter a floating value: ";
    }
    flushInput();
    return val;
}

double readPositiveDouble() {
    double val;
    while (true) {
        val = readDouble();
        if (val > 0.0) break;
        cout << "Invalid. Please enter a positive value (> 0): ";
    }
    return val;
}

string simpleHash(const string& str) {
    unsigned long hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return to_string(hash);
}

bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.rfind('.');
    if (atPos == string::npos || dotPos == string::npos) return false;
    if (atPos == 0 || dotPos == email.length() - 1 || dotPos < atPos + 2) return false;
    return true;
}

bool isValidPhone(const string& phone) {
    if (phone.empty()) return false;
    size_t start = 0;
    if (phone[0] == '+') start = 1;
    if (phone.length() - start < 7 || phone.length() - start > 15) return false;
    for (size_t i = start; i < phone.length(); ++i) {
        if (phone[i] < '0' || phone[i] > '9') return false;
    }
    return true;
}

string readContactInfo() {
    string contact;
    while (true) {
        cout << "Enter Contact Info (Email or Phone Number): ";
        contact = readString();
        if (isValidEmail(contact) || isValidPhone(contact)) {
            break;
        }
        cout << "Invalid contact format. Please input a valid Email (name@domain.com) or Phone (7-15 digits): " << endl;
    }
    return contact;
}

bool isValidID(const string& id) {
    if (id.empty()) return false;
    for (char c : id) {
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) {
            return false;
        }
    }
    return true;
}

string readValidID() {
    while (true) {
        string id = readString();
        if (isValidID(id)) {
            return id;
        }
        cout << "Invalid ID. Use alphanumeric characters and hyphens only: ";
    }
}

DynamicArray<string> splitString(const string& str, char delimiter) {
    DynamicArray<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty() && token.back() == '\r') {
            token.pop_back();
        }
        tokens.push_back(token);
    }
    return tokens;
}
