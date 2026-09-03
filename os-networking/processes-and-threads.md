1. Processes & threads
process vs thread, address spaces, fork, exec, clone, thread creation, process states

Different across everything:
- Program counter, stack pointer, registers
- Scheduling priority (own nice value, scheduling class)
- thread_local storage (more granularity, avoids contention)

Shared across threads, different across processes:
- Address space
- File descriptor / signal handler table

Shared across processes:
- Physical memory
- Can use shared memory and sockets to communicate

Cost of context switch between on the same vs different processes
(note context switches is always between threads)
- Since TLB only valid in one address space, need to flush everything (but with PCID optimization, still pay eviction cost)
- Cache is tagged with physical memory, so we only pay the cost of new entries pushing old ones out with limited size (ping pong penalty)
