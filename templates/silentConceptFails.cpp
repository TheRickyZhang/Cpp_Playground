#include "bits/stdc++.h"
using namespace std;

template<typename C>
concept PrintableRange = !is_same_v<decay_t<C>, string> &&
requires(const C& c) {
  begin(c);
  end(c);
} &&
requires(ostream& os, C c) {
  os << *begin(c);
};

template<typename C>
requires PrintableRange<C>
ostream& operator<<(/* const */ ostream& os, C c) {
  for(auto x : c) {
    os << x << " ";
  }
  return os;
}

int main() {
  vector<int> a;
  a.push_back(1);
  a.push_back(3);
  a.push_back(4);
  a.push_back(5);
  a.push_back(8);
  cout << a << endl;
}
