#include <bits/stdc++.h>
using namespace std;

// Clever ideas:
// Put missing values in set, so check for largest span is fast

#include <bits/stdc++.h>
using namespace std;

enum class Dir { HORIZONTAL, VERTICAL };

class SquareGrid {
  int n;
  // Stores missing edges
  vector<set<int>> h, v;

  bool complete(const set<int>& s, int l, int r) {
    return *s.lower_bound(l) > r;
  }
  pair<int, int> max_span(const set<int>& s, int x) {
    auto it = s.lower_bound(x);
    int l = *prev(it) + 1;
    int r = *it - 1;
    return {l, r};
  }

  // (i, j) is top-left lattice point, len is edge length
  bool squareComplete(int i, int j, int len) {
    return complete(h[i], j, j+len-1) &&
           complete(h[i+len], j, j+len-1) &&
           complete(v[j], i, i+len-1) &&
           complete(v[j+len], i, i+len-1);
  }

  bool addHorizontal(int i, int j) {
    auto& s = h[i];

    if(!s.contains(j)) return false;
    s.erase(j);
    auto [l, r] = max_span(s, j);

    for(int len = 1; len <= r - l + 1; ++len) {
      int l = max(l, j - len + 1);
      int r = min(j, r - len + 1);
      for(int k = l; k <= r; ++k) {
        if(i - len >= 0 && squareComplete(i - len, k, len))
          return true;
        if(i + len < n && squareComplete(i, k, len))
          return true;
      }
    }
    return false;
  }

public:
  SquareGrid(int n) : n(n), h(n), v(n) {
    set<int> s;
    for(int i = -1; i < n; ++i)
      s.insert(i);

    h.assign(n, s);
    v.assign(n, s);
  }

  bool addStick(int i, int j, Dir dir) {
    if(dir == Dir::HORIZONTAL) return addHorizontal(i, j);
    swap(i, j);
    swap(h, v);
    bool res = addHorizontal(i, j);
    swap(h, v);
    return res;
  }
};
