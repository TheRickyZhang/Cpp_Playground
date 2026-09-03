Projections: not any concrete type, but basically any callable, less common ones are:
- Functors
- std::function
- pointer to member

# Ranges
Generally zero-overhead

Can do like:
```cpp
vector<pair<int, int>> a;
ranges::lower_bound(a, 100, {}, &a::first);
```

The {} is comparator, default std::less.

Last argument is projection, which can be anything that works with std::invoke. Can also use lambda.
The &class::member is syntax for pointer-to-member, which is a special type. (Think of it as an offset just for that class).

Remember any_of, all_of, none_of

# Views
- A specific type of range, lightweight types
- Can be slower depending on how used
Views have a type as a filter_view, storing ref to object and the predicate. It has iterators with the predicate behaviors baked in. So it should make sense that:
- An expensive predicate can be repeated if iterated over twice
- Random access not preserved with filter(), but is preserved with transform()
- Ordering multiple views impacts performance


for(int x : a | views::filter(lambda boolean)) {
}

for(int x : a | views::transform(lambda type)){
}

In a map, you can explicitly do: mp | view::keys or mp | views::values

Good example: to sum all position for active orders, we can do:
```cpp
int totPosition = a
  | views::filter([](const Order& o){ return o.active; })
  | views::transform([](const Order& o) { return o.size * o.price;});
```
