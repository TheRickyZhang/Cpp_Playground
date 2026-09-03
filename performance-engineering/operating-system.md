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


