#include <iostream>
#include <memory>
#include <utility>
#include <vector>

//------------------------------------------------------------
// 1. Plain, non-polymorphic domain types
//------------------------------------------------------------

class Circle {
public:
    explicit Circle(double radius)
        : radius_(radius) {}

    double radius() const { return radius_; }

private:
    double radius_;
    // ... more circle-specific data/methods
};

class Square {
public:
    explicit Square(double side)
        : side_(side) {}

    double side() const { return side_; }

private:
    double side_;
    // ... more square-specific data/methods
};

//------------------------------------------------------------
// 2. Internal polymorphic interface + model (implementation detail)
//    - This is the "external hierarchy" the book talks about.
//    - It lives in a detail namespace; users never touch it directly.
//------------------------------------------------------------

namespace detail {

// Abstract base: what operations a polymorphic "erased shape" supports.
// 
// Note: names (draw, clone) are internal; they do NOT impose names on Circle/Square.
// The requirements on ShapeT are defined by the MODEL implementation, not by these names.
class ShapeConcept {
public:
    virtual ~ShapeConcept() = default;

    // Required operation: "draw this shape"
    virtual void draw() const = 0;

    // Required operation: "clone this polymorphic object"
    virtual std::unique_ptr<ShapeConcept> clone() const = 0;
};

// Templated model: owns a concrete ShapeT and a DrawStrategy.
// - ShapeT: the concrete domain type (Circle, Square, etc.).
// - DrawStrategy: any callable with signature void(ShapeT const&).
//
// This is the "glue": it knows how to turn a specific ShapeT into a ShapeConcept.
template<typename ShapeT, typename DrawStrategy>
class OwningShapeModel : public ShapeConcept {
public:
    OwningShapeModel(ShapeT shape, DrawStrategy drawer)
        : shape_(std::move(shape))
        , drawer_(std::move(drawer)) {}

    void draw() const override {
        // Delegate to the strategy; ShapeT just needs to be usable by drawer_.
        drawer_(shape_);
    }

    std::unique_ptr<ShapeConcept> clone() const override {
        // Prototype-style copy: exact copy of this model, including shape_ and drawer_.
        return std::make_unique<OwningShapeModel>(*this);
    }

private:
    ShapeT shape_;        // stored by value: value semantics for Shape
    DrawStrategy drawer_; // how to draw this specific ShapeT
};

} // namespace detail

//------------------------------------------------------------
// 3. Public type-erased wrapper: Shape
//    - Value type, owns the erased object via unique_ptr<ShapeConcept>.
//    - Users see only Shape, not the internal hierarchy.
//------------------------------------------------------------

class Shape {
public:
    // Templated constructor: accept any ShapeT + DrawStrategy.
    // This is where we "instantiate" the model and erase its type.
    template<typename ShapeT, typename DrawStrategy>
    Shape(ShapeT shape, DrawStrategy drawer) {
        using Model = detail::OwningShapeModel<ShapeT, DrawStrategy>;
        pimpl_ = std::make_unique<Model>(std::move(shape), std::move(drawer));
        // After this, the concrete type (Circle/Square/...) is no longer visible
        // outside: only stored behind a pointer to ShapeConcept.
    }

    // Copy constructor: use virtual clone() to copy unknown concrete type.
    Shape(Shape const& other)
        : pimpl_(other.pimpl_ ? other.pimpl_->clone() : nullptr) {}

    // Copy assignment: Copy-and-swap for strong exception safety.
    Shape& operator=(Shape const& other) {
        if(this != &other) {
            Shape tmp(other);
            pimpl_.swap(tmp.pimpl_);
        }
        return *this;
    }

    // Move operations: default are fine thanks to unique_ptr.
    Shape(Shape&&) noexcept = default;
    Shape& operator=(Shape&&) noexcept = default;

    ~Shape() = default;

    // Hidden friend free function:
    // - Keeps "draw" as a free function, not a member, to reduce coupling.
    // - Still has access to internals via 'friend'.
    friend void draw(Shape const& shape) {
        if(shape.pimpl_) {
            shape.pimpl_->draw();
        }
    }

private:
    // Bridge to the implementation:
    // - This is the "pointer to erased type" (the pImpl + type-erasure combo).
    std::unique_ptr<detail::ShapeConcept> pimpl_;
};

// 4. Example usage
int main() {
    auto circleDrawer = [](Circle const& c) {
        std::cout<<"Drawing circle with radius "<<c.radius()<<"\n";
    };

    auto squareDrawer = [](Square const& s) {
        std::cout<<"Drawing square with side "<<s.side()<<"\n";
    };

    // Build a heterogeneous collection of Shapes.
    std::vector<Shape> shapes;
    shapes.emplace_back(Circle{1.5}, circleDrawer);
    shapes.emplace_back(Square{2.0}, squareDrawer);
    shapes.emplace_back(Circle{3.0}, circleDrawer);

    // Use them polymorphically via type-erased interface.
    for(auto const& sh : shapes) {
        draw(sh); // free function dispatch → virtual call → correct drawer_
    }

    // Value semantics demo: copy/move still work.
    Shape a = Shape(Circle{4.2}, circleDrawer);
    Shape b = a;       // copy
    Shape c = std::move(a); // move
    draw(b);
    draw(c);
}

