#pragma once
#include <functional>
#include <optional>

namespace utils {
template <typename T> class Lazy {
    using Initializer = std::function<T()>;

  public:
    template <typename F,
              typename = std::enable_if_t<std::is_invocable_r_v<T, F>>>
    explicit Lazy(F &&f) : initializer(std::forward<F>(f)) {}

    inline T &get() noexcept {
        if (!value.has_value()) [[unlikely]] {
            value = initializer();
        }
        return *value;
    }

    inline const T &get() const noexcept {
        if (!value.has_value()) [[unlikely]] {
            value = initializer();
        }
        return *value;
    }
    inline T &operator*() noexcept { return get(); }

    inline const T &operator*() const noexcept { return get(); }

    inline T *operator->() noexcept { return &get(); }

    inline const T *operator->() const noexcept { return &get(); }

    void reset() { value.reset(); }

  private:
    Initializer initializer;
    std::optional<T> value;
};
} // namespace utils
