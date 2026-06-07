# Random
Usage of ... (Can mean different things):
- typename... name -> You repeat the typename for everything in pack. 
- name... -> to use a pack,you must expand so it is not ambiguous.

How are exceptions zero-cost?
- We still pay the cost of whatever check we do to emit the exception, but in addition, compiler emits exception tables that trigger when we do throw.

SFINAE: A bit ugly, but allows for deactivating template code for types that don't match.
https://www.fluentcpp.com/2018/05/15/make-sfinae-pretty-1-what-value-sfinae-brings-to-code/
- It is standard API to put public: first, then private: in a class, but when just doing a quick interview implementation, can but private members at the top to make interface / invariants more visible.
- Placement new can allocate + construct (usual form) or just construct at existing memory with the form: = (ptr) new(...)k
- char\* and std::byte\* are basically the same, only prefer char\* when you need flexibility of looking inside
- std::launder does memory laundering; that is, preventing compiler from tracing where memory came from, thus allowing you to rewrite const variables or reinterpret_cast.

- nullopt_t is just the type for nullopt, ie if you only want to accept that.

TCP/IP Model: ATN DP, application, transport, network, data, physical.

Cache Coherence: how all cores can agree on value of cache line. 
- MESI is standard protocol.

A vtable is what is used to support virtual functions; create table of function pointers, and each instance has pointer to table entry; uses dynamic dispatch to figure out what to call at runtime

Generally, "store" destructors using type-erased destruction, so function pointer + pointer to object to destroy.
- Delegate destroying to a callable (wrapper) that knows what type to delete on.
- Only costs one indirect call

== Thinking about Restrictions
Why can't you define nested functions?
- C ABI compatibility
- Difficult, benefit not work cost
- Funarg problem: it is possible for nested function to escape enclosing function, as opposed to classes which CANNOT access local variables of the function.
  - If we tried to make nested functions not capture surrounding scope, then it does not add anything vs classes, which don't capture.
  - Languages that do implement will need extra memory, and some strategy like storing parent function or creating an extra class on heap to extend lifetime (what lambda captures do). OCaml's Hybrid approach is interesting.

## Initializer Lists
Initializer lists work similarly to const char*: if we do
```cpp
vector<int> a = {1, 2, 3}
// It is doing something like: 
const int[3] temp = {
  1, 2, 3
};
vector<int> a(temp.begin(), temp.end());
```
Note that it usually does an extra copy compared to a more direct:
```cpp
vector<int> a; 
a.reserve(3);
a.push_back(1); a.push_back(2); a.push_back(3);
```
So we typically want to avoid in low-latency contexts
