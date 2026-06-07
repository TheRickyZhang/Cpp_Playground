/*
 * What it the main difficulty of this approach?
 * Preventing use-after-free when doing operations on atomics with multiple
 * threads; basically, what hazard pointers solve. If thread 1 does: T* a =
 * p.get(), p.store(other), delete a[], while b interleaves: T* a = get(),
 * f(a->member), NPE
 *
 * Even with that, we need to consider the ABA issue; if our allocator can
 * produce the same address nodes on different instantiations, we might get a
 * case where a p == thing check passes even though the node is different, but
 * the newly allocated happened to have the same address.
 *
 * Two ways of addressing:
 * - Hazard pointers (canonical)
 * - Epoch-based reclamation (courser, wait until every other thread cannot have
 * old pointers)
 *
 */

#include <bits/stdc++.h>
using namespace std;

using mo = memory_order;

template <typename T> class LockFreeStack {
private:
  static constexpr size_t MaxThreads = 256;
  static constexpr size_t ScanThreshold = 2 * MaxThreads;

  struct Node {
    T val;
    Node* next = nullptr;

    template <typename... Args>
    explicit Node(Args&&... args) : val(std::forward<Args>(args)...) {}
  };

  // Not sure on the exact hazard implementation details; theoretically better for learning, but obviously prefer stdlib impl if you have a good base.
  struct HazardRecord {
    std::atomic<bool> active{false};
    std::atomic<Node*> ptr{nullptr};
  };

  struct RetiredNode {
    Node* node = nullptr;
    RetiredNode* next = nullptr;
  };

  inline static std::array<HazardRecord, MaxThreads> hazards{};
  inline static std::atomic<RetiredNode*> retired_head{nullptr};

  std::atomic<Node*> head{nullptr};

private:
  static bool try_claim(HazardRecord& h) {
    bool expected = false;
    return h.active.compare_exchange_strong(expected, true, mo::acq_rel,
                                            mo::relaxed);
  }

  struct HazardOwner {
    HazardRecord* rec = nullptr;

    HazardOwner() {
      for (HazardRecord& h : hazards) {
        if (try_claim(h)) {
          rec = &h;
          return;
        }
      }

      throw std::runtime_error("no free hazard pointer slots");
    }

    ~HazardOwner() {
      if (!rec)
        return;

      rec->ptr.store(nullptr, mo::seq_cst);
      rec->active.store(false, mo::release);
    }

    HazardOwner(const HazardOwner&) = delete;
    HazardOwner& operator=(const HazardOwner&) = delete;
  };

  static HazardRecord& hazard_for_thread() {
    thread_local HazardOwner owner;
    return *owner.rec;
  }

  Node* protect_head(HazardRecord& hz) const {
    while (true) {
      Node* h = head.load(mo::acquire);

      // Publish: "this thread may dereference h."
      hz.ptr.store(h, mo::seq_cst);

      // Recheck that h is still the head after publishing the hazard.
      if (h == head.load(mo::acquire)) {
        return h;
      }
    }
  }

  static bool is_hazard(Node* n) {
    for (HazardRecord& h : hazards) {
      if (h.ptr.load(mo::seq_cst) == n) {
        return true;
      }
    }

    return false;
  }

  static void push_retired_record(RetiredNode* r) {
    RetiredNode* old = retired_head.load(mo::relaxed);

    do {
      r->next = old;
    } while (
        !retired_head.compare_exchange_weak(old, r, mo::release, mo::relaxed));
  }

  static void scan_retired() {
    RetiredNode* list = retired_head.exchange(nullptr, mo::acquire);

    while (list) {
      RetiredNode* curr = list;
      list = list->next;

      Node* n = curr->node;

      if (is_hazard(n)) {
        push_retired_record(curr);
      } else {
        delete n;
        delete curr;
      }
    }
  }

  static void retire(Node* n) {
    RetiredNode* r = new RetiredNode{n, nullptr};
    push_retired_record(r);

    thread_local size_t retire_count = 0;

    if (++retire_count >= ScanThreshold) {
      retire_count = 0;
      scan_retired();
    }
  }

  static void force_delete_all_retired_after_threads_stopped() {
    RetiredNode* list = retired_head.exchange(nullptr, mo::relaxed);

    while (list) {
      RetiredNode* curr = list;
      list = list->next;

      delete curr->node;
      delete curr;
    }
  }

public:
  LockFreeStack() = default;

  LockFreeStack(const LockFreeStack&) = delete;
  LockFreeStack(LockFreeStack&&) = delete;
  LockFreeStack& operator=(const LockFreeStack&) = delete;
  LockFreeStack& operator=(LockFreeStack&&) = delete;

  ~LockFreeStack() {
    // Assumes no concurrent push/pop/destructor.
    Node* h = head.load(mo::relaxed);

    while (h) {
      Node* next = h->next;
      delete h;
      h = next;
    }

    // Assumes all worker threads using this stack have stopped.
    force_delete_all_retired_after_threads_stopped();
  }

  template <typename... Args> void push(Args&&... args) {
    Node* n = new Node(std::forward<Args>(args)...);

    Node* h = head.load(mo::relaxed);

    do {
      n->next = h;
    } while (!head.compare_exchange_weak(
        h, n,
        mo::release, // publish n->val and n->next
        mo::relaxed  // failure only gives us a pointer value
        ));
  }

  bool pop(T& out) {
    HazardRecord& hz = hazard_for_thread();

    while (true) {
      Node* h = protect_head(hz);

      if (!h) {
        hz.ptr.store(nullptr, mo::seq_cst);
        return false;
      }

      // Safe only because h is hazard-protected.
      Node* next = h->next;

      if (head.compare_exchange_weak(
              h, next,
              mo::acquire, // consume h->val and h->next
              mo::relaxed  // failure result ignored; we re-protect
              )) {
        // We now exclusively own h logically.
        // It is no longer reachable from the stack.
        hz.ptr.store(nullptr, mo::seq_cst);

        try {
          out = std::move(h->val);
        } catch (...) {
          retire(h);
          throw;
        }

        retire(h);
        return true;
      }

      // CAS failed. Do not use overwritten h.
      // Restart through protect_head().
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
