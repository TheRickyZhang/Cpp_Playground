To handle refcounts in lock-free structures where we might have dereferenced in another thread before getting to increment the refcount, split into internal / external references
(Don't fully understand yet)
