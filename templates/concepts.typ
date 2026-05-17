== Traits and Concepts
*Traits:*
- Example: std::is_integral_v<T>, std::remove_reference_t<T>
- Gets something about a type.
- Implemented as templated structs. Use v as syntactic sugar for cleaner access.
*Concepts:*
- Example: concept Integral = std::is_integral_v<T>, Addable = requires(T a, T b){ a + b; };
- Something enforced at compile time, can be defined via traits or expressions

If you want to just limit the types of a single function, you can just use std::requires() after the params

= requires (keyword)
Appears in three contexts:

== Requires Clause
*Constrains* when a templated T is valid, so immediately follows template<>, and precedes definition.
`template<typename T>
*requires* ... // Something on T
T func(T a, T b) { ... }
` 

== Requires Expression
Returns a compile-time boolean depending on if the body compiles. 
In particular, expressions like t[i] asserting "you can index this" are not able to be conveyed by any other methods.

A requires block can have 4 kinds of requirements:

- Simple (existence) requirement: Put a value / expression / method call
requires(T t) {
  t.foo(); 
}

- Compound requirement: Can add noexcept / return type to simple
requires {
  { t.foo() } noexcept -> std::same_as <int> // We must use concept here, not exact type
}
Note that -> is syntactic sugar for decltype((expr)), so here it would be:
`requires std::same_as<decltype((t.foo())), int>`

- Type requirement: some type / specialization (usage) of the type must exist
`requires {
  typename T::member;
  typename std::hash<T>
}`

And the last one is 3rd type of usage:
== Nested requires: more boolean conditions
requires {
  requires sizeof(T) >= 4;
}

Building to previous contexts, it is important to understand difference between:
`
template<typename T>
concept small_size = sizeof(T) <= 8;

template<typename T>
concept small_size = requires(T t) {
  sizeof(T) <= 8;
};
`
The latter only checks that the expression "sizeof(T) <= 8" compiles, which is true for any T.
So, if we want a boolean check inside of a requires, we can use a nested requires:
concept small_size = requires(T t, size_t i) {
  t[i];
  requires sizeof(T) <= 8;
  requires alignof(T) <= 16;
  ...
}
Note that this is largely syntactic sugar; we can represent the above as:
concept small_size = requires(T t, size_t i) { t[i]; } && sizeof(T) <= 8 && alignof(T) <= 16;
But keeping in one scope greatly helps with organization.



