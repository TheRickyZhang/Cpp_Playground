#include <bits/stdc++.h>
using namespace std;

/*
 * What we are focusing on in grocery store design:
 *
 * Item management: keep track of prices and inventory of items
 * Checkout: Calculate total price of all bought items, including discounts, taxes, etc, and handling payment
 * Keep everything in basis points to avoid floating operations
 *
 */

/*
 * Areas for improvement:
 *
 * Globad productMap, tight coupling with checkoutLine: should instead create a bridge between them, and consider having an interface to allow changes to productMap
 * Probably want a ricker pricing engine and interface if more complex deals arise
 * Units aren't used at all
 *
 * Areas for extension:
 *
 * Concurrency and different types of checkouts
 * Modeling Customers and directing them to fastest checkout
 * Managing inventory
 * Error handling
 */

enum class Unit {
  Piece,
  Kilogram,
  Liter,
};

using Cents = int;

struct Quantity {
  int amount;
  Unit unit;
};

// Pricing Strategies
class PricingStrategy {
protected:
  Cents price;
  explicit PricingStrategy(Cents price) : price(price) {}
public:
  virtual ~PricingStrategy() = default;
  virtual Cents calculatePrice(int amount) = 0;
};

class UnitPricingStrategy : public PricingStrategy {
public:
  UnitPricingStrategy(Cents price) : PricingStrategy(price) {}
  Cents calculatePrice(int amount) {
    return price * amount;
  }
};
class FloatingPricingStrategy : public PricingStrategy {
public:
  FloatingPricingStrategy(Cents price) : PricingStrategy(price) {}
  Cents calculatePrice(int amount) {
    return price * amount / 100;
  }
};
class BOGOPricingStrategy : public PricingStrategy {
public:
  BOGOPricingStrategy(Cents price) : PricingStrategy(price) {}
  Cents calculatePrice(int amount) {
    return (amount + 1) / 2 * price;
  }
};


struct Product {
  int sku;
  string name;
  unique_ptr<PricingStrategy> pricingStrategy;
  Unit unit;
};

struct ShoppingCart {
  map<int, int> products; // sku -> count
  ShoppingCart() = default;
  void addProduct(int sku, int amount = 1) {
    products[sku] += amount;
  }
  void removeProduct(int sku, int amount = 1) {
    auto it = products.find(sku);
    if(it == products.end()) return;
    it->second = max(it->second - amount, 0);
    if(it->second == 0) products.erase(it);
  }
};

class TaxStrategy {
protected:
  float flatRate;
  TaxStrategy(float rate) : flatRate(rate) {};
public:
  virtual ~TaxStrategy() = default;
  virtual Cents calculateTotal(Cents originalTotal) = 0;
};
class DefaultTaxStrategy : public TaxStrategy {
public:
  DefaultTaxStrategy() : TaxStrategy(0.075) {}
  Cents calculateTotal(Cents originalTotal) {
    return static_cast<Cents>(originalTotal * (1 + flatRate));
  }
};

const map<int, Product> productMap = []{
  map<int, Product> m;
  m.emplace(1,Product{1, "Apple", make_unique<FloatingPricingStrategy>(199), Unit::Kilogram});
  m.emplace(2, Product{2, "Bag of Apples", make_unique<UnitPricingStrategy>(499), Unit::Piece});
  m.emplace(3, Product{3, "Carrots", make_unique<BOGOPricingStrategy>(299), Unit::Piece});
  return m;
}();

class checkoutLine {
  // To fill
private:
  queue<ShoppingCart> cartQueue{};
  unique_ptr<TaxStrategy> taxStrategy = nullptr;
public:
  // Want to take ownership of the cart
  explicit checkoutLine(unique_ptr<TaxStrategy> strategy) {
    taxStrategy = std::move(strategy);
  }
  void addCart(ShoppingCart&& cart) {
    cartQueue.push(std::move(cart));
  }
  Cents processCart() {
    if(cartQueue.empty()) return 0;
    const auto& cart = cartQueue.front();
    Cents total = 0;
    for(auto [sku, amount] : cart.products) {
      if(!productMap.count(sku)) {
        // throw error
        return 0;
      }
      const Product& product = productMap.at(sku);
      total += product.pricingStrategy->calculatePrice(amount);
      cout<<"scanned "<<product.name<<": "<<total<<endl;
    }
    cartQueue.pop();
    return taxStrategy->calculateTotal(total);
  }
};


int main () {
  ShoppingCart cart;
  checkoutLine checkout(make_unique<DefaultTaxStrategy>());
  cart.addProduct(1, 150); // 1.50 kg
  cart.addProduct(2, 1);   // 1 bag
  cart.addProduct(3, 4);   // 4 bags
  checkout.addCart(std::move(cart));
  Cents total = checkout.processCart();
  cout<<"total is: "<<total<<" cents"<<endl;
}
