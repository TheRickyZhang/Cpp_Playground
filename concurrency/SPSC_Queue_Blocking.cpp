#include <bits/stdc++.h>
using namespace std;

template <typename T>
class BlockingSPSCQueue {
  mutex m;
  condition_variable not_full, not_empty;
  deque<T> q;
  size_t cap;
  bool done = false;

public:
  explicit BlockingSPSCQueue(size_t n) : cap(n) {
    if(n == 0) throw runtime_error("capacity must be positive");
  }
  BlockingSPSCQueue(const BlockingSPSCQueue&) = delete;
  BlockingSPSCQueue(BlockingSPSCQueue&&) = delete;
  BlockingSPSCQueue& operator=(const BlockingSPSCQueue&) = delete;
  BlockingSPSCQueue& operator=(BlockingSPSCQueue&&) = delete;

  bool push(const T& x) {
    {
      unique_lock lock(m);
      not_full.wait(lock, [&]{
        return q.size() < cap || done;
      });
      if(done) return false;
      q.push(x);
    }
    not_empty.notify_one();
    return true;
  }

  bool pop(T& x) {
    {
      unique_lock lock(m);
      not_empty.wait(lock, [&]{
        return !q.empty() || done;
      });
      if(done) return false;
      x = std::move(q.front());
      q.pop();
    }
    not_full.notify_one();
    return true;
  }

  void close() {
    {
      lock_guard lock(m);
      done = true;
    }
    not_empty.notify_all();
    not_full.notify_all();
  }
};
