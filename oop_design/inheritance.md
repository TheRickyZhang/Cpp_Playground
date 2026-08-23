How vtables work for polymorphic inheritance:
If we have abstract class (virtual), we put vtable pointer as the first 8 bytes
vtable contains:
- pre-content entries: offset-to-start (may vary based on multiple inheritance), runtime type information (RTTI)
- Method addresses
- Destructors (may be multiple versions, beyond current scope)


Private inheritance: acts like composition because you can only use class like an outsider
- except for protected methods, overriding virtual functions, EBO

Which function types can be virtual?
Think about how a virtual function is called: We go to the vtable ptr of the object and select a specific entry. For polymorphic classes, 
