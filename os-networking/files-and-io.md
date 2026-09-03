File descriptor = abstraction over IO, index into table
Table = in KERNEL

-> file description = high-level information: cursor offset, access mode (read/write/both), status flags, read()/write()/close() overrides
- When creating a file / socket / device, the corresponding handling functions are known at compile-time, so we store a pointer to a struct of function pointers in the fd table
- Not all fields are used by all types - for instance, offset is meaningless for a socket

# Reading a file
Initially, file data is in disk, not physical memory (RAM).

- Address translation: hardware raises exception (page fault), restarts instruction
- File reading: user syscall (not found in page cache), fetches from disk

Note that "page" is completely separate from address translation pages.
Page cache = map of {offset, page descriptor} per inode, where you can get physical address from descriptor

If not in page cache, thread makes request to disk, blocks itself (thread adds itself to wait queue, marks as blocked). Then the disk's interrupt handler does waking, and the thread memcpys to buffer. (Generally, reads from disk need to be blocked)

We can bypass the memcpy with DMA
- Just mmap the page cache frame into address space.
- Mmap better for large files with random access (ex databases)

O_DIRECT bypasses the page caches - good for not evicting page cache
Logging typically goes to preallocated mmapped region, and separate thread flushes it.

# Reading a socket
Normally blocks, O_NONBLOCK returns -1 immediately

`poll`: Pass entire fd list on every call. (Max is 1024 fds, bitmask)
`epoll`: register once with `epoll_ctl`, `epoll_wait` will return ready list.
- Generally prefer state-change triggered vs state-triggered, since fewer wakeups



Other:
fsync() actually writes to disk instead of lazily copying from physical memory
