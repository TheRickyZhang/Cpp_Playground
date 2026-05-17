== Concurrency Primitives
Lock vs Lock-free:
*Lock*: use a mutual exclusion primitive to ensure exclusive access. Simpler correctness.
- Preserve invariant by holding lock
- Block with std::condition_variable
- Issues: deadlock, ordering, priority inversion, convoying (long processes increase average wait)
- Use when: low contention / few threads, small critical section
*Lock-free:* built from atomic operations with progress guarantee. Think of smaller scale locks on a fundamental per-thread level.
- Preserve invariant by atomic logic. Often needs extra data.
- Block with atomic::wait() / sleep()
- ABA problem, starvation, hazard pointers for dynamic memory
- Use when: simple enough to reason about.

std::mutex
- Usually not accessed directly, put in unique_lock or lock_guard

std::lock_guard
- Just RAII, holds on to the mutex as long as in scope.

*std::unique_lock, std::condition_variable*
- More flexible version of lock_guard, if you want to defer, check ownership, try locking, etc.
- Mainly use cv.wait(lock, predicate), which is same as (while(!predicate)) { cv.wait(lock); }
- After that, it is guaranteed that predicate = true, and that we hold the lock.
- Prefer unlocking before notifying for performance
https://en.cppreference.com/w/cpp/thread/condition_variable.html
- This example is helpful. We have a worker and main thread; we can "pass" to worker thread by setting ready = true in a lock, then cv.notify_one(). Then, it passes the cv.wait(lock, [](){return ready;}), which does work then "sends back" to main by setting processed = true, lock.unlock(), cv.notify_one(). Main has cv.wait(lock, [](){return processed;}). Finally worker.join() -> merge process back.
- Must use a while loop to ensure condition is satisfied, since it is possible you can have spurious wakeups.

std::counting_semaphore
- Does not follow RAII: Anyone can acquire/release, shared count
- Also has binary_semaphore variant.

std::atomic<>

std::atomic_flag
- Basically atomic<bool>, but guaranteed to be lock free, which is beneficial because being backed by a mutex/OS lock blocks longer and is at risk of deadlock. Does not have load()/store()
