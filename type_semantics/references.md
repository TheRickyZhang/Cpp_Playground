- References as pointers is an implementation detail, so not always guaranteed.
In basic cases like:
int x = 1;
int& r = x;
The compiler can directly substitute foo(x) -> foo(r).

This does not work when the compiler cannot guarantee the identity of the reference. For instance:
int& r = cond ? x : y;
void foo(int& r) // But will if inlined
class Foo {
  int& r; // Possible to resolve f.r statically, such as if everything known in translation unit
}
- Note that the GENERAL case is what determines the language rules and the ABI

Classes can contain references, but must implicitly delete operator= as a result (you can define, but no rebind is possible)
On the other hand, unions cannot contain references, since the same compromise goes against the purpose of a union

- References have no notion of "beginning lifetime", as they are tied to something else. 
- Assignment, ie f.r = s should assign by value to match the standard everywhere else, but that does not fit here.

union -> different tagged union implementations: std::optional, std::variant, std::expected

std::expected: recognize when to use std::transform to preserve optional -> optional consecutive processing, returning nullopt if fail.

Note when used with a template, such as:
`
template<typename T>
inline typename std::remove_reference<T>::type&& move(T&& t) {
  return static_cast<typename std::remove_reference<T>::type&&>(t);
}
`
T&& is a perfect forwarding reference, not an rvalue reference, ie it can bind to anything like const T&, but preserves all value category information, like constness.


Some summary to get you intuition on why we have this exception (very similar to how const T& binds to everything, which is just a more ergonomic overload that would be hard or impossible to implement otherwise):
Summary
The core question: Why does std::move's parameter T&& accept lvalues, when int&& (non-template) only accepts rvalues?
The answer in one sentence: Templates have one special deduction rule for T&& that, combined with the general rule of reference collapsing, makes the parameter accept anything while preserving value category.
The two mechanisms
1. Reference collapsing (general rule, not special to forwarding)
When reference qualifiers stack via template substitution, they collapse:
InnerOuterResult&&&&&&&&&&&&&&&&&
Mnemonic: rvalue-ness only survives when both qualifiers are &&; lvalue-ness wins any composition. This rule exists because templates can be instantiated with reference types as T, producing reference-to-reference situations that need a defined meaning — it's not specific to forwarding.
2. The one special deduction rule (the thing you have to memorize)

When the parameter is T&& (with T being deduced) and the argument is an lvalue of type X, deduce T = X&.

This is the only place in the type system where T can deduce to a reference type. Everywhere else (T, T&, const T&), T deduces to a non-reference. This is an unapologetic special case the committee added in C++11 specifically to enable perfect forwarding.
How it produces forwarding behavior
For template<class T> f(T&& x):

f(a) with int a (lvalue): special rule fires, T = int&, parameter becomes int& && → collapses to int&. Accepts the lvalue.
f(42) (rvalue): normal rule, T = int, parameter becomes int&&. Accepts the rvalue.

So the same source generates two different instantiations depending on the argument's category — one for lvalues, one for rvalues — under one signature.
Why T&& (not const T& or something else) for std::move
const T& would bind to anything but couldn't be cast to a non-const rvalue reference (UB-adjacent, defeats the purpose). The forwarding reference is the only parameter form that:

Accepts both lvalues and rvalues.
Preserves const-ness exactly.
Doesn't lose value category information.
