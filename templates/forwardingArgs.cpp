#include <memory>

// Note that ... is not a type, it is special syntax meaning *repeat this pattern*
namespace ricky
{
    // Multiple typenames: typename U, typename V, ...
    // Args now means the pattern of U, V, ...
    template<typename T, typename... Args>
    // Here we want to pass in U&&, V&&, ... so we do Args&&... args
    // args now means the pattern of U&&, V&&, ...
    std::unique_ptr<T> make_unique(Args&&... args) {
        // But here we want to pass in T(forward(U), forward(V), ...) - since we forward each, we put the ... after.
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
