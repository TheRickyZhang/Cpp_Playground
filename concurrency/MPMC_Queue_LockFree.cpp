// /home/ricky/Whitebox/[5, 7)/mpmcqueue.cpp

#include <bits/stdc++.h>

/*

Note this is not exactly a simple expansion of a MPMC lock-free queue.
What a producer might do is:
- I want to find the tail index to write to.
- I will write to the tail index
Without locks, these cannot be atomic together, ie another producer might start
this process in the middle of another. So we separate out "reserving" a state vs
"is there actually data"?

- Initially, you might think to represent liveliness with a
vector<atomic<bool>>. However, lock-free code cannot assume threads run quickly!
ABA problem -> can't tell whether a "free" state belongs to current view. So it
is better to instead track: {what "cycle" are we on, ready for producer /
consumer}, which can be condensed into: {i, i+1, cap+i, cap+i+1, ...}

Let's run down the categories of bounded queue implementations, probably from simplest -> most complex:
- blocking SPSC: Can use mutexes exactly like MPMC, or optimized with counting semaphores
- blocking MPMC: use mutexes, waiting / publishing condition variables of not_empty and not_full.
- Lock-free SPSC: maintain atomic head and tails counters. Can optimized with side-cached state.
- Lock-free sequence MPMC (this): maintains sequence data in addition to head/tail counters.Try reserving with CAS, then update sequence state. Less to optimize.

Compared to SPSC, we need to change a lot of things about this impl.
To deal with multiple producers / consumers, we need to make sure only one of them can "win" at a time, and that our atomicity is preserved considering that all sequential operations can be interrupted by other threads.

The simplest idea is to designate the previous head/tail atomic indices as the "reserved" space, while we have a separate "liveliness state" to formally verify presence with an atomic compare-and-swap. For optimal organization, we put these togethe with data. I don't really feel like writing more so you can see the code to hopefully get the intuition.

Note that there are many other lock-free MPMC designs.

*/

using namespace std;

template <typename T> class MPMCQueue {
public:
  // remember we still need to initialize a; placement new is fine.
  MPMCQueue(size_t n) : cap(check(n)), mask(n - 1), a(new Cell[n]) {
    for (size_t i = 0; i < cap; i++) {
      a[i].seq.store(i, memory_order_relaxed);
    }
  }
  ~MPMCQueue() noexcept {
    size_t h = consumer.head.load(memory_order_relaxed);
    size_t t = producer.tail.load(memory_order_relaxed);
    for(size_t i = h; i < t; i++) {
      Cell& c = a[i & mask];
      if(c.seq.load(memory_order_relaxed) == i+1) {
        destroy_at(c.get());
      }
    }
    delete[] a;
  }

  MPMCQueue(const MPMCQueue&) = delete;
  MPMCQueue& operator=(const MPMCQueue&) = delete;
  MPMCQueue(MPMCQueue&&) = delete;
  MPMCQueue& operator=(MPMCQueue&&) = delete;

  bool push(const T& x) {
    while (true) {
      // since owned by producer, we can load relaxed
      size_t t = producer.tail.load(memory_order_relaxed);
      Cell& c = a[t & mask];
      // For shared cells, must release / acquire
      size_t s = c.seq.load(memory_order_acquire);

      // exact ready state. Note if values are large enough to wrap, would need to use signed differences
      if (s == t) {
        // Need to use CAS for the atomic check of: only one thread gets to "win" the write
        // Is the most straightforward if we need conditional claiming
        if (producer.tail.compare_exchange_weak(t, t + 1,
            memory_order_relaxed, memory_order_relaxed)
        ) {
          construct_at(c.get(), x);
          c.seq.store(t + 1, memory_order_release);
          return true;
        }
      } else if (t > s) {
        // since s has lower seq number, queue has not caught up yet (likely full)
        return false;
      } else {
        continue;
      }
    }
  }

  bool push(T&& x) {
    while (true) {
      size_t t = producer.tail.load(memory_order_relaxed);
      Cell& c = a[t & mask];
      size_t s = c.seq.load(memory_order_acquire);

      if (s == t) {
        if (producer.tail.compare_exchange_weak(
              t, t + 1,
              memory_order_relaxed,
              memory_order_relaxed)) {
          construct_at(c.get(), std::move(x));
          c.seq.store(t + 1, memory_order_release);
          return true;
        }
      } else if (t > s) {
        return false;
      }
    }
  }

  bool pop(T& x) {
    while (true) {
      size_t h = consumer.head.load(memory_order_relaxed);
      Cell& c = a[h & mask];
      size_t s = c.seq.load(memory_order_acquire);
      if (h+1 == s) {
        if (consumer.head.compare_exchange_weak(h, h + 1, memory_order_relaxed,
                                        memory_order_relaxed)) {
          T* res = c.get();
          x = std::move(*res);
          destroy_at(res);
          c.seq.store(h+cap, memory_order_release);
          return true;
        }
      } else if(h+1 > s) {
        // queue is empty
        return false;
      } else {
        continue;
      }
    }
  }

private:
  // Since the data and sequence number are used together, prefer to bundle them
  // rather than separating T* and vector<atomic<size_t>> Since we want to store
  // cells contiguously, prefer inline storage (lives directly inside the
  // object). If we had a T* for each element, we would need to allocate
  // separately, which would scatter memory
  struct Cell {
    alignas(T) unsigned char storage[sizeof(T)];
    atomic<size_t> seq;

    T* get() noexcept { return std::launder(reinterpret_cast<T*>(storage)); }
    const T* get() const noexcept {
      return std::launder(reinterpret_cast<const T*>(storage));
    }
  };
  Cell* a;
  size_t cap;
  size_t mask;

  static constexpr int ALIGN = hardware_destructive_interference_size;

  struct alignas(ALIGN) ProducerState {
    atomic<size_t> tail{0};
  };
  struct alignas(ALIGN) ConsumerState {
    atomic<size_t> head{0};
  };
  ProducerState producer;
  ConsumerState consumer;

  static size_t check(size_t n) {
    if(!(n > 2 && has_single_bit(n))) {
      throw runtime_error("capacity must be power of 2 and greater than 2");
    }
    return n;
  }
};

// int main() {
//   MPMCQueue<int> q(4);
//   int x = 0;
//   q.push(x);
//   q.push(1);
//   q.pop(x);
// }
