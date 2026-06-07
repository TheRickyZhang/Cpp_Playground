Multiple levels of granularity can implement locking at.
Need to support operations: at(), insert()
Steps of: call -> hash -> mode size -> probe? -> res

Entire structure per call: trivial
Striping specific bucket groups: simple
Heavy reads: shared_lock
Heavy writes: striping

Resizing:
Global lock: trivial, has latency spikes
Incremental resizing / avoid resizing

Lifetime:
Hazard pointers
Epoch-based reclamation
Read-Copy-Update: EBR but optimized for read-heavy workflows
- use versions with specific removal rule instead of general cleanup
Reference counting: 
