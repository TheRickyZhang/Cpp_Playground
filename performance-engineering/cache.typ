We of course want to use templates to speed up our execution. But we also don't want to bloat the instruction cache (icache) with too many code paths, especially if they are not used.

Thus it is very important to separate the slowpath code from the hotpath. We reduce "inlining" particularly by encapsulating in function and marking usage with attributes (compiler hints) like [[unlikely]], [[gnu::noinline, gnu::cold]]
Example that puts cold section in separate location:
``` cpp
void processOrder(Order& order) {
  if(!validOrder(order)) {
    ...
  }
  // Actual hot path order logic
}
->
[[gnu::noinline, gnu::cold]]
void handleInvalidOrder(Order& order) {
  ...
}
void processOrder(Order& order)  {
  // Note unlikely only makes sense for runtime branches
  if(!validOrder(order)) [[unlikely]] {
    handleInvalidOrder(order);
  }
  // Actual hot path order logic
}
```

The same applies to a constexpr if(templatedBool). To not get compiled into every instantiation, we mark out of line with noinline, cold.
