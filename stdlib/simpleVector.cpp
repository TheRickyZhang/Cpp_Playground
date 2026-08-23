#include <bits/stdc++.h>

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

}
