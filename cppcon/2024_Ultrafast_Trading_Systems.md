https://www.youtube.com/watch?v=sX2nF1fW7kI&t=139s

# Order Book TLDR
Everywhere uses an order book as a central structure

Designing a matching engine with order book: starts with map, but that has decent mid-time hump

## Principle 1: Usually node containers are bad
- Moves to vector for contiguous memory, but compared to map, it has a long tail latency that gets fixed by "reversing" a direction (huh)?
- Optimize with branchless binary search: simply transform runtime conditional checks with data, which often fail branch prediction, into non-branch-predicted runtime evaluations, ie:
```cpp
if(condition) l += jump -> l += (condition ? 1 : 0) * jump;
```
(Other examples include incrementing, min/max, and especially vectorized/SIMD code)
(Basically, in assembly, a jump like jge (jump if greater or equal to) changes which instruction comes less, which is a big deal. But a conditional move, like cmovl keeps all the instructions in order and so doesn't pay a large cost for getting the prediction wrong.)

## Principle 2: Understand your problem, and look at data to do so ->
## Principle 3: Specialized algorithms are key to achieve performance

Perf's Top-down microarchitecture analysis classification:
Not Stalled {
  Retiring { Good work }
  Bad Speculation {
    Branch Mispredict: erase wrong path work
    Machine Clear: more drastic restart for things like internal memory violations
  }
}
Stalled {
  Frontend Bound = frontend of fetching/decoding/feeding instructions was bottlenecked
  {
    Fetch Latency: waiting on something
    Fetch Bandwidth: cannot send quickly enought
  }
  Back End Bound = load/store/execution, cache/memory was bottlenecked
  {
    Core Bound: bottleneck in execution
    Memory Bound: bottleneck in data memory, such as cache misses, false sharing, atomic contention, or even pointer chasing
  }
}

## Principle 4: Simplicity is the ultimate sophistication
## Principle 5: Mechanical Sympathy

Immediately Invoked Function Expression (IIFE) -> helpful for const variable with complex initialization.
Use lambdas, functors > std::function, obviously

### Transport: networking & concurrency
Userspace networking > kernel networking
- Kernel networking like send(), recv() has overhead from system calls, data copying, and interrupts
- So we have application directly communicating with the Network Interface Card (NIC): Solarflare is the defacto industry standard
- There are more levels where you can trade code chages, up to a significant rewrite for TCPDirect/ef_vi to get the lowest latency

## Principle 6: True efficiency is found not in the layers of complexity that we add, but in the unnecessary layers we remove
Shared memory: best way for inter-process communication; corresponds virtual address space for different processes
- Latency bounded by cache coherence latency
- The fact we deal with multiple processes in the first place (instead of a single multi-threaded process) is for fault isolation

## Principle 7: Choose the right tool for the right task
Not reviewed in talk, but a lot of difficulties involved to get it working correctly:
- lock-free data structures > sequence locks > locks
- Memory ordering
- Pointers don't work
- Lifetimes are tricky

# Illustrative Example: SPSC lock-free queue
Good example is concurrent queues, with these concerns:
Bounded? : Yes (universal choice), always helpful for predictability, cache-friendliness, backpressure signal
Blocking : No (near-universal choice depending on latency vs cpu budget, prefer latency) 
Type Support: POD (forced by shared memory and copy semantics)
Consumers: Many (by use case)
Message Size: Variable (by use case)
Dispath: Fan Out (by use case)

TLDR: single-producer, single-consumer lock-free byte queue
Stores data in format: [size] [payload] [size] [payload]
Conceptually, writer does:
```cpp
- localCounter += 4 + buffer.size()
- writeCounter.store(localCounter, release)
- memcpy(nextElement, &size, 4)
- memcpy(nextElement + 4, buffer.data(), buffer.size())
- readCounter.store(localCounter, release)
```

Conceptually, reader does:
```cpp
- if(localCounter == readCounter.load(acquire)) return;
- memcpy(&size, nextElement, 4);
- (check) EXPECT(writeCounter.load(acquire) - localCounter <= QUEUE_SIZE)
- memcpy(buffer.data(), nextElement + 4, size)
- update members
- (check) again
```

Improvements for better latency / throughput:
- Cache coherence: instead of publishing the exact write counter, publish a coarser value (implying more space) to minimize change of conflict.
  - Can do fast align with template<S, T> align: some O(1) bit operations
- Use block-alignment padding to where you put the objects in the buffer, similar to built-in alignment padding for classes
- Consumer-side caching of read counter: only reload when consumer consumed everything previously knew

We also want to avoid unnecessary work, such as copying data to a buffer. In a naive API we might have:
```cpp
struct Order { ... };
void SerializeOrder(const Order&, std::span<byte> out) { ... }

Order o {...};
array<byte, SIZE> temp;
SerializeOrder(o, temp);
q.write(temp);
```

But we are serializing the order before we even touch the queue, which means we need to memcpy the data again.

We would prefer to serialize it in-place; to prevent leaking queue internals, we thus use a strategy pattern and pass in our serialization logic as a callable to be applied when we write to the queue:
```cpp
template<class Writer>
void FastQueue::write(int sz, Writer writer) {
  std::span<byte> buffer = getBuffer(sz);
  writer(buffer);
}

Order o{...};
q.write(SIZE, [&](std::span<byte> dest) {
  SerializeOrder(o, dest);
});
```
Note that here, o is captured implicitly by the [&]. This is a design trade-off:
Pros:
- Zero-copy abstraction
- The API is generic and flexible (doesn't need to know about what it's writing).
Cons:
- Caller must know size ahead of time
- Size mismatch possible if writes more/less than requested
- Exposes internal state (must document how dest can be used)
- Less semantically clear, and cost of using templates
The cons are usually alleviated by introducing typed wrappers over this generic function.

Bulk writing; either managed by the user or internal structure
If we're using NUMA architecture, we can make node-local copies of members.
- const metadata like capacity, &buffer can be copied to each side
- Local cached counters (an optimization) can also be copied to each, because they are only relevant to one side
- Shared counters should be put on the node of the thread that reads it, since reads > writes, especially if we adapt the coarser values in previously outlined optimizations. It additionally depends on what the "hot path" is: it is common for one side to spin while waiting for data, so making that operation faster has the greatest overall benefit

## Accuracy in benchmarking low-latency programs
- Best way is to use RAII-like as start/end within a function. So:
```cpp
void foo() {
  ScopedTrace t("foo");
  ...
}
```
Where ScopedTrace does rdtcs() (much faster than std::chrono) in constructor / destructor
(is much better than:
```cpp
auto start = now();
foo();
auto end = now();
```
)
But it is difficult to pinpoint bottlenecks.
-> use clang x-ray instrumentation: can inject whatever you want without recompiling

## Principle 8: Being fast is good - staying fast is better

## Principle 9: Thinking about the system as a whole
Entire program will likely not fit in the L1 cache - but still need to be aware in context of everything




Comment on the video:
This is a great talk, but it didn't mention probably the most important optimizations of order book building.
1) Simple thing: multiply all BID prices by -1. After this you can be sure that a better price is lower price. This removes a
branch misprediction problem which fails in 50% of cases. Or multiply asks by -1, it doesn't matter.
2) Prices are aligned to price step. PriceInSteps=price/priceStep is an int32 variable. One can use it as an index in an array of price levels, so we don't need to store it at all. We need to store only volume in this array. We get even better cache locality and O(1) complexity for all operations. Improvement: limit the size of an order book to 128 elements and use a ring buffer (array with mod 128 arithmetics, which can be implemented as bit mask x|127, which works for both positive and negative numbers.



