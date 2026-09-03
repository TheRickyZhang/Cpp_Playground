- dentry = directory entry, {parent, file name} -> inode
- inodes are originally on-disk (in inode table) and also as a struct in inode cache (also contains runtime info).

`read(path, flags)`
Kernel parses file path

Directory is just a pair of (name, inode), so fast-path is dentry cache
- avoids linear scan (or logarithmic scan over B-tree)
- avoids needing to fetch inodes from disk

Kernel writeback threads walk dirty lists to write in-memory inodes to disk
(fsync is synchronous with hardware, but note you also need to sync the directory as well as the file)
Note journaling as a method to guarantee no lost work on power failure

Actual design for writing market data:
- Normal writeback will block if queues are overwhelmed
- So, push writes into preallocated ring buffer
- Preallocate file to never hit allocations, use asynchronous IO
Use sequence number / drop count if still hitting

`write()` steps:
- Already have inode from open()
- look into page cache, on miss request kernel's page allocator for new frame
