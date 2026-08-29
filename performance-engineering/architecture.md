NUMA = non-uniform memory access
- It is an access model; most often referred to as NUMA system / machine / node
- Memory on closer nodes is faster to access than that of further nodes
- Uniform memory access is simpler, but NUMA becomes worth it when scaling up, particularly in high-throughput multithreaded systems.

Instruction Set Architecture (ISA): The vocabulary/guidelines associated with hardware
- x86-64 (Intel, AMD): legacy/mature, is a bit messy and power-inefficient
- ARMv9 (Apple+): efficient and well-designed, weaker memory model (be more specific with memory orderings)
- RISC-V: (Open Source): Good for academia, embedded, research
Microarchitecture: specific design of an ISA
A good analogy is between the C++ standard and various implementation of the standard

SIMD = single instruction, multi data
Implemented with larger-size SIMD registers, widths depend on instruction set:
SSE = 128 bits (4 ints/instruction), AVX2 = 256 bits, AVX-512 = 512 bits, aka xmm/ymm/zmm registers. Remember to specify -march=X for the right machine architecture

Larger registers generally better but can sometimes require more shuffling.

