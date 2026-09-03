Act as a mock interviewer for a low-latency C++ quantitative developer role. Interview me the way a real interviewer would: one question at a time, follow-ups based on my answers, no hints unless I'm clearly stuck.

Emphasis: operating systems and networking fundamentals, with C++ and systems design as needed. Start with fundamentals and go deeper only once I've demonstrated them. I will provide the topics.

Rules:

If I say anything incorrect or imprecise, stop and correct it before moving on.
Probe the edges of my knowledge — push until you find where my understanding breaks down.
Don't accept vague answers; ask for mechanisms, tradeoffs, and numbers where relevant.
Stay in character; no summaries or encouragement mid-interview.

Goal: find and expose my gaps so I'm ready for real interviews.

1. Operating systems

You should be able to explain
1. Processes & threads	process vs thread, address spaces, fork, exec, clone, thread creation, process states
2. Virtual memory	virtual → physical translation, pages, page tables, TLB, page faults, demand paging, COW, mmap, swap
3. Syscalls / kernel boundary	user vs kernel mode, syscall vs interrupt vs exception, entering/exiting kernel
4. Scheduling	runnable vs blocked, context switch, preemption, priorities, CPU affinity, scheduler latency
5. Concurrency	mutexes, futexes, atomics, memory ordering, spinlocks, condvars, semaphores, deadlock
6. Files & I/O	file descriptors, open file descriptions, read/write, page cache, mmap, buffered I/O
7. I/O waiting	blocking/nonblocking I/O, select/poll/epoll, level vs edge triggering
8. Interrupts & devices	IRQs, softirqs, DMA, PCIe, interrupt affinity
9. IPC & signals	pipes, shared memory, signals, sockets, wakeups
10. Filesystems/storage	inode/dentry basics, page cache, fsync, persistence; journaling at a high level
11. Performance	cache locality, false sharing, NUMA, context-switch cost, page faults, profiling/tracing

Some distinctions here are especially important.

For example, fork() doesn't simply create an unrelated duplicate: the child inherits file descriptors referring to the same underlying open-file descriptions, while its virtual address space is established using mechanisms such as copy-on-write.

Similarly, mutexes are a good example of crossing abstraction layers: an uncontended Linux mutex can operate entirely in userspace using atomics, while a contended implementation can use futex() to let the kernel sleep/wake threads.

For scheduling, focus more on concepts than memorizing one Linux algorithm. Modern Linux has been transitioning from CFS toward EEVDF since Linux 6.6; the important interview concepts are runnable tasks, run queues, preemption, fairness, priorities, affinity and wakeup latency.

How a syscall works:
CPU switches privilege to kernel (ring 3 -> 0)
Jumps to fixed handler installed on boot
Dispatches through syscall table
Usually takes around 200ns. IO can take microseconds, and blocking can take infinite

An fd is some process handle into kernel-managed IO resource, use with read()/write()/close()
**Examples:**
read() (read/write from fd)
mmap() (anything that allocates memory)
send() (transmit data across sockets)

Note that read already populate contents in page cache by "reading" all the data, while mmap does lazy copy with virtual address space. (has on-demand paging when used)


Thread vs process:
- Threads have different stack and program counters. Share file descriptors, handlers, and address space (virtual -> physical memory map)
- Process is scheduleable unit, has own address space

Context switch (between processes, but can also be among threads with less affect):
Preserved:
- Registers
- Stack/instruction pointer
- page table / task_struct = execution context (pointer-swapped)

Largest performance impacts:
1. Cache warmth
2. TLB flush
3. Branch predictor state

Kernel is privileged software layer
- CPU scheduling
- Virtual memory, page tables
- Networking
- Drivers
- Interrupts

User code transitions through:
- syscalls
- exceptions
- hardware interrupts

Dereference pointer: virtual address -> TLB -> page table -> page -> cache hierarchy
Note that a failed page translation will issue an exception - if access is legal, then get more pages


How does a mutex work?
Spinlock = simplest kind of lock
Mutex = sleep if not available
Futex mechanics should be intuitive, where we only need kernel if we are being put to sleep / being woken up, not if we are the only thread getting the lock

Full path:
syscall to deschedule, syscall to wake, 

How does fork() work?
- It creates another process (NOT thread), so it needs to set up own address space
- Is lazy, so uses copy-on-write: when we actually write in new thread and get a page fault, make a copy and set that as the value
- Only the calling thread is copied to the forked process, so we generally prefer exec() for replacing the old program instead
**Examples:**
- Shells use fork() all the time, for instance ls -> child thread will exec() with /usr/bin/ls
- Implementation use fork(), but generally avoid directly in user code

How does epoll() work?
- Think of epoll event object as maintaining lists for not ready vs ready (will not block)
- We get notified when transitions from not ready -> ready

