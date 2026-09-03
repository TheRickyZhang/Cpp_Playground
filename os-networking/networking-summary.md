2. Networking

I would divide networking into these levels:

Area	You should be able to explain
1. Layering / encapsulation	application → TCP/UDP → IP → Ethernet; what headers each layer adds
2. Ethernet / L2	MAC addresses, Ethernet frames, switches, MTU, VLAN basics
3. IP / L3	IP addresses, subnetting, routing, TTL, fragmentation, ICMP, ARP/neighbor discovery
4. UDP	datagrams, lack of reliability/order, checksum, multicast basics
5. TCP	handshake, sequence numbers, ACKs, retransmission, ordering, connection teardown
6. TCP control mechanisms	receive window/flow control vs congestion control, RTO, fast retransmit, slow start
7. Socket API	socket/bind/listen/accept/connect/send/recv, socket buffers
8. I/O multiplexing	epoll, readiness, blocking/nonblocking
9. Kernel packet path	NIC → DMA → RX queue → IRQ/NAPI → network stack → socket → application
10. NIC architecture	RX/TX rings, descriptors, DMA, multiple queues, MSI-X
11. Network performance	RSS, affinity, interrupt coalescing, GRO/GSO/TSO, busy polling, buffer sizes
12. Advanced low latency	kernel bypass, AF_XDP, DPDK, ef_vi, Onload, RDMA
