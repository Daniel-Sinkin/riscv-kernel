// lib/array.hpp

#pragma once

#include "common.hpp"
#include "kernel/console.hpp"

namespace lib
{
template <typename T, usize N>
    requires(N > 0)
class Array
{
  public:
    auto operator[](usize idx) -> T&
    {
        if (idx >= N)
        {
            PANIC("Overflow on Array operator[]");
        }
        return data_[idx];
    }

    auto operator[](usize idx) const -> const T&
    {
        if (idx >= N)
        {
            PANIC("Overflow on Array operator[]");
        }
        return data_[idx];
    }

    auto at(usize idx) -> T&
    {
        if (idx >= N)
        {
            PANIC("Overflow on Array at()");
        }
        return data_[idx];
    }

    auto at(usize idx) const -> const T&
    {
        if (idx >= N)
        {
            PANIC("Overflow on Array at()");
        }
        return data_[idx];
    }

    // clang-format off
    [[nodiscard]] auto front()  const -> const T& { return data_[0]; }
    [[nodiscard]] auto front()        ->       T& { return data_[0]; }
    [[nodiscard]] auto back()   const -> const T& { return data_[N - 1]; }
    [[nodiscard]] auto back()         ->       T& { return data_[N - 1]; }
    [[nodiscard]] auto data()   const -> const T* { return data_; }
    [[nodiscard]] auto data()         ->       T* { return data_; }
    [[nodiscard]] auto begin()  const -> const T* { return data_; }
    [[nodiscard]] auto begin()        ->       T* { return data_; }
    [[nodiscard]] auto cbegin()       -> const T* { return data_; }
    [[nodiscard]] auto end()    const -> const T* { return data_ + N; }
    [[nodiscard]] auto end()          ->       T* { return data_ + N; }
    [[nodiscard]] auto cend()         -> const T* { return data_ + N; }
    // clang-format on

    [[nodiscard]] static constexpr auto size() -> usize
    {
        return N;
    }
    [[nodiscard]] static constexpr auto empty() -> bool
    {
        return N == 0;
    }

  private:
    T data_[N];
};
}  // namespace lib
