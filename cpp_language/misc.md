# Random
How are exceptions zero-cost?
- We still pay the cost of whatever check we do to emit the exception, but in addition, compiler emits exception tables that trigger when we do throw.

- It is standard API to put public: first, then private: in a class, but when just doing a quick interview implementation, can but private members at the top to make interface / invariants more visible.

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
