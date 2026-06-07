Two types of profiling:

Sampling profiling: taking random samples of stack trace to probabilistically measure how long we spend in each function
- Cons: inline functions are invisible

Instrumentation profiling: add hooks to explicitly record metrics
- Cons: more comprehensive, but more expensive and difficult to implement

In practice, use mostly instrumentation with some sampling
