Listening socket:
Think as {ip, port} reception desk
Internally, it contains a SYN queue for connections pending final ACK, and an accept queue that just waits for userspace processes to call accept().

Note that CPU socket is totally different concept, which is hardware that contains cores.
