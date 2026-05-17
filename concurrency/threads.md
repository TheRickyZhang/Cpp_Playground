Prefer using std::jthread (joinable thread) over std::thread in all modern C++ code.

Examples uses that require concurrency: logging, async file writer, autoclicker reacting to key presses, anything that needs to run in the "background".

Usually, thread names are based on their role (listener, worker) and suffixed with _thread.

std::condition_variable only works with std::mutex, but is also optimized more as a result.
std::condition_variable_any works with any lockable object.

std::stop_token is a modern replacement for stopped booleans:
`while(!stopped) { ... } -> while(!st.stop_requested)`

Note that when using condition variables, it is better to notify outside the mutex scope to prevent unnecessary block:
```cpp
// bad
{
  lock_guard<mutex> lk(m);
  held = curr_held;
  cv.notify_all(); // would wake the worker, even though we still hold the lock
}

// good
{
  lock_buard<mutex> lk(m);
  held = curr_held;
  changed = true;
}
if(changed) cv.notify_all();
```

Put threads last in class declaration order, since they likely refer to other members, and members are destroyed in reverse declaration order.

Active Object vs thread-safe class design.
Deciding whether to include `std::jthread` members within the class dictates ownership. If we include, then we necessarily control them.
If we don't, the class can still be thread-safe, ie ensuring methods called by different threads work implicitly through use of flags / condition variables / mutexes. But there still may be a contract that certain methods must be called by certain threads in a certain order.

One way to enforce the contract at compile-time is to split methods by their intended caller thread to their own classes, with a pointer to shared state.



