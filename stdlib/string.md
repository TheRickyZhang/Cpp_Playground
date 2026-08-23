Two main std::string implementations, differing in how they handle the SSO case: 

- gcc: 32 bytes:
```cpp
class string {
  char* ptr; // ptr to heap-allocated, or to local_buf
  size_t size;
  union {
    size_t capacity;    // 8 bytes when on heap
    char local_buf[16]; // Can store up to 15 bytes
  }
};
```

- clang: 24 bytes:
```cpp
class string_long {
  char* ptr;
  size_t size;
  size_t cap;
}
class string_small {
  unsigned char size; // Since size <= 22, use the top bit to indicate this is small
  char buf[23]; // Can store up to 22 bytes
}
```

Core distinction: the gcc cost is paid on transitioning between small <-> large strings, while the clang cost is paid on every data() read: `is_long() ? ptr : buf`


