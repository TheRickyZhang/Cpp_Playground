https://www.youtube.com/watch?v=BP6NxVxDQIs&list=PLiDK46vnsTT-JerJ8k5-W5L-yKvSu0M41&index=6
By Timur Doumler

To make C++ fast is to know the hardware.
Have mental model of (CPU + Registers) <-> L1 <-> L2 <-> L3 <-> Memory
Prefetcher between CPU and L1 cache can get in advance if you have a pattern of access

Recall cache associativity - fully associative is most flexible more expensive and slower.
https://www.youtube.com/watch?v=BP6NxVxDQIs&list=PLiDK46vnsTT-JerJ8k5-W5L-yKvSu0M41&index=6
By Timur Doumler

To make C++ fast is to know the hardware.
Have mental model of (CPU + Registers) <-> L1 <-> L2 <-> L3 <-> Memory

Recall cache associativity: when N-way associativity, certain "jumps" can cause conflicts.
