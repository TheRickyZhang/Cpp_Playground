Example Usages:
`/home/ricky/Whitebox/spscqueue.cpp`
`/home/ricky/Whitebox/mpmcqueue.cpp`
`/home/ricky/Whitebox/mpmcstack.cpp`


# Atomics
Compared to locks such as mutexes / condition variables, atomics can have less overhead but more difficult to use
- Bad when there is a lot of cache coherence traffic
- Good when you have rare coordination with relatively simple state

# Memory Orders
Note that std::atomic give you atomicity, and you decide ordering.

Conceptually, modern CPUs have out-of-order execution and cache coherence protocols, and compilers can reorder instructions. So when there are multiple threads, memory orders provide a guarantee to the *compiler and CPU* of what order is preserved.

To test understanding, think about why, for a shared pointer, increment's fetch_add can be relaxed, while decrement's fetch_sub() must be acq_rel. The fetch_add isn't doing any checks so it doesn't care what order it's done in, but since a fetch_sub might delete the object, we must ensure that it has the correct counts.

There are three "main" levels:
- *Relaxed*: You only get atomicity
{
- *Release*: Publish earlier writes now, don't reorder them later
- *Acquire*: Perform later reads now, don't reorder them before
- *Acquire/Release:* precisely for operations that read and write (like fetch_add, exchange);
}
- *Sequential Consistency*: ensures all threads agree on the ordering indicated by seq_cst operations.
Generally, this is not needed in most structures, ie local implementations. But global implementations may need it for syncing: for instance, see the Dekker pattern


# Atomic Properties
- Are not copy / move constructible because if you think about it for two seconds, it would be ambiguous what memory order it should be in.

- Note that the indirection of pointers means that seeing an atomic pointer does NOT mean you can see its contents.
  - relaxed -> only guarantees atomicity of pointer itself
  - release/acquire -> all the writes to the fields are visible as well.
This is NOT a special-case rule; it is very natural when you consider what is actually part of the object, and the fact that release/acquire also reorder ordinary writes.
