#include <bits/stdc++.h>
using namespace std;

namespace getcracked {

template <typename T> struct is_small_trait { static inline constexpr bool value=false; };
template <> struct is_small_trait<int>      { static inline constexpr bool value=true; };
template <typename T> inline constexpr bool is_small_v = is_small_trait<T>::value;

class any {
    union Storage {
      // Magic ensuring alignment in this particular use case of small object optimization
        alignas(std::max_align_t) unsigned char soo[sizeof(void*) * 4]; // (4 words)
        void* large;
        Storage() {}
    };

    Storage storage{};
    void* obj = nullptr;
    // Note we need a type-erased destructor here since we need to know the type at runtime (cannot use EBO)
    void (*deleter)(void*) = nullptr;
    const std::type_info* type = nullptr; // nullptr => empty
    bool small = false;                           

public:
    any() = default;
    any(const any&) = delete;
    any& operator=(const any&) = delete;
    any(any&&) = delete;
    any& operator=(any&&) = delete;

    // copy-from-value constructor
    template <class T>
    any(T x) {
        using U = std::decay_t<T>;
        small = is_small_v<U>;
        type = &typeid(U);

        if (small) {
            U* p = std::construct_at(reinterpret_cast<U*>(storage.soo), x);
            obj = p;
            deleter = +[](void* q) { std::destroy_at(static_cast<U*>(q)); };
        } else {
            U* p = new U(x);
            storage.large = p;
            obj = p;
            deleter = +[](void* q) { delete static_cast<U*>(q); };
        }
    }

    ~any() {
        if (deleter) deleter(obj);
    }

    template <class T>
    const T& any_cast() const {
        if (!type || *type != typeid(T)) throw std::bad_cast();
        return *static_cast<const T*>(obj);
    }

    // Optional helper (not required by the prompt)
    const std::type_info& type_info() const noexcept { return type ? *type : typeid(void); }
};

static_assert(sizeof(any) <= 80, "getcracked::any must be <= 80 bytes");

} // namespace getcracked

