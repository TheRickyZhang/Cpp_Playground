#include <atomic>
#include <bits/stdc++.h>
using namespace std;

// The core reason stack is more complex than queue, say comparing to SPSC
// queue, is that the head is modified by both producers and consumers, meaning
// that it is more similar to a MPMC queue, where we need CAS to validate that
// our view on some data is still valid, and if not, loop until it is.

/*
Intuition for memory ordering (DON'T OVERTHINK IT)
The only non-atomic data that matters is node->val and node->prev.
We see that push writes node contents if it succeeds, and pop always consumes node contents.
Thus, push does release/relaxed, while pop does acquire. Simple as that.
*/

using mo = std::memory_order;


template <typename T> class SPSCStack {
public:
  SPSCStack() {}
  ~SPSCStack() {
    Node* h = head.load(mo::relaxed);
    while(h != nullptr) {
      Node* prev = h->prev;
      delete h;
      h = prev;
    }
  }

  SPSCStack(const SPSCStack& other) = delete;
  SPSCStack(SPSCStack&& other) = delete;
  SPSCStack& operator=(const SPSCStack& other) = delete;
  SPSCStack& operator=(SPSCStack&& other) = delete;

  template <typename... Args> void push(Args&&... args) {
    Node* n = new Node{T(std::forward<Args>(args)...)};
    Node* old = head.load(mo::relaxed);
    do {
      n->prev = old;
    } while (!head.compare_exchange_weak(
        old, n,
        mo::release, // On success, we want to publish n->val and n->prev
        mo::relaxed  // On failure, we are not changing anything
    ));
  }

  // While push should always work when unbounded, pop can still fail, so must return bool
  bool pop(T& x) {
    Node* h = head.load(mo::acquire);
    while (h != nullptr) {
      Node* prev = h->prev;
      if (head.compare_exchange_weak(h, prev, mo::acquire, mo::acquire)) {
        x = std::move(h->val);
        // Only safe under narrow assumptions:
        // SPSC, producer never dereferences old pointers, deleted nodes not recycled back, destructor runs after all threads
        // Essentially, avoiding memory reclamation: if we stored a node as a local variable in one thread that is deleted in another, uh oh spagettio.
        delete h;
        return true;
      }
    }
    return false;
  }

private:
  struct Node {
    T val;
    Node* prev{nullptr};
  };
  atomic<Node*> head{nullptr};
};
