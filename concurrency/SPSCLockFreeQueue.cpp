// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////

/* Requirements:
* You cannot use std::mutex or sequential consistency order.
* Queue is fixed capacity and must be O(1)
*/

/*
* Thinking about the design:
* No STL container supports concurrent operations that modify state
* Additionally, Fixed capacity and O(1) operations strongly indicate using a fixed-size buffer
*
* We want to maintain atomic front, back. You do not want size, as that is too coupled, leading to more complexity
* Note that design means that we can only store cap-1 items to disambiguate empty vs full
*
* SPSC is external contract! We assume that whoever is using it will ONLY call push() with one thread, and only pop() with the other thread. This allows for memory_order_relaxed since we can guarantee the ordering is consistent with the variable.
* Think about it like this: a acquire / relax is a sync point. relaxed() is still ordered in own thread, but arbitrary in other threads.
*
* We must handle potential exceptions. Since pop() and push() do construction / assignment, they can only provide basic guarantee. However, if we design this for low-latency/lock-free, we want to prevent exceptions on the hot path, so we should constraint T -> strong guarantee
*
* 
*/

#include <bits/stdc++.h>
using namespace std;


namespace getcracked {
template <typename T>
class SPSCQ {
  static_assert(is_nothrow_destructible_v<T>);

  // Intuitively, you push to the back, and pop from the front!
  // [      front=0   1   2   3   back]
  // This is just more consistent / intuitive logic
  T* a;
  int cap;
  
  // These are at risk of false sharing (repeatedly invalidates same cache line), so we need to align.
  alignas(hardware_destructive_interference_size) atomic<int> front;
  alignas(hardware_destructive_interference_size) atomic<int> back;
 
  // If cap is a power of 2, we can optimize this
  int next(int x) const {
    return (x + 1) % cap;
    // return (x + 1) & (cap-1);
  }
public:
  SPSCQ(size_t capacity) {
    if(capacity < 2 || (capacity & (capacity - 1)) != 0) {
      throw logic_error("Not a power of 2");
    }
    a = static_cast<T*>(::operator new(capacity * sizeof(T)));
    cap = capacity;
    front.store(0, memory_order_relaxed);
    back.store(0, memory_order_relaxed);
  }
  ~SPSCQ() {
    int f = front.load(memory_order_acquire);
    int b = back.load(memory_order_acquire);
    // Be careful here, much easier to iterate from back since front is noninclusive
    for(int i = f; i != b; i = next(i)) {
      std::destroy_at(a + i);
    }
    ::operator delete(a);
  }

  SPSCQ(const SPSCQ&) { throw logic_error("huh"); }
  SPSCQ& operator=(const SPSCQ&) { throw logic_error("huh"); }
  SPSCQ(SPSCQ&&) { throw logic_error("huh"); }
  SPSCQ& operator=(SPSCQ&&) { throw logic_error("huh"); }


  // "Owns" front via unique thread
  bool push(const T &item) noexcept requires is_nothrow_copy_constructible_v<T> {
    int f = front.load(memory_order_acquire);
    int b = back.load(memory_order_relaxed); // This can be relaxed, since was updated in same thread
    if(next(b) == f) {
      return false;
    }
    std::construct_at(a + b, item);
    back.store(next(b), memory_order_release);
    return true;
  }
  bool push(T&& item) noexcept requires is_nothrow_move_constructible_v<T> {
    int f = front.load(memory_order_acquire);
    int b = back.load(memory_order_relaxed);
    if(next(b) == f) {
      return false;
    }
    std::construct_at(a + b, std::move(item));
    back.store(next(b), memory_order_release);
    return true;
  }

  // "Owns" back via unique thread
  // This is a bit unintuitive API, we return success and store the result in a reference
  bool pop(T &item) noexcept requires is_nothrow_move_assignable_v<T> {
    int f = front.load(memory_order_relaxed);
    int b = back.load(memory_order_acquire);
    if(f == b) {
      return false;
    }
    item = std::move(a[f]);
    std::destroy_at(a + f);
    front.store(next(f), memory_order_release);
    return true;
  }

  // The tradeoffs of memory order of these is important: 
  // Relaxed: lowest cost, not accurate
  // Acquire: more likely to be accurate, higher cost
  // Retry until consistent / size counter: exact, but expensive
  [[nodiscard]] bool full() const noexcept { 
    int f = front.load(memory_order_acquire);
    int b = back.load(memory_order_acquire);
    return next(b) == f; 
  }

  [[nodiscard]] size_t size() const noexcept {
    int f = front.load(memory_order_acquire);
    int b = back.load(memory_order_acquire);
    return (b + cap - f) % cap;
  }

  [[nodiscard]] bool empty() const noexcept {
    int f = front.load(memory_order_acquire);
    int b = back.load(memory_order_acquire);
    return f == b;
  }
};
} // namespace getcracked
