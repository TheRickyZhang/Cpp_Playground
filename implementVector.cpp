#include <bits/stdc++.h>
#include <memory> 
#include <stdexcept>

namespace ricky {

template <typename T> class vector1 {
  T *a_;
  size_t n_;
  size_t mx_;

  void setToSize(size_t sz) {
    T *a = new T[sz];
    for (size_t i = 0; i < n_; i++) {
      a[i] = a_[i];
    }
    delete[] a_;
    a_ = a;
    mx_ = sz;
  }
  void expandSize() { setToSize(mx_ == 0 ? 1 : 2 * mx_); }

public:
  vector1() : a_(nullptr), n_(0), mx_(0) {}
  vector1(int n) : n_(n), mx_(n) { a_ = new T[n]; }
  ~vector1() { delete[] a_; }

  void push_back(T x) {
    if (n_ == mx_) {
      expandSize();
    }
    a_[n_++] = x;
  }
  const T at(size_t i) {
    if (i >= n_)
      throw std::runtime_error("out of bounds");
    return a_[i];
  }
  size_t getSize() { return n_; }
  size_t getCapacity() { return mx_; }
  void shrink_to_fit() { setToSize(n_); }
  void pop_back() {
    if (n_ == 0)
      throw std::runtime_error("nothing to pop back");
    n_--;
  }

  void print() {
    std::cout << "size: " << n_ << ", capacity: " << mx_ << "\n";
    std::cout << "array: ";
    for (size_t i = 0; i < n_; i++) {
      std::cout << a_[i] << " ";
    }
    std::cout << "\n";
  }
};

template <typename T> class vector2 {
  T *a_;
  size_t n_;
  size_t mx_;

  void setToSize(size_t sz) {
    T *a = static_cast<T *>(::operator new(sizeof(T) * sz));
    size_t i = 0;
    for (; i < n_; ++i){
      std::construct_at(a + i, std::move(a_[i]));
    }
    for (size_t j = 0; j < n_; ++j) {
      std::destroy_at(a_ + j);
    }
    ::operator delete(a_);
    a_ = a;
    mx_ = sz;
  }

  void expandSize() { setToSize(mx_ == 0 ? 1 : 2 * mx_); }

public:
  vector2() : a_(nullptr), n_(0), mx_(0) {}

  vector2(int n) : a_(nullptr), n_(0), mx_(0) {
    a_ = static_cast<T *>(::operator new(sizeof(T) * static_cast<size_t>(n)));
    for (; n_ < static_cast<size_t>(n); ++n_) {
      std::construct_at(a_ + n_);
    }
    mx_ = static_cast<size_t>(n);
  }

  ~vector2() {
    for (size_t i = 0; i < n_; ++i) {
      std::destroy_at(a_ + i);
    }
    ::operator delete(a_);
  }

  vector2(const vector2 &) = delete;
  vector2 &operator=(const vector2 &) = delete;

  vector2(vector2 &&o) noexcept : a_(o.a_), n_(o.n_), mx_(o.mx_) {
    o.a_ = nullptr;
    o.n_ = o.mx_ = 0;
  }
  vector2 &operator=(vector2 &&o) noexcept {
    if (this != &o) {
      for (size_t i = 0; i < n_; ++i) {
        std::destroy_at(a_ + i);
      }
      ::operator delete(a_);
      a_ = o.a_;
      n_ = o.n_;
      mx_ = o.mx_;
      o.a_ = nullptr;
      o.n_ = o.mx_ = 0;
    }
    return *this;
  }

  // void push_back(const T &x) {
  //   if (n_ == mx_) expandSize();
  //   std::construct_at(a_ + n_, x);
  //   ++n_;
  // }
  // void push_back(T &&x) {
  //   if (n_ == mx_) expandSize();
  //   std::construct_at(a_ + n_, std::move(x));
  //   ++n_;
  // }
  void push_back(T x) {
    if (n_ == mx_) {
      expandSize();
    }
    a_[n_++] = x;
  }

  // Also const variant
  T &at(size_t i) {
    if (i >= n_) throw std::runtime_error("out of bounds");
    return a_[i];
  }

  size_t getSize() { return n_; }
  size_t getCapacity() { return mx_; }
  void shrink_to_fit() { setToSize(n_); }

  void pop_back() {
    if (n_ == 0) throw std::runtime_error("nothing to pop back");
    --n_;
    std::destroy_at(a_ + n_);
  }

  void print() {
    std::cout << "size: " << n_ << ", capacity: " << mx_ << "\n";
    std::cout << "array: ";
    for (size_t i = 0; i < n_; i++) std::cout << a_[i] << " ";
    std::cout << "\n";
  }
};
} // namespace ricky

struct complexObject {
  int *a;
  std::string b;
  complexObject(int x, std::string y) {
    a = new int(x);
    b = y;
  }
  friend std::ostream &operator<<(std::ostream &os, const complexObject &o) {
    return os << *o.a << ", " << o.b;
  }
};

int main() {
  ricky::vector2<complexObject> v;
  v.print();
  v.push_back(complexObject(1, "a"));
  v.push_back(complexObject(1, "b"));
  v.print();
  v.pop_back();
  v.push_back(complexObject(2, "a"));
  complexObject c(2, "c");
  v.push_back(c);
  v.print();
}

// // -------- Level 2: new[]/delete[] (requires DefaultConstructible,
// // Move/CopyAssignable)
// namespace ricky {
// template <typename T> class vector {
// public:
//   vector() { expand(1); }
//   ~vector() {
//     for (int i = 0; i < sz; i++) {
//       (a + i)->~T();
//     }
//     ::operator delete(a);
//   }
//   void push_back(const T &x) {
//     if (sz == cap)
//       expandToCapacity(3 * cap);
//     ::new ((void *)(a + sz)) T(x);
//     sz++;
//   }
//   void emplace_back(T &&x) {
//     if (sz == cap)
//       expandToCapacity(3 * cap);
//     ::new ((void *)(a + sz)) T(std::move(x));
//     sz++;
//   }
//   const T &at(std::size_t i) const {
//     if (i >= sz)
//       throw std::runtime_error("Out of bounds");
//     return a[i];
//   }
//   std::size_t get_size() const { return sz; }
//   std::size_t get_capacity() const { return cap; }
//   void shrink_to_fit() {
//     if (cap != sz) {
//       expandToCapacity(sz);
//     }
//   }
//   void pop_back() {
//     if (sz == 0)
//       throw std::runtime_error("Cannot pop");
//     (a + sz - 1)->~T();
//     sz--;
//   }
//
// private:
//   void expandToCapacity(int n) {
//     // Populate new data
//     T *na = static_cast<T *>(::operator new(sizeof(T) * n));
//     for (int i = 0; i < sz; i++) {
//       ::new ((void *)(na + i)) T(std::move(a[i]));
//     }
//
//     // Destroy, deallocate old data
//     for (int i = 0; i < sz; i++) {
//       (a + i)->~T();
//     }
//     ::operator delete(a);
//
//     // Update vals
//     a = na;
//     cap = n;
//   }
//   T *a = nullptr;
//   int sz = 0, cap = 0;
// };
// } // namespace ricky
//
// // -------- Level 3: raw storage + placement new (no DefaultConstructible
// // requirement)
// namespace l3 {
// template <typename T> class vector {
// public:
//   vector() = default;
//   ~vector() {
//     destroy_range(data_, sz_);
//     ::operator delete(data_);
//   }
//
//   void push_back(T element) {
//     if (sz_ == cap_)
//       grow_raw(cap_ ? cap_ * 2 : 1);
//     ::new (data_ + sz_) T(std::move(element)); // construct one
//     ++sz_;
//   }
//   const T &at(std::size_t i) const {
//     if (i >= sz_)
//       throw std::out_of_range("vector::at");
//     return data_[i];
//   }
//   std::size_t get_size() const { return sz_; }
//   std::size_t get_capacity() const { return cap_; }
//
//   void shrink_to_fit() {
//     if (cap_ == sz_)
//       return;
//     if (sz_ == 0) {
//       ::operator delete(data_);
//       data_ = nullptr;
//       cap_ = 0;
//       return;
//     }
//     T *nd = static_cast<T *>(::operator new(sizeof(T) * sz_));
//     std::size_t built = 0;
//     try {
//       for (; built < sz_; ++built)
//         ::new (nd + built) T(std::move_if_noexcept(data_[built]));
//     } catch (...) {
//       destroy_range(nd, built);
//       ::operator delete(nd);
//       throw;
//     }
//     destroy_range(data_, sz_);
//     ::operator delete(data_);
//     data_ = nd;
//     cap_ = sz_;
//   }
//   void pop_back() {
//     if (!sz_)
//       return;
//     --sz_;
//     (data_ + sz_)->~T(); // destroy one
//   }
//
// private:
//   static void destroy_range(T *p, std::size_t n) noexcept {
//     for (std::size_t i = 0; i < n; ++i)
//       (p + i)->~T();
//   }
//   void grow_raw(std::size_t nc) {
//     T *nd =
//         static_cast<T *>(::operator new(sizeof(T) * nc)); // raw,
//         uninitialized
//     std::size_t built = 0;
//     try {
//       for (; built < sz_; ++built)
//         ::new (nd + built) T(std::move_if_noexcept(data_[built]));
//     } catch (...) {
//       destroy_range(nd, built);
//       ::operator delete(nd);
//       throw;
//     }
//     destroy_range(data_, sz_);
//     ::operator delete(data_);
//     data_ = nd;
//     cap_ = nc;
//   }
//
//   T *data_ = nullptr;
//   std::size_t sz_ = 0, cap_ = 0;
// };
// } // namespace l3
