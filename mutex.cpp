// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////
#include <bits/stdc++.h>

enum LOCK_STATE {
  unlocked = 0,
  locked = 1,
};

class Mutex
{
public:
    void lock()
    {
        auto expected = LOCK_STATE::unlocked;
        // Note that compare exchange weak must take the first value by reference since it writes into it if value is not what is expected (returns false)
        if(state.compare_exchange_weak(expected, LOCK_STATE::locked)) {
          return;
        }
        // Hm a bit fragile but arbitrary max wait time until give up
        for(int i = 0; i < 10000; i++) {
          expected = LOCK_STATE::unlocked;
          if(state.compare_exchange_weak(expected, LOCK_STATE::locked)) {
            return;
          }
          wait();
        }
    }

    void unlock()
    {
      state.store(LOCK_STATE::unlocked, std::memory_order_release);
    }
private:
    std::atomic<LOCK_STATE> state = std::atomic<LOCK_STATE>(LOCK_STATE::unlocked);
    static void wait() {
        for(int i = 0; i < 100000; i++) {
        }
        std::this_thread::yield();
    } 
};
