// lib/expected.hpp

#pragma once

#include "common.hpp"
#include "kernel/console.hpp"

#include <type_traits>
#include <utility>

namespace lib
{
template <typename E>
class Unexpected
{
  public:
    Unexpected(const E& value) : value_(value)
    {
    }
    Unexpected(E&& value) : value_(std::move(value))
    {
    }
    [[nodiscard]] auto value() -> E&
    {
        return value_;
    }
    [[nodiscard]] auto value() const -> const E&
    {
        return value_;
    }

  private:
    E value_;
};

template <typename T, typename E>
class Expected
{
  public:
    Expected(const T& value)
    {
        emplace_value(value);
    }
    Expected(T&& value)
    {
        emplace_value(std::move(value));
    }
    Expected(const Unexpected<E>& error)
    {
        emplace_error(error.value());
    }
    Expected(Unexpected<E>&& error)
    {
        emplace_error(std::move(error.value()));
    }

    Expected(const Expected& other)
    {
        copy_from(other);
    }

    auto operator=(const Expected& other) -> Expected&
    {
        if (this == &other)
        {
            return *this;
        }

        copy_from(other);
        return *this;
    }

    Expected(
        Expected&& other
    ) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>)
    {
        move_from(std::move(other));
    }

    auto operator=(Expected&& other) noexcept(
        std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>
    ) -> Expected&
    {
        if (this == &other)
        {
            return *this;
        }

        move_from(std::move(other));
        return *this;
    }

    ~Expected()
    {
        reset();
    }

    template <typename... Args>
    auto emplace_value(Args&&... args) -> T&
    {
        reset();
        new (&storage_.value_) T{std::forward<Args>(args)...};
        initialized_ = true;
        has_value_ = true;
        return storage_.value_;
    }

    template <typename... Args>
    auto emplace_error(Args&&... args) -> E&
    {
        reset();
        new (&storage_.error_) E{std::forward<Args>(args)...};
        has_value_ = false;
        initialized_ = true;
        return storage_.error_;
    }

    auto reset() noexcept -> void
    {
        if (!initialized_)
        {
            return;
        }

        if (has_value_)
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                storage_.value_.~T();
            }
        }
        else
        {
            if constexpr (!std::is_trivially_destructible_v<E>)
            {
                storage_.error_.~E();
            }
        }

        initialized_ = false;
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
    [[nodiscard]] auto value()      const -> const T& { require_value(); return storage_.value_; }
    [[nodiscard]] auto value()            ->       T& { require_value(); return storage_.value_; }
    [[nodiscard]] auto error()      const -> const E& { require_error(); return storage_.error_; }
    [[nodiscard]] auto error()            ->       E& { require_error(); return storage_.error_; }
    [[nodiscard]] auto operator*()  const -> const T& { return value(); }
    [[nodiscard]] auto operator*()        ->       T& { return value(); }
    [[nodiscard]] auto operator->() const -> const T* { require_value(); return &storage_.value_; }
    [[nodiscard]] auto operator->()       ->       T* { require_value(); return &storage_.value_; }
    // clang-format on
    [[nodiscard]] auto has_value() const noexcept -> bool
    {
        return initialized_ && has_value_;
    }
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return has_value();
    }

  private:
    auto require_value() const -> void
    {
        if (!has_value())
        {
            kernel::panic("Expected access without value");
        }
    }

    auto require_error() const -> void
    {
        if (!initialized_ || has_value_)
        {
            kernel::panic("Expected access without error");
        }
    }

    auto copy_from(const Expected& other) -> void
    {
        if (other.has_value_)
        {
            emplace_value(other.storage_.value_);
        }
        else if (other.initialized_)
        {
            emplace_error(other.storage_.error_);
        }
        else
        {
            reset();
        }
    }

    auto move_from(Expected&& other) noexcept(
        std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_constructible_v<E>
    ) -> void
    {
        if (other.has_value_)
        {
            emplace_value(std::move(other.storage_.value_));
        }
        else if (other.initialized_)
        {
            emplace_error(std::move(other.storage_.error_));
        }
        else
        {
            reset();
            return;
        }

        other.reset();
    }

    union Storage
    {
        constexpr Storage()
        {
        }
        ~Storage()
        {
        }

        T value_;
        E error_;
    } storage_{};

    bool initialized_{false};
    bool has_value_{false};
};
}  // namespace lib
