#include <atomic>
#include <bits/stdc++.h>
using namespace std;

// Minimal implementation
struct SpinLock {
  atomic_flag f = ATOMIC_FLAG_INIT;
  void lock() {
    // while(f.test_and_set(memory_order_acquire)) {
    //   sleep(1);
    // }

    // Better growing hueristic for waiting
    int spins = 0;
    while(f.test_and_set(memory_order_acquire)) {
      if(spins < 16) {
        // f(i, 1LL<<spins) pause();
        spins++;
      } else {
        this_thread::yield();
      }
    }
  }
  void unlock() {
    f.clear(memory_order_release);
  }
};
