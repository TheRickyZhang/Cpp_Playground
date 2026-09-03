To have shared memory, use mmap(). Just costs cache coherence.
- To detect, spin on sequence number in shared region
```cpp
struct SpscRing { // Typically use case for shared memory is having readers / writers
  int* buf = mmap(...)
  alignas(64) atomic<size_t> head, tail;
}
```

Signals = SIGINT, SIGTERM, etc
- Basically, forced function call injected to control flow
- User writes signal handlers (can be called at any time), so must be safe to call even if already called (reentrant)
  - safe: write, read, kill
  - unsafe: malloc, free

Generally, you want to block signals on every thread except one dedicated handler thread.

Pipes:
Two fds connected by a ring buffer
So `ls | grep` will fork, and wire ls stdout to grep stdin
Costs 2 syscalls + 2 copies: 1/1 for input, 1/1 for output

Unix Domain Sockets:
Implements socket semantics, similar to pipe but bidirectional and supporting many connections

Use mmap(MAP_SHARED) for ipc, since will immediately reflect in physical memory
