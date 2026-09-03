Load, add, store.

You would want atomic with `fetch_add()` (does read-modify-write instruction under hood)

Cache contention: goes from ~20ns -> 200ns
False sharing = bad version, fix with align_as(hardware destructive interference size)

A mutex makes region of code exclusive. Similar to atomics, but because wait can be arbitrarily long it is a longer case.
- Typically combined with `lock_guard` and `unique_lock` for RAII.
- Implemented with boolean flag in user memory (futex), do CAS
- pass: set true, 10ns
- fail: requires syscall: pass flag address and this thread, if still false, add address to queue that will be notified when address value changes

Memory ordering
- Relaxed:
- Acquire: read, so all writes before this happen before this. 
- Release: write, so all reads after this happen after this
- Sequential: there is one total order all threads agree on
Note that acquire-release must be used in pairs, otherwise useless.

Compiler will reorder at build time, while CPU reorders at run time
- Memory ordering affects both! Affects compiler, and emits CPU barrier instruction
- on x86, acquire-release free because already done by default
- (ARM's model is weaker, meaning more correctness pushed to software)

Condition variables handle wakeups: (cv.notify())
Need to wait in while loop because:
- Spurious wakeups: condition can just be false
- Stolen wakeups: third thread can get mutex between wake and mutex acquired

Deadlock:
- Circular wait (mutual exclusion, hold and wait, no preemption) -> fix with lock ordering

