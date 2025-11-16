#include <bits/stdc++.h>

#include <cstddef>
#include <vector>
using namespace std;

using Id = size_t;
using Price = long;
using Quantity = int;

class Order {
public:
    Order(Id orderId, Price level, bool isBuy, Quantity quantity) {

    }

private:

};

using Orders = std::vector<Order>;

// DO NOT MODIFY.
struct Trade {
    Id OrderIdA;
    Id OrderIdB; // Aggressor's OrderId
    Id AggressorOrderId;
    bool AggressorIsBuy;
    Price Level;
    Quantity Size;
};

using Trades = std::vector<Trade>;


class Orderbook {
public:
    Trades AddOrder(const Order& order) {
        return { };
    }

    void CancelOrder(Id orderId) {

    }

private:

};
