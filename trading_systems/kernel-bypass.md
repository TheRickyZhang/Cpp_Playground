Elaborate on my involvement in the kernel bypass at SIG.

How kernel bypass works: instead of going through the entire networking stack with recv() and the kernel, we just DMA from the NIC to an RX buffer accessible in user space. (CPU polls on completed descriptor)
TX (transmission) is the opposite process.

Eliminates syscall and scheduler overhead, but means we take responsibility for the entire stack.

Note modern NICs have multiple RX/TX queues, generally for different threads


