#include <bits/stdc++.h>
using namespace std;


template<typename T>
class MPMCBlockingQueue {
  mutex m;
  T* a;
  int cap;
  int head = 0;
  int tail = 0;
  condition_variable not_empty, not_full;
  bool done = false;

  int next(int x) {
    return (x + 1) % cap;
  }
  bool empty() { return head == tail; }
  bool full() { return next(tail) == head; }

public:
  MPMCBlockingQueue(int n) : cap(n) {
    a = static_cast<T*>(::operator new(n * sizeof(T))); 
  }
  ~MPMCBlockingQueue() {
    {
      lock_guard<mutex> temp(m); 
      done = true;
      for(int i = head; i != tail; i = next(i)) {
        ::destroy_at(a + i);
      }
      head = tail;
    }
    not_empty.notify_all();
    not_full.notify_all();
    ::operator delete(a);
  }
  MPMCBlockingQueue(const MPMCBlockingQueue&) = delete;
  MPMCBlockingQueue(MPMCBlockingQueue&&) = delete;
  MPMCBlockingQueue& operator=(const MPMCBlockingQueue&) = delete;
  MPMCBlockingQueue& operator=(MPMCBlockingQueue&&) = delete;

  bool push(const T& x) {
    unique_lock lock(m);
    not_full.wait(lock, [&](){ return !full() || done;});
    if(done) return false;

    construct_at(a + tail, x);
    tail = next(tail);

    lock.unlock();
    not_empty.notify_one();
    return true;
  }

  bool pop(T& x) {
    unique_lock lock(m);
    not_empty.wait(lock, [&](){ return !empty() || done;});
    if(done) return false;

    x = std::move(a[head]);
    destroy_at(a + head);
    head = next(head);

    lock.unlock();
    not_full.notify_one();
    return true;
  }
};



template<typename T>
class BoundedBlockingQueue {
  mutex m;
  condition_variable cv_notempty, cv_notfull;

  // Use for simplicity, but would be better to model as fixed-size ring buffer.
  // 
  deque<T> q;
  int cap;
  bool done = false;

public:
  BoundedBlockingQueue(int n) : cap(n) { }

  // Important to understand the necessity of this to prevent threads from blocking forever
  // Closed = insta-shut everything. Should notify_all() to unblock all threads
  void close() {
    {
      lock_guard<mutex> lock(m);
      done = true;
    }
    cv_notempty.notify_all();
    cv_notfull.notify_all();
  }
 
  // Can also consider implementing try_push(), try_pop() that don't block, for low-latency purposes.
  // Avoids context switches and sleeping.
  //

  bool push(const T& x) {
    unique_lock lock(m);
    // Let's first wait on the direct not full condition.
    // Internally, wait() releases m, then records interally that we are waiting on cv_notfull and blocks. Once unblocked, we relock m.
    cv_notfull.wait(lock, [&](){q.size() < cap || done; });
    if(done) return false;
    
    // Now we know it's not full, so push back. This will directly update, no need to adjust any flags.
    q.push_back(x);
    lock.unlock();
    // We now notify, since now there shoudl be ability for pop() to work
    cv_notempty.notify_one();
    return true;
  }

  bool push(T&& x) {
    unique_lock lock(m);
    cv_notfull.wait(lock, [&](){q.size() < cap || done; });
    if(done) return false;
    
    q.push_back(std::move(x));
    lock.unlock();
    cv_notempty.notify_one();
    return true;
  }

  bool pop(T& x) {
    unique_lock lock(m);
    cv_notempty.wait(lock, [&](){q.size() > 0 || done; });
    // Note this is a slightly different condition, because when we are done we still want to pop until there are no more items in the queue
    if(done && q.empty()) return false;

    x = std::move(q.front());
    q.pop_front();
    lock.unlock();
    cv_notfull.notify_one();
    return true;
  }
};

// Semaphore design
// Overall, a bit more complex to manage
// No built-in "notify-all" shutdown equivalent.

// template<typename T>
// class BoundedBlockingQueueSemaphore {
//   ...
//   atomic<bool> done = false;
//   std::counting_semaphore<> spaces;
//   std::counting_semaphore<> items;
//
//   bool push(const T& x) {
//     while(true) {
//       if(done) return false;
//       if(spaces.try_acquire_for(1)) break;
//     }
//     lock_guard {
//
//     }
//     items.release();
//     return true;
//   }
// }
