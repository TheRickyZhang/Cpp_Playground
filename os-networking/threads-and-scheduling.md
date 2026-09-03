# These are with threads!

Three main states:
- Running
- Runnable
- Blocked

Running <-> Runnable determined by scheduler
Running <-> Blocked: self-triggered like read() on empty socket, scheduler picks who runs next, then the other thread/interrupt handler wakes thread

Both cost a context switch, but blocking has variable wakeup times. Better for throughput, worse for latency.

Mitigate cost:
- Pin thread to a specific core with `sched_set_affinity`, and isolate the core with `isolcpus` (isolate cpus) at boot, so scheduler won't place anything there.

Other things:
- nohz_full stops periodic ticks from being sent out
- Default scheduling is nice value of -20 (important) to 19 (more nice), CPU time proportion exponential per step, while SCHED_FIFO will entirely prioritize important threads.




