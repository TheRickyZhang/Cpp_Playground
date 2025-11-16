*Klaus Iglberger: C++ software design*

C++ Concepts are the static equivalent of base classes - just specifying certain requirements that must be fulfilled

A design pattern is a proven, named solution which expresses a specific intent.
Note that this is distinct from an implementation pattern, such as std::make_unique, which does not allow for decoupling or extending

Dependency injection: modify existing code and inject it from the outside, ex in the constructor / set methods.

Dynamic Polymorphism makes a decision: you can either add types easily, or operations easily.
OOP = easy to add types, procedural = easy to add operations

= Design Patterns
Passing in something: Strategy, Command
Relating components : Bridge,   Adapter
Prevent repetition  : Visitor,  CRTP,  Decorator
The GOAT?           : Type Erasure, External Polymorphism

Various             : Observer, Clone, Singleton
More Obscure:
Memento: Have state snapshots to allow rollback without exposing public members
Chain of Responsibility: Pass requests along a sort of linked list until you get to a node that can handle
Facade: Hide complexity, while still allowing access to internals

Flyweight: Split state into intrinsic (shared memory) and extrinsic (per-instance)
Composite: Single and group things treated the same (ex a group of people in game = single person)
Proxy: Provide a friendly placeholder / interface that goes through real object through proxy
Abstract Factory: When you have related things, and you want to be able to switch them all at once quickly, so package together.
Null: Create child class that does nothing instead of a null object
Builder: For creating complex object, enforce invariants, 
Template: Every instance created the same way, but subclasses can customize a step

In Battlebeyz:
Command: Stack of GameStates
Bridge: Representation of BeybladeBody holding a BeybladeLayer, which could be a SingleLayer, GodLayer, ChoZLayer, etc.
Observer: Once the user activates a signature move, then it calls the correct functions to all relevant beys
Facade: Provide simple GameEngine methods like loadScreen(), changeState()
At SMBC:
Builder: Basically immutable objects

= Strategy
Have interchangeable algorithms, allowing it customizable from the outside; think like passing lambdas.
- Easy addition of new derived objWhat's the difference between a bridge design patterna nd an adapter?ects (preserves strength of OOP)
- Examples: passing in lambda to accumulate, allocator to vector
- Can end up with lots of classes and pointer indirection
- So better when smaller number of implementation details
- Can improve performance using templates, just lose ability to specify at runtime

= Visitor (Cyclic Visitor)
- Instead of creating functions for each class, make them all accept a visitor class. So you only need to change the visitor class to add new behavior
- Double dispatch: virtual picks the element type, overload picks visitor function based on that type
_- Easy addition of new operations by separating behavior to different class (but is workaround for OOP weakness)_
- Not very flexible, violates DRY if very similar functionality. Assumes closed set of types
- Subjectively hard to understand/maintain

class Square : public Shape {
  void accept(ShapeVisitor& v) override {
    return v.visit(this);
  }
}
class ShapeVisitor {
  void visit(const Square& s) { ... }
}

Can be improved by using std::variant. We can use std::visit(func, variant), where func must provide operator() to all of the closed types of the variant.
- Great because we do not need any base class, and are flexible to customize the visitor to whatever types we choose.
- Non-intrusive! No cyclic dependencies

= Command
Turn an action and its parameters into a first-class object
- _Example: package calculator operations with execute(), undo(), log(), ex. AddOperation : public CalculatorOperation_
- TODO: Implement calculator as an example
- Another example is a thread pool class; you don't care what the actual commands you pass into it are, you just need to manage them. This also allows for arbitary categorization

_Note the distinction between Strategy and Command : Strategy is what should be done, and command is how it should be done._
- Thus, there is a lot of overlap in the standard library, for instance.
- Strategy = way of doing something. Usually isn't meaningful without context of class
  - Ex std::sort()
- Command = request to do something. Self-contained, makes sense.
  - Ex std::for_each

Chapter 6 (239)

= Adapter
Relatively self-explanatory, convert between incompatible interfaces
- Quite flexible and nonintrusive; generally prefer object adapters (composition) over class adapters (inheritance)
- In std lib, std::stack, std::queue, for instance, use a std::deque container by default, but the API of something like vector to pop, push, as well.
- As opposed to Strategy, specifies all aspects of behavior, not just one.
- Backbone of generic programming, since you can build on complete interfaces without changing anything there
- But makes it very easy to combine things that don't belong together

= Observer
notify all listeners when something changes
- Push observer: given all information in parameters of the update() function
- Pull observer: given the entire object, then need to pull relevant information
- But even better, let's also pass in a tag signaling anything specific, and move the observing dispatch to templates
*Disadvantages:*
- Order of registration/deregistration, nontrivial handling is problematic
- Using std::function only works well with pull observers

= CRTP (Curiously Recurring Template Pattern)
- No common base class; everything is its own template (all compile-time)
- Note: *Mixin* classes are base classes just for the purposes of injecting specific behavior
- A bit unintuitive and circular, but you can define:
```
template<typename T>
class parentClass {
  ...
  return static_cast<const Derived&>(*this).size();
};

template<typename T>
class derivedClass : public parentClass<derivedClass<T>>
```
- Note that while you may access members, you cannot define return types; instead, do:
decltype(auto) function(params...) { ... }
- Can somewhat improve with concepts, but does not restrict the universe of all types
*Disadvantages:*
- No common base class
- Everything becomes a template, not flexible

= Bridge
Class instances hold reference to implementation and delegate to it
- Structural design pattern; focuses on physical dependencies of abstraction and implementation
- As opposed to usual definition, decouple things by constructing a bridge to separate them
- For instance, if we store SingleLayer in BeybladeBody, instead of directly storing a unique_ptr<SingleLayer>, we instead create abstract base class Layer, and SingleLayer inherit from Layer, storing a Layer in BeybladeBody.
- Compilation Firewall: changes to implementation details should not cause recompilation to higher classes
- Want to use pimpl = pointer implementation (condense member variables into one std::unique_ptr(Pimpl))
  - This seems similar to just creating a DTO but provides a lot of benefits - class no longer needs to be recompiled on data member changes, since it has consistent interface
  - There is some caveats though because Impl is an _incomplete type_; need to define the destructor in source, implement copy constructor/assignment to preserve semantics, define things in source.
  - Cleverly, we can improve performance by only putting less-frequently used members in a pimpl.
*Disadvantages:*
- Overhead from pointer access and no inlining
- Increased code complexity

= Prototype
- Have pure virtual clone() in base, and implement with return std::make_unique(\*this) for all derived
- Just allows for copying when you don't have any other way to get
*Disadvantages:*
- Uses dynamic memory

= External Polymorphism
Allow unrelated classes without virtual methods to be treated polymorphically (???)
Maybe clearer with an example: keep classes independent:
```
class Square {}
class Circle {}

class ShapeConcept {
  virtual void draw()
}
template<typename T>
class ShapeModel : public ShapeConcept {
  using DrawStrategy = function<void(const T&)>;
  ShapeModel(T, DrawStrategy) {}
  void draw const override(drawer(shape)); // delegating it to type
}
```
- Kind of good for everything; very flexible even for like treating an int polymorphically
*Disadvantages:*
- Complicated, does not target efficiency
- Makes it easy to violate expectations

= Type Erasure
Ultimate combination of External Polymorphism, Bridge, and Prototype that generally fulfills all design principles
- Value-based: types that are copyable, movable, and easy to reason about
- Nonintrusive: all types that will match expected behavior are automatically easily supported
- Extendable unrelated set of types: add any types easily
- Potentially Polymorphic: should not need to use inheritance, while still having polymorphic behavior that is decoupled
- Same semantic behavior: emit compile error for types that do not behave as expected
Overall, very similar to External Polymorphism, but we introduce an actual Shape class that contains a pimpl (bridge), and then finish out the copy constructor/assignment using clone() for exact types.

Example: see ./typeErasureShapeImplementation.cpp

*Disadvantages*
- Complex implementation
- Binary operations, like == on objects, is very ugly

- Can also be adapted to non-owning version, which does pull back into reference semantics
- (Too complex, maybe read later)

= Decorator
When you want to flexibly combine multiple different strategies to apply to something (ex pricing logic)
- Indicated by is-a and has-a, since a decorator parent class will modify instance in some way
- Can have multiple things inheriting from DecoratedItem. Then, in construction, pass combination by nested constructors, like:
```
std::unique_ptr<Item> item2(
std::make_unique<Taxed>( 0.19,
std::make_unique<Discounted>( 0.2,
std::make_unique<ConferenceTicket>( "CppCon", 999.0 ) ) ) );
```
Example: std::allocator implementation
- Compared to Strategy, Decorator makes it easy to combine functionality rather than completely switch

*Disadvantages*
- Can have performance overhead from levels in hierarchy
- Easy to use incorrectly

= Singleton
Only have one instance of a class ever
Treat as implementation pattern; it does not introduce any abstractions

Example: Meyers Singleton
- Declare a static instance inside static get() function, and delete all copy constructor/assignment/move

*Disadvantages:*
- Causes artificial dependencies
- Obstructs testability - can't substitute the singleton usage with mock or anything else
- Basically mutable global state

But, can route to make the global aspect configurable by converting to Strategy:
- abstraction/implementation (ex using global memory? Idk)
- Some complicated stuff you can do
