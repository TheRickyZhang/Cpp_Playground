#include <bits/stdc++.h>
using namespace std;

namespace ricky {

// There are also typename KeyEqual and Allocator, but these are the important ones.
template<typename Key, typename T, typename Hash>
class unordered_map {
  struct Bucket {
    enum State : uint8_t {
      Empty, Full, Tomb
    };
    // Align storage for <K, V>
  };

  // We choose open addressing over separate chaining for speed, locality.
  vector<Bucket> mp;
  int n;
  float max_load;
  Hash h;
  int tombs;

  /*
   * resolve key logic:
   * T find(Key k)
   *   int pos = hash(k) % cap;
   *   f(i, cap-1)
   *     j = probe(pos, i);
   *     if(full(j) && key matches)
   *        return mp[j];
   *     else if(empty(j))
   *        return null;
   */

  /*
   * void insert(Key k, T v)
   *    ... resolve key
   *    mp[x] = v;
   *    sz--;
   */

  /*
  * iterator<T> erase(Key k)
  *   ... resolve key
  *   mp[x] = Tomb;
  *   sz--;
  *   tombs++;
  */

  // These will invalidate iterators:
  // Rehash if tombs gets too large proportional to cap
  // Resize if map gets too large, similar to vector.

  // Any single operation or rehash should have strong exception guarantee, so long as hash / KeyEqual are noexcept.
};

};




