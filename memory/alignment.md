Know the terminology: cache coherence is a fundamental mechanism that ensures all processors have the same view of memory.

False sharing is the large performance penalty that cache coherence causes when different threads modify variables independently on the same cache line.

To prevent false sharing, such as a SPSC queue, use:
```cpp
alignas(hardware_destructive_interference_size)
```

To encourage sharing for data that should be accessed together, use:
```cpp
alignas(hardware_constructive_interference_size)
```
Note that while it is very natural for this to be universally applicable in single-threaded structs, it has downside of over-aligning them, so we can't use it everywhere.

Note that in most cases both equal sizeof(L1 cache), but when compiling to multiple targets we can have constructive <= destructive.
