// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

#include <atomic>
#include <bits/stdc++.h>
#include <cstddef>

namespace getcracked {
struct control_block {
  std::atomic<int> count = 1;
};

template <typename T>
class shared_ptr {
public:
  shared_ptr() = default;
  shared_ptr(T *pointer) : p(pointer) { 
    if(p) cb = new control_block();
  }
  ~shared_ptr() {
    decrement(cb, p);
  }

  shared_ptr(const shared_ptr &other) noexcept : p(other.p), cb(other.cb) {
    increment(cb);
  }

  shared_ptr &operator=(const shared_ptr &other) noexcept {
    if(this == &other) return *this;

    // Increment the other first to avoid any conflicts
    increment(other.cb);
    decrement(cb, p);
    p = other.p, cb = other.cb;
    return *this;
  }

  // Can just directly copy since no need to change ref count. std::exchange is nice and concise.
  shared_ptr(shared_ptr &&other) noexcept
    : p(std::exchange(other.p, nullptr)), cb(std::exchange(other.cb, nullptr)) {}

  shared_ptr &operator=(shared_ptr &&other) noexcept {
    if(this == &other) return *this;
    decrement(cb, p);
    p = std::exchange(other.p, nullptr);
    cb = std::exchange(other.cb, nullptr);
    return *this;
  }

  // This looks a bit weird but copy-and-swap pattern is clever concise way to ensure no partial state
  void reset(T *pointer) {
    if(p == pointer) return;
    shared_ptr temp(pointer);
    swap(temp);
  }

  size_t get_count() const {
    return cb ? cb->count.load() : 0;
  }
  
  T* get() const {
    return p;
  }
  T *operator->() const {
    return p;
  }
  T &operator*() const {
    return *p;
  }

  operator bool() const noexcept {
    return p != nullptr;
  }
  void swap(shared_ptr& other) {
    std::swap(p, other.p);
    std::swap(cb, other.cb);
  }

private:
  T* p = nullptr;
  control_block* cb = nullptr;

  static void increment(control_block* cb) {
    // memory_order_relaxed means no guarantees besides atomicity
    if(cb) cb->count.fetch_add(1, std::memory_order_relaxed);
  }

  static void decrement(control_block* cb, T* p) {
    if(!cb) return;
    // We want to guarantee we see all writes beforehand for accurate destruction
    // The default sequential consistency is fine for this (slowest / entirely sequential)
    if(cb->count.fetch_sub(1 /*, std::memory_order_seq_cst */) == 1) {
      delete p;
      delete cb;
    }
    p = nullptr;
    cb = nullptr;
  }
};
} // namespace getcracked
