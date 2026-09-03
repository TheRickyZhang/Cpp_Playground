O0, O1, O2, O3 = optimization aggression

Note that even on O3, compiler must still preserve semantics of the program, which may miss some optimizations.

For instance, floating point addition is not associative, so cannot pipeline this effectively:
```cpp
vector<double> v = {...};
double a = 0.0;
for(int i = 0; i < n; i++) {
  a += v[i];
}
```

However, we can still manually separate into 4 accumulators, which will produce same sum in real math, but not necessarily in floating point math:
```cpp
vector<double> v = {...};
double a1{}, a2{}, a3{}, a4{};
for(int i = 0; i < n; i += 4) {
  a1 += v[i];
  a2 += v[i+1];
  a3 += v[i+2];
  a4 += v[i+3];
}
double a = a1 + a2 + a3 + a4;
```


