= Values of Types:
Great SO post about move semantics: https://stackoverflow.com/questions/3106110/what-is-move-semantics/11540204#11540204
lvalue  - "literal" value, is in memory.      Such as int x. Can be referred to with & or &&.
prvalue - ephemeral value, not in memory.   Such as function(x()) Declared inline
xvalue  - literal value about to be freed.  Such as move(x). Produced by move() or static_cast\<T&&>, which transfers to &&.
glvalue = lvalue | xvalue
rvalue = prvalue | xvalue

Note the distinction between _types_ and _expressions_. While these can be constructed/assigned with different values, all named expressions are lvalue.
T& is an lvalue reference, meaning only binds to lvalues.
T&& is an rvalue reference, meaning binds only to rvalues, and extends its lifetime with new variable.
Note a named T&& is an lvalue.

move(): casts to xvalue (does not actually move anything).
- When assigning from a move, the type/behavior you get for resulting object depend on declaration:
T x        = std::move(t): lvalue, owns the object. Tries move constructor, else copy constructor
T& x       = std::move(t): illegal to bind non-const lvalue to rvalue 
const T& x = std::move(t): lvalue, does not own, and cannot move from
T&& x      = std::move(t): lvalue, does not own, can move from 

