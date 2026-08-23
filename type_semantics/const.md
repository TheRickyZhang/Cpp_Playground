Generally our practice should be:
Put const to the immediate right of whatever it applies to

int const* -> the integer is constant.
int* const -> the pointer is constant
int const* const -> both are constant

Generally, const is shallow, ie just because a pointer is const doesn't mean its contents are const.

Propagation instead occurs bitwise, ie when an object is const, all of its members are const.

mutable makes an exception to this rule, ie it just makes const contracts to the object not necessarily apply to all members;
struct foo {
  int mutable x;
  // allowed
  void bar() const {
    x = 1;
  }
}
// not allowed
const foo;
foo.x = 1;

A good use of this is with *cached members*, where we want a logical "const" contract API to still be supported by a change to physical memory. *mutexes* are very commonly cached because their role even in const objects is to lock, which still modifies its internal state.

So this should make sense:

```cpp
class foo {
  int x;
  mutable mutex m;
  mutable int cached_x;
}

```
