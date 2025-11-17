#include <bits/stdc++.h>
#include <memory>

/*
Don't forget to use ::operator new and ::operator delete
Don't forget to cast from void* to T*
Use copy and swap for copy assignment
Destroy on pop back

template<typename T>
class vector {
  T* a;
  int n; // Should be size_t
  int cap;

  void resize(int m) {
    copy using operator new, construct_at with move
    delete old using destroy_at and delete
    reassign variables
  }
  void expandSize() {
    wrapper over resize
  }

public:
  constructors
  destructor
  copiers, movers (don't forget to properly destroy before setting)

  push_back() (multiple args)
  getSize()
  at()
  getCapacity()
}
*/

template<typename T>
class vector {
  T* a = nullptr;
  int n = 0;
  int cap = 0;
  
  void resize(int m)  {
    T* b = static_cast<T*>(::operator new(m * sizeof(T)));
    int cnt = std::min(n, m);
    for(int i = 0; i < cnt; i++) {
      std::construct_at(b + i, a[i]);
    }
    cleanup();
    a = b;
    n = cnt;
    cap = m;
  }

  void increaseSize() {
    resize(n == 0 ? 1 : 2 * n);
  }

  void cleanup() {
    for(int i = 0; i < n; i++) {
      std::destroy_at(a + i);
    }
    ::operator delete(a);
  }

public:
  vector() = default;
  vector(int n, const T& x = T()) : n(n), cap(n) {
    a = static_cast<T*>(::operator new(n * sizeof(T)));
    for(int i = 0; i < n; i++) {
      std::construct_at(a + i, x);
    }
  }
  ~vector() {
    cleanup();
  }

  vector(const vector& v) : n(v.n), cap(v.cap) {
    a = static_cast<T*>(::operator new(cap * sizeof(T)));
    for(int i = 0; i < n; i++) {
      std::construct_at(a + i, v.a[i]);
    }
  }
  vector& operator=(const vector& v) {
    if(this != &v) {
      vector temp(v);
      swap(temp);
    }
    return *this;
  }

  vector(vector&& v) : a(v.a), n(v.n), cap(v.cap) {
    v.a = nullptr;
    v.n = 0;
    v.cap = 0;
  }
  vector& operator=(vector&& v) noexcept {
    if(this != &v) {
      cleanup();
      a = v.a; n  = v.n; cap = v.cap;
      v.a = nullptr; v.n = v.cap = 0;
    }
    return *this;
  }
  void swap(vector& other) noexcept {
    std::swap(a, other.a);
    std::swap(n, other.n);
    std::swap(cap, other.cap);
  }

  T& at(size_t i) {
    assert(i >= 0 && i < n);
    return a[i];
  }

  void push_back(const T& x) {
    if(n == cap) increaseSize();
    std::construct_at(a + n, x);
    n++;
  }

  template<class... Args>
  T& emplace_back(Args&&... args) {
    if(n==cap) increaseSize();
    std::construct_at(a+n, std::forward<Args>(args)...);
    return a[n++];
  }

  T pop_back() {
    assert(n != 0);
    T temp = std::move(a[n-1]);
    std::destroy_at(a + (n-1));
    n--;
    return temp;
  }

  size_t size() {
    return n;
  }
  size_t capacity() {
    return cap;
  }
  void shrink_to_fit() {
    resize(n);
  }
};


// struct complexObject {
//   int *a;
//   std::string b;
//   complexObject(int x, std::string y) {
//     a = new int(x);
//     b = y;
//   }
//   friend std::ostream &operator<<(std::ostream &os, const complexObject &o) {
//     return os << *o.a << ", " << o.b;
//   }
// };
//
// int main() {
//   ricky::vector<complexObject> v;
//   v.print();
//   v.push_back(complexObject(1, "a"));
//   v.push_back(complexObject(1, "b"));
//   v.print();
//   v.pop_back();
//   v.push_back(complexObject(2, "a"));
//   complexObject c(2, "c");
//   v.push_back(c);
//   v.print();
// }
