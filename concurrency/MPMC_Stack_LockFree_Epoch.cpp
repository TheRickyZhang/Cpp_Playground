#include <bits/stdc++.h>
using namespace std;

using mo = std::memory_order;

template <typename T>
class LockFreeStack {
private:
  static constexpr size_t MaxThreads = 256;
  static constexpr size_t RetireThreshold = 64;

  struct Node {
    T val;
    Node* next = nullptr;

    template <typename... Args>
    explicit Node(Args&&... args) : val(std::forward<Args>(args)...) {}
  };

  struct ThreadRecord {
    std::atomic<bool> claimed{false};
    std::atomic<bool> active{false};
    std::atomic<size_t> epoch{0};
  };

  struct RetiredNode {
    Node* node = nullptr;
    size_t epoch = 0;
    RetiredNode* next = nullptr;
  };

  inline static std::array<ThreadRecord, MaxThreads> threads{};
  inline static std::atomic<size_t> global_epoch{0};

  std::atomic<Node*> head{nullptr};

  // Retired nodes for this stack instance.
  std::atomic<RetiredNode*> retired_head{nullptr};
  std::atomic<size_t> retired_count{0};

private:
  static ThreadRecord* claim_thread_record() {
    for (auto& r : threads) {
      bool expected = false;

      if (r.claimed.compare_exchange_strong(
              expected,
              true,
              mo::acq_rel,
              mo::relaxed)) {
        return &r;
      }
    }

    throw std::runtime_error("too many threads");
  }

  struct ThreadData {
    ThreadRecord* rec = nullptr;

    ThreadData() : rec(claim_thread_record()) {}

    ~ThreadData() {
      if (!rec) return;

      rec->active.store(false, mo::seq_cst);
      rec->claimed.store(false, mo::release);
    }

    ThreadData(const ThreadData&) = delete;
    ThreadData& operator=(const ThreadData&) = delete;
  };

  static ThreadData& thread_data() {
    thread_local ThreadData data;
    return data;
  }

  class EpochGuard {
    ThreadRecord* rec;

  public:
    explicit EpochGuard(ThreadRecord* rec) : rec(rec) {
      size_t e = global_epoch.load(mo::seq_cst);

      // Publish epoch before becoming active.
      rec->epoch.store(e, mo::seq_cst);
      rec->active.store(true, mo::seq_cst);
    }

    ~EpochGuard() {
      rec->active.store(false, mo::seq_cst);
    }

    EpochGuard(const EpochGuard&) = delete;
    EpochGuard& operator=(const EpochGuard&) = delete;
  };

  static bool can_advance_epoch(size_t curr) {
    for (auto& r : threads) {
      if (!r.claimed.load(mo::seq_cst)) {
        continue;
      }

      if (!r.active.load(mo::seq_cst)) {
        continue;
      }

      if (r.epoch.load(mo::seq_cst) != curr) {
        return false;
      }
    }

    return true;
  }

  static void try_advance_epoch() {
    size_t curr = global_epoch.load(mo::seq_cst);

    if (can_advance_epoch(curr)) {
      global_epoch.compare_exchange_strong(
          curr,
          curr + 1,
          mo::seq_cst,
          mo::seq_cst
      );
    }
  }

  static bool safe_to_delete(size_t retire_epoch, size_t curr_epoch) {
    return curr_epoch >= retire_epoch + 2;
  }

  void push_retired_record(RetiredNode* r) {
    RetiredNode* old = retired_head.load(mo::relaxed);

    do {
      r->next = old;
    } while (!retired_head.compare_exchange_weak(
        old,
        r,
        mo::release,
        mo::relaxed
    ));
  }

  void scan_retired() {
    try_advance_epoch();

    RetiredNode* list = retired_head.exchange(nullptr, mo::acquire);
    size_t curr = global_epoch.load(mo::seq_cst);

    while (list) {
      RetiredNode* r = list;
      list = list->next;

      if (safe_to_delete(r->epoch, curr)) {
        delete r->node;
        delete r;
        retired_count.fetch_sub(1, mo::relaxed);
      } else {
        push_retired_record(r);
      }
    }
  }

  void retire(Node* n) {
    size_t e = global_epoch.load(mo::seq_cst);
    RetiredNode* r = new RetiredNode{n, e, nullptr};

    push_retired_record(r);

    size_t old_count = retired_count.fetch_add(1, mo::relaxed);

    if (old_count + 1 >= RetireThreshold) {
      scan_retired();
    }
  }

  void force_delete_all_retired_after_threads_stopped() {
    RetiredNode* list = retired_head.exchange(nullptr, mo::relaxed);

    while (list) {
      RetiredNode* r = list;
      list = list->next;

      delete r->node;
      delete r;
    }

    retired_count.store(0, mo::relaxed);
  }

public:
  LockFreeStack() = default;

  LockFreeStack(const LockFreeStack&) = delete;
  LockFreeStack(LockFreeStack&&) = delete;
  LockFreeStack& operator=(const LockFreeStack&) = delete;
  LockFreeStack& operator=(LockFreeStack&&) = delete;

  ~LockFreeStack() {
    // Assumes no concurrent push/pop.
    Node* h = head.load(mo::relaxed);

    while (h) {
      Node* next = h->next;
      delete h;
      h = next;
    }

    // Safe only after all worker threads using this stack have stopped.
    force_delete_all_retired_after_threads_stopped();
  }

  template <typename... Args>
  void push(Args&&... args) {
    Node* n = new Node(std::forward<Args>(args)...);

    // Push also enters the epoch. It may hold a stale head pointer as its
    // CAS expected value, so EBR must prevent that address from being freed
    // and reused while this push is in progress.
    ThreadData& td = thread_data();
    EpochGuard guard(td.rec);

    Node* h = head.load(mo::relaxed);

    do {
      n->next = h;
    } while (!head.compare_exchange_weak(
        h,
        n,
        mo::release, // publish n->val and n->next
        mo::relaxed  // failure only updates h; we do not dereference it
    ));
  }

  bool pop(T& out) {
    ThreadData& td = thread_data();
    EpochGuard guard(td.rec);

    while (true) {
      Node* h = head.load(mo::acquire);

      if (!h) {
        return false;
      }

      // Safe under EBR: while this thread is active, h cannot be reclaimed.
      Node* next = h->next;

      if (head.compare_exchange_weak(
          h,
          next,
          mo::acquire,
          mo::relaxed
      )) {
        try {
          out = std::move(h->val);
        } catch (...) {
          retire(h);
          throw;
        }

        retire(h);
        return true;
      }

      // CAS failed. Retry while still inside the epoch critical section.
    }
  }
};

int main() {
  LockFreeStack<int> s;
  thread t1([&]() { s.push(1); });
  int res{};
  thread t2([&]() { s.pop(res); });
  t1.join();
  t2.join();
}
