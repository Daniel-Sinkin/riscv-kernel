// lib/optional.hpp

#pragma once

#include "common.hpp"
#include "kernel/console.hpp"

#include <type_traits>
#include <utility>

namespace lib
{
template <typename T>
class Optional
{
  public:
    // clang-format off
    Optional() = default;
    Optional(const T& value) { emplace(value); }
    Optional(T&& value) { emplace(std::move(value)); }
    // clang-format on

    Optional(const Optional& other)
    {
        copy_from(other);
    }

    auto operator=(const Optional& other) -> Optional&
    {
        if (this == &other)
        {
            return *this;
        }

        copy_from(other);
        return *this;
    }

    Optional(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        move_from(std::move(other));
    }

    auto operator=(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> Optional&
    {
        if (this == &other)
        {
            return *this;
        }

        move_from(std::move(other));
        return *this;
    }

    template <typename... Args>
    explicit Optional(std::in_place_t, Args&&... args)
    {
        emplace(std::forward<Args>(args)...);
    }

    ~Optional()
    {
        reset();
    }

    template <typename... Args>
    auto emplace(Args&&... args) -> T&
    {
        reset();
        new (&storage_) T{std::forward<Args>(args)...};
        has_value_ = true;
        return *ptr();
    }

    auto reset() noexcept -> void
    {
        if (!has_value_)
        {
            return;
        }

        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            ptr()->~T();
        }
        has_value_ = false;
    }

    template <typename U>
    [[nodiscard]] auto value_or(U&& fallback) const -> T
    {
        if (has_value_)
        {
            return value();
        }
        return static_cast<T>(std::forward<U>(fallback));
    }

    // clang-format off
    [[nodiscard]] auto value()      const -> const T& { require_value(); return *ptr(); }
    [[nodiscard]] auto value()            ->       T& { require_value(); return *ptr(); }
    [[nodiscard]] auto operator*() const  -> const T& { return value(); }
    [[nodiscard]] auto operator*()        ->       T& { return value(); }
    [[nodiscard]] auto operator->() const -> const T* { require_value(); return ptr(); }
    [[nodiscard]] auto operator->()       ->       T* { require_value(); return ptr(); }
    [[nodiscard]] auto has_value() const noexcept -> bool { return has_value_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_value_; }
    // clang-format on

  private:
    struct alignas(T) Storage
    {
        Byte bytes[sizeof(T)];
    };

    auto require_value() const -> void
    {
        if (!has_value_)
        {
            kernel::panic("Optional access without value");
        }
    }

    auto copy_from(const Optional& other) -> void
    {
        if (other.has_value_)
        {
            emplace(*other);
        }
        else
        {
            reset();
        }
    }

    auto move_from(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> void
    {
        if (other.has_value_)
        {
            emplace(std::move(*other));
            other.reset();
        }
        else
        {
            reset();
        }
    }

    // clang-format off
    [[nodiscard]] auto ptr() const -> const T* { return reinterpret_cast<const T*>(&storage_); }
    [[nodiscard]] auto ptr()       ->       T* { return reinterpret_cast<T*>(&storage_); }
    // clang-format on

    Storage storage_{};
    bool has_value_{false};
};
}  // namespace lib
