Overload pattern for using std::visit (note that it must be called with std::variant anyways, so very common idiom)
Normally, std::visit takes in (optional return type, ) a callable visitor, and Variants...

Without overloaded:

struct SomeVisitor {
  void operator(T t) {...}
  void operator(U u) {...}
}
std::visit(SomeVisitor{}, x)

With overloaded:

// By inheriting from each T, we easily create a callable object that supports all our different types. Note there should be no waste since the Ts are generated from a lambda, so they only define operator()
`
template<class Ts...>
struct SomeVisitor : Ts... { 
  using Ts::operator()...; // We assume Ts are public (structs), and these using declarations disambiguate function calls
};
`

std::visit(
  overloaded{
    [](T t) {...},
    [](U u) {...}
  }, x
)
This pattern is very common but not added to the standard library. It is also much better with variable capturing.
