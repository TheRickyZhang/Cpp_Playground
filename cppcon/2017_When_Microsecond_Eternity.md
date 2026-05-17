https://www.youtube.com/watch?v=NH1Tta7purM
By Carl Cook (Optiver)

Hotpath only executed ~0.1% of the time, but needs to be consistently fast, which is at odds with the operating system, networks, and hardware.

You don't just need to know C++ well, but actually figure out what the compiler is generating (godbolt!)
Probably good to know how to read some assembly code: call/jump expensive, conditional move less expensive

Hyperthreading = simultaneous multithreading; one physical CPU core exposes itself as multiple logical cores in the operating systemSo for instance, in an 8-core / 16-thread CPU, std::thread::hardware_concurrency() returns 16.
- It exists because a single CPU core can be idle when waiting for something else
- Hyperthreads differ in the first parts of the CPU pipeline (different instruction pointers, registers) and share the later parts (cache, allocation, scheduling)
Overall, beneficial when one thread often stalls, or contains different bottlenecks that can be spread out. Bad when the shared resource is saturated -> conflict with accesses
- Good: dependency-chain arithmetic code, ie rolling hash / sorting
- Needs Benchmarking: cache-miss / pointer-chasing / branchy code (tradeoff)
- Bad: vectorized math, anything with concurrency
- When benchmarking, we usually want to test all of: single thread, multi-thread, multi-thread with hyperthreading

# Low latency programming techniques

Slowpath removal:
- Ensure error handling code is not inlined, such as using an integer error flags instead of checking for each.

Template-based configuration
- When needing runtime configuration, instead of using virtual functions, use a factory pattern that assigns the types at the highest level possible:
```cpp
std::unique_ptr<IOrderManager> Factory(const Config& config) {
  if(config.useOrderSenterA()) {
    return std::make_unique<OrderManager<OrderSenderA>>();
  } else {
    return std::make_unique<OrderManager<OrderSenderB>>();
  }
}
```

Lambdas:
Very obvious, but prefer this syntax to preserve exact types for compile-time:
```cpp
template<typename T> void SendMessage(T&& lambda) { ... lambda(msg); ... }
SendMessage([&](Message& msg) {...});
```

Memory Allocation
- Use a pool and reuse objects, obviously
- If you must delete large objects, use another thread

Multithreading is best avoided
- If you must use multiple threads, consider passing copies of data rather than sharing data.
- If you must share data, consider removing synchronization requirements

Okay to denormalize data to improve cache-line utilization with spatial locality!


Benchmarking std::unordered_map - prefer some cache-friendly version

Use `__attribute__`((always_inline)) and ((noinline)) for actually forcing code inline
No inline: preferred to not pollute instruction cache

# Keeping the cache hot
- simple solution: run a very frequent dummy path simulating an order send

## Various:
- Copy-on-write means: only fully copy an assigned string until the one of the values is modified (use shared references until you can't)
- This is very bad behavior in the cases of non-SSO strings.
- As of C++ 11, function-local static variables are guaranteed to be initialized once, even across different threads, but that comes at a slight cost of checking initialization each time if using dynamic initialization.
```cpp
struct Random() {
  int get() {
    static int i = rand(); // calls the initialization guard on every call
    return i;
  }
}
```
This is not inherently bad, but can get misused if callers do lot know they are paying the cost. If you want to access the member many times, it is better to only do it once within a type:
```cpp
struct Random {
  int i = rand(); // created once at construction
  int get() const { return i; }
}
Random() random_instance() {
  static Random r; // still must call the initialization guard, but now r.i is free if we use it in multiple places through the class
  return r;
}
```

# Measurement
Two ways:
## Profiling: What is my code doing? Will change the program for comprehensive report.
## Benchmarking: How fast is my program, over many iterations?
- Sampling profilers (gprof): miss key events since they only sample a small amount
- Instrumentation profilers (callgrind): are too intrusive
- Microbenchmarks (Google benchmark): are too specific, don't account for heap fragmentation / overall optimization of full env
-> Best for micro-benchmarking is measuring end-to-end in production-like setup.

# QnA
Question: You said consider deleting large objects on another thread, but one of the things I've been burned by is that we'll drain the TC malloc size class for that, and then cause global mutex locks across all of your threads when you allocated or deallocate in that size class

Breakdown:
TCMalloc = Google's thread-cached malloc (general-purpose heap allocation avoided in latency-sensitive paths)
TCMalloc Size class = what bucket sizes are grouped into for purposes of allocating
Try to make allocations thread-local by giving each thread its own freelist, which is occasionally refilled by bath from the global cache
So if thread A always allocates, and B always frees, then we basically still get the same traffic on the main allocator
