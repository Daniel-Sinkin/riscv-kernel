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

    constexpr Span(T* ptr, usize size) : ptr_(ptr), size_(size)
    {
    }

    // clang-format off
    [[nodiscard]] auto data()              const -> T*    { return ptr_; }
    [[nodiscard]] auto begin()             const -> T*    { return ptr_; }
    [[nodiscard]] auto end()               const -> T*    { return ptr_ + size_; }
    [[nodiscard]] auto empty()             const -> bool  { return size_ == 0; }
    [[nodiscard]] auto operator[](usize i) const -> T&    { return ptr_[i]; }
    [[nodiscard]] auto size()              const -> usize { return size_; }
    // clang-format on

  private:
    T* ptr_;
    usize size_;
};
}  // namespace lib
