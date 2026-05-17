#include <bits/stdc++.h>
using namespace std;

/*
 * The first challenge of this is how to leverage the templated nature to have
 * compile-time resolution, since we must know the specific type / location of
 * what we are trying to fetch. We actually think in a functional sense: a tuple
 * is just the first element + the rest of the tuple, as a template This is
 * recursive resolution, the dominant metaprogramming pattern in C++
 *
 */
namespace getcracked {
// Must have primary template before specialization. But we will never use this, will always fall to specializations.
template <typename... Types> class tuple;

// Only exists to satisfy compiler for ending recursion. We never will actually access a member from this.
template <> class tuple<> {
public:
  tuple() = default;
};

// Actual implementation and usage. Note use of head() and rest()
template <typename Head, typename... Tail>
class tuple<Head, Tail...>{
  Head head{};
  tuple<Tail...> tail{};
public:
  tuple() = default;
  tuple(const Head& h, const Tail&... r) : head(h), tail(r...) { }

  Head& getHead() { return head; }
  const Head& getHead() const { return head; }

  tuple<Tail...>& getTail() { return tail; }
  const tuple<Tail...>& getTail() const { return tail; }
};

// For the getter, we use a struct because function templates cannot be partially specialized.

// Must have primary template before specialization. Again, will not be used.
template<unsigned N, typename Tuple>
struct getter;

// specialization for N = 0
template<typename Head, typename... Rest>
struct getter<0, tuple<Head, Rest...>> {
  static Head& get(tuple<Head, Rest...>& t) {
    return t.getHead();
  }
  static const Head& get(const tuple<Head, Rest...>& t) {
    return t.getHead();
  }
};

// Specialization else if at least 1 element
template<unsigned N, typename Head, typename... Rest>
struct getter<N, tuple<Head, Rest...>> {
  // We need to supply const and non-const versions here to support const and non-const declarations of tuple.
  static auto& get(tuple<Head, Rest...>& t) {
    return getter<N-1, tuple<Rest...>>::get(t.getTail());
  }
  static const auto& get(const tuple<Head, Rest...>& t) {
    return getter<N-1, tuple<Rest...>>::get(t.getTail());
  }
};

template<unsigned N, typename... Elements>
auto& get(tuple<Elements...>& t) {
  return getter<N, tuple<Elements...>>::get(t);
}
template<unsigned N, typename... Elements>
const auto& get(const tuple<Elements...>& t) {
  return getter<N, tuple<Elements...>>::get(t);
}

} // namespace getcracked
