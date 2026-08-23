# PROMPT
I'm trying to effectively design an options exchange. Here are the requirements:

A price-time options exchange in C++ on Linux. Main goal is to work o n the exchange protocol and wire spec.
Inputs loaded at startup: the options universe to trade, and a client whitelist.

Connections: TCP, only whitelisted clients, one session each.
Order types: DAY and IOC, limit only. New and cancel (no cancel-replace)
Quotes: two-sided, atomic both-side replace, post-only, exactly one resting quote per (client, symbol)
Matching: price-time, per options
Market data: multicast feed - trades and top 3 levels per sidne per symbol.

Messages we must support (in order of client -> exchange, exchange -> client):
Log on / logon response
New Order / ACK
Cancel Order / order status (fill, ack, expired)
Mass quote (two-sided) / ACK

The exact wire format - fields, sizes, framing, sequence numbers, error codes - is your design.


I think we should also have the following messages:

Heartbeat, logout, mass cancel, resend / snapshot requests, trade correction.
Design points to consider:
- What messages do we need?
- What types do we use?
- How to handle protocol errors?
- How do we know clients are up?
- What's the wire format & framing?


Additionally, ensure you keep these ideas in mind:
Recording
- Logging
- Auditing
- Replay & determinism
- Observability
- Threading & handoff
- Hot-path discipline
- Startup & shutdown
- Failure handling
- Testing & tooling

My process for designing this is: let's trace the full path of all the messages we support, and try to separate the steps into logically independent categories. Then, once we have a good picture of that, we can start thinking about throughput and fairness, which are the main architectural challenges of this system. Obviously, for a first-pass spec, we do want to keep things relatively simple, but also keep latency / throughput / fairness in mind, such as having dedicated threads and load balancing, etc.

# RESPONSE
