#ifndef UTILITIES_H
#define UTILITIES_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <memory>
#include <sstream>
#include <utility>

using namespace std;

#if __cplusplus < 201402L && !defined(_MSC_VER)
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif

template<typename T>
class DynamicArray {
private:
    T* data;
    int size_;
    int cap;

    void grow() {
        int newCap = (cap == 0) ? 4 : cap * 2;
        T* newData = new T[newCap];
        for (int i = 0; i < size_; ++i) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
        cap = newCap;
    }

public:
    DynamicArray() : data(nullptr), size_(0), cap(0) {}

    DynamicArray(const DynamicArray& other) : data(nullptr), size_(0), cap(0) {
        if (other.cap > 0) {
            data = new T[other.cap];
            cap = other.cap;
            for (int i = 0; i < other.size_; ++i) {
                data[i] = other.data[i];
            }
            size_ = other.size_;
        }
    }

    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), size_(other.size_), cap(other.cap) {
        other.data = nullptr;
        other.size_ = 0;
        other.cap = 0;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            data = nullptr;
            size_ = 0;
            cap = 0;
            if (other.cap > 0) {
                data = new T[other.cap];
                cap = other.cap;
                for (int i = 0; i < other.size_; ++i) {
                    data[i] = other.data[i];
                }
                size_ = other.size_;
            }
        }
        return *this;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size_ = other.size_;
            cap = other.cap;
            other.data = nullptr;
            other.size_ = 0;
            other.cap = 0;
        }
        return *this;
    }

    ~DynamicArray() { delete[] data; }

    int size() const { return size_; }
    bool empty() const { return size_ == 0; }

    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }

    T& back() { return data[size_ - 1]; }
    const T& back() const { return data[size_ - 1]; }

    void push_back(const T& v) {
        if (size_ == cap) grow();
        data[size_++] = v;
    }

    void push_back(T&& v) {
        if (size_ == cap) grow();
        data[size_++] = std::move(v);
    }

    void erase(int index) {
        if (index < 0 || index >= size_) return;
        for (int i = index; i < size_ - 1; ++i) {
            data[i] = std::move(data[i + 1]);
        }
        --size_;
    }

    void clear() {
        delete[] data;
        data = nullptr;
        size_ = 0;
        cap = 0;
    }

    T* begin() { return data; }
    T* end() { return data + size_; }
    const T* begin() const { return data; }
    const T* end() const { return data + size_; }
};

inline string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm ltm;
#if defined(_MSC_VER)
    localtime_s(&ltm, &now);
#else
    tm* ltm_ptr = localtime(&now);
    if (ltm_ptr) {
        ltm = *ltm_ptr;
    }
    else {
        unsigned char* zp = reinterpret_cast<unsigned char*>(&ltm);
        for (size_t i = 0; i < sizeof(tm); ++i) zp[i] = 0;
    }
#endif
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ltm);
    return string(buf);
}

string trim(const string& str);

bool isAllAlphaOrSpace(const string& str);
void flushInput();
string readString();
string readAlphaString();
bool isValidDate(const string& str);
bool isDateBeforeOrEqual(const string& date1, const string& date2);
string readDate();
int parseYear(const string& dateStr);
string addMonthsToDate(const string& dateStr, int months);
bool validateAgeAtJoining(const string& dob, const string& joinDate);
bool validateDOBAge(const string& dob);
string readPassword();
string readCNIC();          // returns formatted XXXXX-XXXXXXX-X
string formatCNIC(const string& raw);   // "3520112345671" -> "35201-1234567-1"
string stripCNIC(const string& formatted); // "35201-1234567-1" -> "3520112345671"
int readInt();
int readPositiveInt();
double readDouble();
double readPositiveDouble();
string simpleHash(const string& str);
bool isValidEmail(const string& email);
bool isValidPhone(const string& phone);
string readContactInfo();
bool isValidID(const string& id);
string readValidID();
bool isValidCNIC(const string& str);    // accepts "3520112345671" OR "35201-1234567-1"

DynamicArray<string> splitString(const string& str, char delimiter);

#endif
