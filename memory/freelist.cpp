#include <bits/stdc++.h>
using namespace std;

namespace getcracked {

template<class T>
class ObjectPool {
  struct Slot {
    union {
      Slot* next;
      alignas(T) char storage[sizeof(T)];
    };
  };

  // Every Slot* in the free list guaranteed to have next field.
  Slot* free = nullptr;
  vector<Slot*> chunks;
  size_t n;

  void grow() {
    Slot* base = static_cast<Slot*>(::operator new(sizeof(Slot) * n));
    chunks.push_back(base);

    // Put into free list. Note it is easier and better for performance to prepend into the head.
    for(size_t i = 0; i < n; ++i){
      base[i].next = free;
      free = &base[i];
    }
  }
public:
  explicit ObjectPool(size_t chunk_n_ = 1024) : n(chunk_n_) {
    grow();
  }

  ~ObjectPool() {
    for(Slot* base : chunks) ::operator delete(base);
  }

  template<class... Args>
  T* acquire(Args&&... args) {
    if(!free) grow();
    Slot* s = free;
    free = s->next;

    // placement-new constructs T into slot storage
    return new (s->storage) T(std::forward<Args>(args)...);
  }

  void release(T* p) {
    if(!p) return;
    p->~T();

    Slot* s = reinterpret_cast<Slot*>(p);
    s->next = free;
    free = s;
  }
};

} // namespace getcracked

