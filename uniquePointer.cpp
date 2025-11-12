#include <bits/stdc++.h>
using namespace std;

namespace ricky {
template <typename T> struct custom_deleter {
  void operator()(T *p) const noexcept { delete p; }
};

template <typename T, typename D = custom_deleter<T>> class unique_ptr {
public:
  // Big 5 - Constructors, assignments, destructor. All COPIES are deleted.
  unique_ptr() = default;
  unique_ptr(T *p) : p(p) {}
  unique_ptr(T *p, const D& d) : p(p), d(d) {}

  unique_ptr(const unique_ptr& other) = delete;
  unique_ptr &operator=(const unique_ptr& other) = delete;;

  unique_ptr(unique_ptr&& other) : p(other.p), d(std::move(other.d)) {
    other.p = nullptr;
  }
  unique_ptr& operator=(unique_ptr&& other) {
    // Don't forget to check for self here, and call reset
    if(this != &other) {
      reset();
      p = other.p;
      d = std::move(other.d);
      other.p = nullptr;
    }
    return *this;
  }

  ~unique_ptr() {
    if(p) d(p);
  }

  T* release() {
    T* copy = p;
    p = nullptr;
    return copy;
  }
  
  // Sort of like the copy constructor
  void reset(T* np) {
    if(p) d(p);
    p = np;
  }

  // Operators to access. Don't forget bool!
  T& operator*() const { return *p; }
  T* operator->() const { return p; }
  operator bool() const { return p != nullptr; }

private:
/*
 * Note we mark d as [[no_unique_address]] to take advantage of Empty Base Optimization (EBO)
 * since d has no state, is not virtual, and not final.
 * Here, we only store 8 bytes, instead of 8 + 1 (+7 for alignment) that it would take otherwise
 */
  T *p = nullptr;
  [[no_unique_address]] D d;
};

} // namespace ricky
