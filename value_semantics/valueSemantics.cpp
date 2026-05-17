#include <bits/stdc++.h>
using namespace std;

#define SHOW(expr) (cout << left << setw(24) << #expr, cat(expr))

struct Id {
  int x;
  int operator()() const { return x; }
};

struct Order {
  string ticker = "aaa";
  int price = 100;
  bool isBid = true;
};

// T& → lvalue, T&& → rvalue, const T& → const
void cat(int&)         { cout << "  int&\n"; }
void cat(int&&)        { cout << "  int&&\n"; }
void cat(const int&)   { cout << "  const int&\n"; }
void cat(Id&)          { cout << "  Id&\n"; }
void cat(Id&&)         { cout << "  Id&&\n"; }
void cat(const Id&)    { cout << "  const Id&\n"; }
void cat(Order&)       { cout << "  Order&\n"; }
void cat(Order&&)      { cout << "  Order&&\n"; }
void cat(const Order&) { cout << "  const Order&\n"; }

// ── Return type helpers ──
map<int, Order> mp;
Order  getByVal(int id)  { return mp.at(id); }
Order& getByRef(int id)  { return mp.at(id); }
Order&& getByRRef(int id){ return std::move(mp.at(id)); }


int main() {
goto start;
  // Value Categories (slide 6)
  // named var = lvalue, literal/arithmetic = prvalue, std::move = xvalue
start:
  int x = 0;
  SHOW(x);
  SHOW(0);
  SHOW(x + 1);
  return 0;
  
  // xvalues (slide 9)
  SHOW(std::move(x));
  SHOW(int{});
  
  int a = 42, b = std::move(a);
  cout << "int move: a=" << a << " b=" << b << "\n";
  string s1 = "hello", s2 = std::move(s1);
  cout << "string move: s1=\"" << s1 << "\" s2=\"" << s2 << "\"\n"; // s1 emptied

  Id id{1};
  SHOW(id);
  SHOW(Id{3});
  SHOW(std::move(id));

  // 2. Reference Binding (slide 11)
  // Which of these are not allowed? (r2, r3, rr3)
  int y = 10;
  int& r1 = y;
  int& r2 = 5;
  int& r3 = std::move(y);

  const int& cr1 = y;
  const int& cr2 = 5;
  const int& cr3 = std::move(y);

  int&& rr1 = 5;
  int&& rr2 = std::move(y);
  int&& rr3 = y;

  // Most surprising / unintuitive: named rvalue ref -> int -> will use int& overload
  SHOW(rr1);
  SHOW(std::move(rr1));

  int i = 42;
  SHOW(i);
  SHOW(7);
  SHOW(std::move(i));

  // Return Types & Lifetimes (advanced)
  mp[1] = Order{"AAPL", 150, true};
  mp[2] = Order{"GOOG", 280, false};

  Order copy = getByVal(1);     // independent copy
  copy.ticker = "NFLX";
  cout << "mp[1] after copy mod: " << mp[1].ticker << "\n"; // AAPL

  Order& alias = getByRef(1);   // alias into store (no copy)
  alias.ticker = "GOOG";
  cout << "mp[1] after alias mod: " << mp[1].ticker << "\n"; // GOOG
  mp[1].ticker = "AAPL";

  Order moved = getByRRef(1);   // move-constructs (no copy)
  cout << "moved: " << moved.ticker << "\n";                 // AAPL
  cout << "mp[1] after move: \"" << mp[1].ticker << "\"\n";  // moved-from

  Order&& rref = getByRRef(2);  // no move
  cout << "rref: " << rref.ticker << "\n";                   // GOOG
  SHOW(rref);
  SHOW(std::move(rref));
}
