Ecosystem: Landscape knowledge of what common template patterns / built-in concepts exist.


# Templates

# Concepts

The older way to do the equivalent of concepts was type-traits, ie templates like `std::is_default_constructible<T>` have a ::value boolean member that is either `std::true_type` or `std::false_type`, which also has an `std::is_default_constructible_v` alias.

The primary distinction is:
- Type traits describe everything the compiler knows about an object
- Concepts describe what we semantically and mechanically require with our types
Logical conclusions are:
- Wrapping traits in concepts can make sense when it's a real requirement for your program
- Not all traits have a corresponding concept (ex no such `trivially_copyable<T>` exists). The ones that do are generally common requirements that the standard library would use.

To tell which is which:
Type traits use an is_ prefix:
`is_thing<T>` or `is_thing_v<T>`

Concepts use direct adjectives/capabilities:
`integral<T>`, `same_as<T, U>`

Under the hood, concepts can additionally cover expression checks (will some arbitrary expression compile).
Additionally, the standard specifies that "modeling" a concept satisfies semantic requirements that can't be checked at compile-time implementation, such as `std::partially_ordered`.


Basically, requires clause is an absolute improvement over previous `std::enable_if_t<...>` patterns, and can guard against the function itself instead of relying on static_asserts inside.

Concepts are still used when:
- We want branching implementation details within a function, ie check `if constexpr (is_thing<T>)`
- No standard concept exists (though we can wrap ourselves): `is:trivially_copyable_v<T>`.
This is a bit nuanced:


We have std::is_default_constructible_v<T> which is true iff T() is possible, ie std::is_constructible_v<T> with no arguments

But std::is_default_initializable is a stricter condition, implemented as:

```cpp
template<typename T>
concept default_initializable = constructible_from<T> && 
```

In vector::reallocate, use:
```cpp
if constexpr (std::is_nothrow_move_)
```

Naming idioms:
- uninitialized_X: prepend when destination is raw storage.
- copy/move: when you're taking from another object / range.
- fill: when you're populating with a single value
- default_X: more concise, specialized for default constructors.

Destroy -> only apply to live objects, so no raw-storage form of std::destroy().

Types of initialization:
- default: T x; // will be garbage for primitives like int
- value: T x{}; or T() (temp);




