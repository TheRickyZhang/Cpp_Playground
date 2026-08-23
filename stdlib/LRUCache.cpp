#include <bits/stdc++.h>
using namespace std;

// Important : you may think that we can have a only store values, but we need to store the entire <key, value> pair
// Because when evicting the end of the list, you have no idea what key to remove it from the map.
template<typename K = int, typename T = int>
class LRUCache {
  list<pair<K, T>> a;
  map<K, typename list<pair<K, T>>::iterator> mp;
  int n; 
public:
  LRUCache(int cap) : n(cap) { }

  optional<T> get(K key) {
    auto it = mp.find(key);

    if(it == mp.end()) {
      return nullopt;
    } else {
      int val = it->second->second;
      a.push_front({key, val});
      a.erase(it->second);
      mp[key] = a.begin();
      return val;
    }
  }

  void put(K key, T v) {
    auto it = mp.find(key);
    if(it == mp.end()) {
      if(a.size() >= n) {
        int oldKey = a.back().first;
        mp.erase(oldKey);
        a.pop_back();
      }
      a.push_front({key, v});
      mp[key] = a.begin();
    } else {
      a.erase(it->second);
      a.push_front({key, v});
      mp[key] = a.begin();
    }
  }
};
