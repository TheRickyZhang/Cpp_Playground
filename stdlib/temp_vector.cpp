//
// Generally do this order
// declare members, big 5, destructor
// push_back -> capacity logic
// grow() + reallocate() functions
// reserve() / destructor()
// finish big 5. Do copies first (straightforward), then copy-and-swap idiom for moves
// exception safety, alignment, self reference in push_back

#include <bits/stdc++.h>
using namespace std;

template<typename T>
class vector {
public:
  vector(int n, const T& t) : sz(n), cap(n) {
    a = static_cast<T*>(::operator new(n * sizeof(T)));
    uninitialized_fill(a, a + sz, t);
  }

  ~vector() {
    if(a) {
      destroy(a, a+sz);
      ::operator delete(a);
    }
  }
  
  void swap(vector& other) {
    swap(a, other.a);
    swap(sz, other.sz);
    swap(cap, other.cap);
  }

  vector(const vector& other) : sz(other.sz), cap(other.cap) {
    a = static_cast<T*>(::operator new(other.cap * sizeof(T)));
    uninitialized_copy(other.a, other.a + other.sz, a);
  }
  vector(vector&& other) {
    vector tmp(other);
    swap(tmp);
    return *this;
  }

  vector& operator=(const vector& other) noexcept {
    if(this == &other) return *this;
    T* na = static_cast<T*>(::operator new(other.cap * sizeof(T)));
    uninitialized_copy(other.a, other.a + other.sz, na);
    a = na;
    sz = cap = other.sz;
  }
  // Direct way: set our a, sz, cap then zero out other
  vector& operator=(vector&& other) noexcept {
    if(this == &other) return *this;
    destroy(a, a+sz);
    ::operator delete(a);
    a = other.a;
    sz = other.sz;
    cap = other.cap;
    other.a = nullptr;
    other.sz = other.cap = 0;
    return *this;
  }
  
  // Handle self-referential case later
  void push_back(const T& x) {
    if(sz >= cap) {
      grow();
    }
    construct_at(a + sz, x);
    sz++;
  }

  size_t capacity() { return cap; }
  
private:
  T* a;
  size_t sz;
  size_t cap;

  void grow() {
    reallocate(cap == 0 ? 1 : cap * 2);
  }
  void reallocate(size_t ncap) {
    T* b = static_cast<T*>(::operator new(ncap * sizeof(T)));  
    uninitialized_move(a, a + sz, b);
    destroy(a, a + sz);
    ::operator delete(a);
    a = b;
    cap = ncap;
  }
};
