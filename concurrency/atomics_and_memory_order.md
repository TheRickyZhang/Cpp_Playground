Example Usages:
`/home/ricky/Whitebox/spscqueue.cpp`

## When to use
Compared to locks such as mutexes / condition variables, atomics can have less overhead but more difficult to use
- Bad when there is a lot of cache coherence traffic
- Good when you have rare coordination with relatively simple state

== Memory Orders
Note that std::atomic give you atomicity, and you decide ordering. There are only three levels: what you choose for the middle one depends on the type of operation it is.
- *Relaxed*: You only get atomicity
{
- *Release*: publish what we currently have  
- *Acquire*: load what other things published
- *Release/Acquire:* precisely for operations that read and write (like fetch_add, exchange);
}
- *Sequential Consistency*: when all threads need to agree on the ordering indicated by seq_cst operations.


## Properties
- Are not copy / move constructible because if you think about it for two seconds, it would be ambiguous what memory order it should be in.
