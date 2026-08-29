Key question:
How do you make multi-threaded systems deterministic?

- Try to make scheduling irrelevant by giving each thread a predetermined chunk of work, and make combination step deterministic
- Introduce sequence numbers
- Partition mutable state among threads
