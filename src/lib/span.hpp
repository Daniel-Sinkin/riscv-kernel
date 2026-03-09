// lib/span.hpp

#pragma once

#include "common.hpp"
namespace lib
{
template <typename T>
class Span
{
  public:
    constexpr Span() = default;

    constexpr Span(T* ptr, usize size)
        : ptr_(ptr)
        , size_(size)
    {
    }

    // clang-format off
    auto data()              const -> T*    { return ptr_; }
    auto begin()             const -> T*    { return ptr_; }
    auto end()               const -> T*    { return ptr_ + size_; }
    auto empty()             const -> bool  { return size_ == 0; }
    auto operator[](usize i) const -> T&    { return ptr_[i]; }
    auto size()              const -> usize { return size_; }
    // clang-format on

  private:
    T* ptr_;
    usize size_;
};
}  // namespace lib
