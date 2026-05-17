== Exception Safety
Aside from noexcept, basic vs strong guarantee must be done semantically, and is important for generic code / concurrency.

It is part of the contract of any data structure; what's allowed to throw, and what is still guaranteed to be true?
- Basic Guarantee: the structure is still usable
- _Strong guarantee_: the structure is unchanged on failure
  - Don't commit until you're done preparing
  - Handle by code structure, rollback, or preconditions (ex. is_nothrow_destructable_v)
- No-throw: enforce via noexcept.

Consider: invariants, commit points, what and where potential throws, and do they mutate existing state?
Pay attention to construction and move assignment/construction, which may throw and modify state.

=== Examples:
Noexcept:
- Destructors: Since when called during stack unwinding (other exception thrown), would create another exception -> must terminate
- std::swap(): if T is nothrow_move_constructible, since it is the commit step in strong guarantees patterns.
- If something does copies, constructs, or moves in the body logic (NOT the function overall, ie move constructor can be noexcept), it probably cannot be noexcept
Strong: 
- vector push_back() / emplace_back(): move elements if noexcept, else do a copy. Does rollback and propagates error if fails.
Basic:
- vector erase() / insert(): anything that must mutate existing elements.
- sort(): rollback is not practical, so would get partial reordering
