= Concurrency

== Concurrency and Threads
Use threads for parallelism and avoid blocking IO

Differences:
- Retain same address space
- Have multiple stacks on the address space (thread-local storage)

Race Conditions = when unfortunate timing can lead to incorrect state
- Critical section = when race conditions can occur
- Mutual Exclusion = only one thread can execute at the critical section at a time

Main Problems of concurrency:
- Accessing shared variable state
- Dependence of one thread to execute after another

== Thread API (Updated for modern C++)
You can get everything in <thread>, <mutex>, <condition_variable>, <future>

=== std::thread
A thread will need to basically call a function. So you can construct with std::thread(callable, params...) where first argument is a func, lambda, functor, etc.

- By default, thread() will decay the types (pass by copy)
  - unless you use std::ref (which can generally be avoided with idiomatic design)
- You can also move into a thread; then the object will be empty in the main thread
- join() -> block until finish, detach() -> runs independently

_Things to be careful about:_
- With a member function, you need to pass pointers to it and the object: std::thread(&Class::func, &instance, params...)
- Threads can outlive the stack frame they are created, so references can become undefined

=== std::async
To actually get back results, use one of: \
`auto fut = std::async(std::launch::async, func, params...)` \
`
promise<int> p, future<int> f = p.get_future();
std::thread t(worker, std::move(p)); // Need to transfer ownership
`

=== std::mutex
Ensures only one thread can enter a section at a time. Always want to wrap with _lock_guard_ to automatically manage scope of lock/unlock.
`
std::mutex m()
std::lock_guard lock(m); /* Name doesn't matter, just out of scope -> unlock() {
  Whatever in here is guarded
} */
`

Also, we can use _std::unique_lock_ lock(m), which has a superset of functionality to lock_guard.
- Can manually lock/unlock, have more precise timing, etc.

=== std::condition_variable (waiting with predicate)
- Avoids busy waiting and data race
std::condition_variable cv;
Producer might do: cv.notify_one();
Consumer does: {
  unique_lock<mutex> lock(m);
  cv.wait(lock, [](){...}) // Maybe we wait for some container to be populated
}

== Locks
Goals of locks:
- Mutual exclusion (duh)
- Fairness: does any thread starve?
- Performance

=== Spin-locks
Made possible by the test-and-set instruction
- Bad: wastes CPU time, not fair
- Good: stays on the hot path.
  - Need a preemptive scheduler for single processor.

=== Ticket Lock
Fetch-and-add
Uses tickets to ensure each thread progresses; fair

Compare-and-swap
Yield -> basically de-schedule itself

== Locked Data Structures
to be careful with acquisition and release of locks around control flow changes; that enabling more concurrency does not necessarily increase performance; that performance problems should only be remedied once they exist. 

...

== Semaphores
Conceptually, an integer representing how many instances available that supports:
- release() : Increment
- acquire() : Delayed Decrement
Differs from mutexes in that:
- acquire(), release() not tied to the same thread
- No owning thread

Getcracked Questions:
Lock-free data structures can use Reference Counting, Garbage Collection, and Hazard Pointers to manage memory

Cache Contention: if you have multiple thread writing to adjacent memory (ex vector, elements in same cache line), it can cause *False Sharing*
  - Line bounces between cores constantly
Load Imbalance: if some threads take much longer to complete than others
Sequential Bottleneck: Avoid any sequential steps (like combining the results of all threads), instead prefer parallelizable able tree reduction
Hazard Pointers: Signals if memory is in use by another thread
ABA Problem: When a Compare-and-swap cannot detect a state change of A -> B -> A
Lock-free vs Wait-free:
- Lock-free means someone gets to eat eventually
- Wait-free means no starvation; everyone gets to eat eventually
Coarse-grained vs Fine-grained locking
Task Parallelism (different tasks) vs Data Parallelism (same task, different data)

Beware of partial state that can happen when exception during lock_guard
Beware of providing ownership of supposed locked variables, as they can be referenced after the lock guard finishes
Must guard against spurious wakes, when thread unexpectedly wakes up (allowed by C++ standard since implemented with OS/futex) even if the condition variable isn't met.
