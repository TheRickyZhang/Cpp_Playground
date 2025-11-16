#include <bits/stdc++.h>
using namespace std;

using Id = size_t;
using Price = long;
using Quantity = int;

/* Follow ups
 *
 * In a real trading system, to optimize while still keeping requirements of price order, FIFO creation order, and fast deletion by ID, you could use a pool of Orders, each with their own intrusive next*, prev* pointers to minimize std::list<> overhead
 *
 * Additionally, you can move from a map to indices by ticker group.
 */

class Order {
public:
    Order(Id orderId, Price price, bool isBuy, Quantity quantity) :
      orderId(orderId), price(price), isBuy(isBuy), quantity(quantity) {
        
      }
    Id OrderId() const {
      return orderId;
    }
    Price GetPrice() const {
      return price;
    }
    bool IsBuy() const {
      return isBuy;
    }
    Quantity getQuantity() const {
      return quantity;
    }
    // Returns the amount matched
    Quantity match(Quantity& q) {
      // assert(q >= 0);
      int amount = min(quantity, q);
      quantity -= amount;
      q -= amount;
      return amount;
    }
    void setQuantity(Quantity q) {
      quantity = q;
    }
    // This does not belong here, because fundamentally the order matching logic should reside in the Order Book
    // bool matchWith(Order other) { }
private:
  Id orderId;
  Price price;
  bool isBuy;
  Quantity quantity;
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
      if(idToIterator.count(order.OrderId())) {
        // cout << "duplicate order id" << endl;
        return {};
      }
      Trades res;
      Quantity remaining = order.getQuantity();
      if(order.IsBuy()) {
        matchOrders(asks, order, res);
      } else {
        matchOrders(bids, order, res);
      }
      return res;
    }
    void CancelOrder(Id orderId) {
      auto it = idToIterator.find(orderId);
      if (it == idToIterator.end()) {
        return;
      }
      auto listIt = it->second;
      idToIterator.erase(it);

      Price p = listIt->GetPrice();
      if (listIt->IsBuy()) {
        auto lvl = bids.find(p);
        if (lvl != bids.end()) {
          lvl->second.erase(listIt);
          if (lvl->second.empty()) bids.erase(lvl);
        }
      } else {
        auto lvl = asks.find(p);
        if (lvl != asks.end()) {
          lvl->second.erase(listIt);
          if (lvl->second.empty()) asks.erase(lvl);
        }
      }
    }

private:
  /* IMPORTANT
   * Fundamentally, we need to be able to access orders based on price and FIFO time submitted, as well as delete by ID.
   * The only way to support all of these fast (<= log n) is to map id -> iterator, which requires that the iterator not be invalidated from insertions and other deletions.
   * list<> works, as bidirectional linked list guarantees valid iterators, and has fast deletion. So we make the bids, asks a map<Price, list<Order>>.
   *
   * There is performance impact from using list<> pointer indirection though.
   */
  using bidsMap = map<Price, list<Order>, std::greater<Price>>;
  using asksMap = map<Price, list<Order>>;
  // Sorted so that best prices are first, in .begin()
  bidsMap bids;
  asksMap asks;
  map<Id, list<Order>::iterator> idToIterator;

  template<class M>
  void matchOrders(M& side, const Order& o, Trades& res) requires (same_as<M, bidsMap> || same_as<M, asksMap>) {
    auto stopCondition = [&](Price p) {
      if constexpr (same_as<M, asksMap>) {
        return p > o.GetPrice();
      } else {
        return p < o.GetPrice();
      }
    };
    // Destructure order
    Id id = o.OrderId();
    bool isBuy = o.IsBuy();
    Price price = o.GetPrice(); 
    Quantity remaining = o.getQuantity();

    for(auto mapIt = side.begin(); mapIt != side.end() && remaining > 0; mapIt++) {
      if(remaining <= 0) break;
      auto& [currPrice, orders] = *mapIt;
      if(stopCondition(currPrice)) {
        break;
      }

      for(auto it = orders.begin(); it != orders.end() && remaining > 0; ) {
        // Is this a good API? I feel like it could be confusing
        Quantity amount = it->match(remaining);

        // Destructure resting order
        Id restingId = it->OrderId();
        Price restingPrice = it->GetPrice();
        res.push_back(Trade{
          restingId,
          id,
          id,
          isBuy,
          price,
          amount
        });
        if(it->getQuantity() == 0) {
          idToIterator.erase(restingId);
          it = orders.erase(it);
        } else {
          it++;
        }
      }
    }
    if(remaining != 0) {
      Order copy(id, price, isBuy, remaining);
      if constexpr (same_as<M, bidsMap>) {
        asks[price].push_back(copy);
        idToIterator[id] = prev(asks[price].end());
      } else {
        bids[price].push_back(copy);
        idToIterator[id] = prev(bids[price].end());
      }
    }
  }
};

