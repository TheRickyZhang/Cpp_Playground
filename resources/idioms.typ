Remember SOLID:
S: Single Responsibility
O: Open Closed Principle             - Document class; easy to add to, hard to modify
L: LSP Liskov Substitution principle - Rectangle/square class; derived objects (is-a) should be directly substitutable, following all the assumptions of parent.
  - Cannot strengthen or weaken conditions in child
  - Return types are covariant: subset (enforced in C++)
  - Function parameters are contravariant: superset (not enforced)
I: Interface Segregation Principle   - Do not depend on interfaces that aren't used 
D: Dependency Inversion Principle    - Depend on abstractions over the concrete

Acronyms:
ADL: Argument dependent lookup (doing using std::begin in scope of function, for instance, to look in std scope after trying specific scope)
RAII: Resource Acquisition is Initialization: very intuitive, acquiring/releasing resources should match with the lifetime
KISS: Keep it simple, stupid.

prefer using:
  using std::swap;
  swap(x, y);
over:
  std::swap(x, y);

Because the former allows compiler to choose better, type-specific swap implementation.

== Concurrency:
- Keep synchronization logic simple
- Minimize shared mutable state
- Use RAII for locks
- Use condition variables or atomics
