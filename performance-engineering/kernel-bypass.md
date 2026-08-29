# Prerequisite knowledge:
Normally, CPU code: virtual addresses -> physical RAM address (page tables)
But hardware has:   DMA address       ~= physical RAM address (maybe IOMMU)

Start from the actual payload. Then it gets wrapped in:
- TCP header (note may not preserve delimination)
- IP header
- Ethernet header/ender = largest wrapper
And finally transformed to changing voltage / or modulating light

The RX queue is filled with completed DMA entries - NIC pushes items, and driver consumes (polls) items.
A driver is code that knows how to control a specific device. It is part of the kernel.

- Hard IRQ: Triggered by physical hardware, immediately changes CPU from executing userspace code to kernel code.
- Soft IRQ: Generally run immediately after hard IRQ, in same kernel context for optimization. But can be interrupted by other hard IRQs.
Normal use: hard = notify that NIC queue has work, soft = NAPI = actually processing the packets from the queue

PCIe = high-speed communication lane from CPU (NICs, GPUs)
Driver = translator between linux networking stack and specific hardware
struct sock = Linux's encapsulation of socket state, while struct socket = public higher-level wrapper

Socket: kernel communication endpoint between user processes and kernel networking stack
Note that in user code we have fds, which are only valid in our process's file descriptor table.

Regular model for handling sockets is: epoll_wait on sockets that have data, then recv() on them.
Overall, io_uring generally better, immediately doing some action on data when it's ready and notifying on completion, rather than listing out the ready / recv / do steps. A little more machinery.


# Normal Path (Remember this!)
- NIC gets data as ethernet frames
- They are DMAed into corresponding RX descriptor address in the RX queue
- NIC raises hard IRQ
- CPU stops code execution and runs the NIC driver's interrupt handler in kernel mode, calling napi_schedule
- CPU executes NAPI poll function in softirq context, processing entire RX queue until empty or budget exceeded
- Within the poll, convert RX buffer to kernel's packet representation, process through network stack, determines destination socket, and push data to that socket
- Wakes up threads that call epoll_wait() or recv() corresponding to that socket

Bytes go from:
- NIC -> DMA to kernel -> maybe copy to userspace

Note that most things happen in kernel: when raising hard irq, the CPU stops and 

Normally, from NIC to application, the path is:

NIC:
- receives Ethernet frame, does some initial processing
- sends to specific RX queue
After 
- DMA to memory
- Marks the RX descriptor complete

NIC generates an interrupt / hard IRQ, followed by a soft IRQ, processed by kernel to perform NAPI in loop {
  Polls until empty or budget reached for efficiency
  builds sk_buff (socket buffer = metadata) around packet
  Linux may run GRO (generic received offload), which batches packets to improve throughput at cost of latency. *Turn off* for trading systems
  Recycles RX buffer / descriptor when appropriate (only one of NIC, driver can modify buffer / descriptor)
}

Now it is finally handled by Linux:
- Process Ethernet info, IP info -> UDP or TCP
- UDP: just performs socket lookup
- TCP: handles sequences numbers / duplicates / out-of-order / ACK state
Goes to sock{}.

Now that we have content in the socket, recv() or epoll_wait() successfully wake the corresponding process.
Normally, kernel will copy the packet to userspace, but there are also options to DMA for zero-copy as well, while keeping TCP handling.

# How Kernel Bypass improves
Basically, cuts down everything between NIC receives packet -> code sees packet.

