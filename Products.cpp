#include "Products.h"

using namespace std;

bool DiscountPolicy::isActive() const {
    string today = getCurrentDateTime().substr(0, 10);
    return (today >= startDate && today <= endDate);
}

bool PerishableGrocery::checkExpiry() const {
    string currentDate = getCurrentDateTime().substr(0, 10);
    return (expiryDate < currentDate);
}
