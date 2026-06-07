#include <atomic>
#include <bits/stdc++.h>
using namespace std;

/*
* A shared pointer basically manages the underlying lifetime of a pointer so we can share the same resource
* A simplified version might might store the pointer in the control block, but we should be careful that the point is to have multiple independent owners, NOT avoid repetitive pointer storing.
* Note it is possible to have aliasing. For instance, if we do: struct Foo { int x; }, we can set:
* p = shared_ptr<Foo>(1);
* q = shared_ptr<int>(p, &p.x); -> q contributes to p's pointer lifetime, but will return the corresponding int* instead of Foo*
* Thus an ideal design would store T* per object, not in the control block
* First, implement functions assuming we have increment() and decrement(). Remember to prefer incrementing before decrementing to avoid lifetime issues
* Then implement increment / decrement. For thread safety, 
*
*/

struct control_block {
  std::atomic<int> count = 1;
  std::atomic<int> weakCount = 1;
};

template <typename T>
class shared_ptr {
  T* p = nullptr;
  control_block* cb = nullptr;

  void increment() {
    // memory_order_relaxed means no guarantees besides atomicity
    if(cb) cb->count.fetch_add(1, std::memory_order_relaxed);
  }

  // Be careful - you need pointer references here to assign the actual address to nullptr
  void decrement() {
    if(!cb) return;
    // 
    if(cb->count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete p;
      delete cb;
    }
    p = nullptr;
    cb = nullptr;
  }

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
  // Can just directly copy since no need to change ref count. std::exchange is nice and concise.
  shared_ptr(shared_ptr &&other) noexcept
    : p(std::exchange(other.p, nullptr)), cb(std::exchange(other.cb, nullptr)) {
  }

  shared_ptr &operator=(const shared_ptr &other) noexcept {
    if(this == &other) return *this;

    // Increment the other first to avoid any conflicts
    increment(other.cb);
    decrement(cb, p);
    p = other.p, cb = other.cb;
    return *this;
  }
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
};
