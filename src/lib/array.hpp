#pragma once

#include <concepts>

#include "common.hpp"
#include "kernel/console.hpp"

namespace ds_lib
{
template <typename T, usize N>
    requires (N > 0)
class Array
{
  public:
    auto operator[](usize idx) -> T&
    {
        if (idx >= N)
        {
            kernel::panic("[Exception]Overflow on Array operator[]");
        }
        return data_[idx];
    }

    auto operator[](usize idx) const -> const T&
    {
        if (idx >= N)
        {
            kernel::panic("[Exception]Overflow on Array operator[]");
        }
        return data_[idx];
    }

    auto at(usize idx) -> T&
    {
        if (idx >= N)
        {
            kernel::panic("Overflow on Array at()");
        }
        return data_[idx];
    }

    auto at(usize idx) const -> const T&
    {
        if (idx >= N)
        {
            kernel::panic("Overflow on Array at()");
        }
        return data_[idx];
    }

    // clang-format off
    auto front() -> T& { return data_[0]; }
    auto front() const -> const T& { return data_[0]; }
    auto back() -> T& { return data_[N - 1]; }
    auto back() const -> const T& { return data_[N - 1]; }
    auto data() -> T* { return data_; }
    auto data() const -> const T* { return data_; }
    auto begin() -> T* { return data_; }
    auto begin() const -> const T* { return data_; }
    auto cbegin() const -> const T* { return data_; }
    auto end() -> T* { return data_ + N; }
    auto end() const -> const T* { return data_ + N; }
    auto cend() const -> const T* { return data_ + N; }
    static constexpr auto size() -> usize { return N; }
    static constexpr auto empty() -> bool { return N == 0; }
    // clang-format on

  private:
    T data_[N];
};
}
