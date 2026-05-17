#include <bits/stdc++.h>
#include <memory>
#include <type_traits>
using namespace std;


/*
 * Great write-up about this:
 * https://medium.com/@sgn00/high-performance-memory-management-arena-allocators-c685c81ee338
 *
* Invariants:
* Block is [begin, end), with begin <= curr <= end
* Does not manage object lifetimes, so by default let's say users must destroy objects manually
*
* Start with single-threaded, monotonic arena.
*
* Supporting user construction is easy, but destruction requires storing destructors also in the arena as a reversed linked list.
*/



// Simple implementation
class Arena {
  char* a;
  size_t cap;
  size_t offset;

public:
  explicit Arena(int size) : a(static_cast<char*>(::operator new(size))), cap(size), offset(0) {}
  ~Arena() {
    ::operator delete(a);
  }
  Arena(const Arena&) = delete;
  Arena(Arena&&) = delete;

  void* allocate(int size, int alignment) {
    char* curr = a + offset;
    size_t remain = cap - offset;
    // We want a separate value to fulfill void* type requirements, and to not modify original.
    void* aligned_curr = curr;
    /*
    * Intuition for use of std::align:
    * 
    */
    if(std::align(alignment, size, aligned_curr, remain) == nullptr) {
      throw std::bad_alloc();
    }
    // Important to add size because align returns the first byte that is now allocated, and we want first free
    offset = static_cast<char*>(aligned_curr) - a + size;
    return aligned_curr;
  }
  
  // No need to actually delete anything?
  void reset() {
    offset = 0;
  }

  
};

template<typename T>
class ArenaAllocator {
  Arena* arena;
  explicit ArenaAllocator(Arena* arena) noexcept : arena(arena) { }
  using value_type = T;

  template<typename U>
  ArenaAllocator(const ArenaAllocator<U>& other) : arena(other.arena) { }

  T* allocate(int n) {
    return static_cast<T*>(arena->allocate(n * sizeof(T), alignof(T)));
  }
  void deallocate(T*, size_t) noexcept { }

  // operator=
};


namespace getcracked {
class Arena {

public:
  explicit Arena(int size) : a(static_cast<char*>(::operator new(size))), cap(size), offset(0) {}
  ~Arena() {
    ::operator delete(a);
  }
  Arena(const Arena&) = delete;
  Arena(Arena&&) = delete;

  // Usage of this->allocate() is not excpetion safe, see details.
  template<typename T, typename... Args>
  void* create(Args&&... args) {
    void* aligned_curr = this->allocate(alignof(T), sizeof(T));
   
    T* obj = std::construct_at(aligned_curr, std::forward<Args>(args)...);

    if constexpr (is_trivially_destructible_v<T>) {
      return obj;
    } else {
      aligned_curr = this->allocate(alignof(DestructNode), sizeof(DestructNode));;

      auto dcall = [](void* p) {
        static_cast<T*>(p)-> ~T();
      };
      DestructNode* dnode = construct_at(aligned_curr, {dcall, tail, obj});
      tail = dnode;
      return obj;
    }
  }
  
  // No need to actually delete anything?
  void reset() {
    offset = 0;
  }
private:
  struct DestructNode {
    void (*d)(void*);
    DestructNode* prev;
    void* obj;
  };
  char* a;
  size_t cap;
  size_t offset;
  DestructNode* tail;

  void call_destructors() {
    while(tail != nullptr) {
      tail->d(tail->obj);
      tail = tail->prev;
    }
  }

  // Technically, directlly modifying offset like this is not exception safe, as we might be in partial state before allocating destructor fails. To fix, we need to make this return the new offset instead, and delayed assignment.
  void* allocate(size_t alignment, int size) {
    char* curr = a + offset;
    size_t remain = cap - offset;
    void* aligned_curr = static_cast<void*>(curr);
    if(std::align(alignment, size, aligned_curr, remain) == nullptr) {
      throw std::bad_alloc();
    }
    offset = static_cast<char*>(aligned_curr) - a + size;
    return aligned_curr;
  }
};
};
