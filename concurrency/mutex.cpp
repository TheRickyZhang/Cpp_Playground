// Write your solution here
// C++20 for C++
// /////////////////////////////////////////////////////////////////////////
// If C++: Your code is automatically compiled with a precompiled header. //
// 99% of includes / packages are already added for you.                  //
// You do NOT need to add your own includes here.                         //
// /////////////////////////////////////////////////////////////////////////
#include <bits/stdc++.h>
using namespace std;

enum LOCK_STATE {
  unlocked = 0,
  locked = 1,
};

class Mutex
{
public:
    void lock()
    {
      // Note how we can do this concisely by doing a short-circuit on load == unlocked, and then just invoking exchange
      for(int i = 0; (state.load(std::memory_order_relaxed) == LOCK_STATE::locked) ||
        state.exchange(LOCK_STATE::locked, std::memory_order_acquire); i++) {
        if(i != 0 && i % 8 == 0) {
          this_thread::yield();
        }
      }
    }

    void unlock()
    {
      state.store(LOCK_STATE::unlocked, std::memory_order_release);
    }
private:
    std::atomic<LOCK_STATE> state = std::atomic<LOCK_STATE>(LOCK_STATE::unlocked);
};
