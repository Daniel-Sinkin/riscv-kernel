#pragma once

#include "common.hpp"
#include "kernel/console.hpp"
#include "kernel/heap.hpp"

#include <limits>
#include <type_traits>

namespace lib
{
template <typename T>
    requires(std::is_trivially_copyable_v<T> && std::is_trivially_default_constructible_v<T>)
class Vector
{
  public:
    Vector() = default;

    ~Vector()
    {
        kernel::free(data_);
    }

    Vector(const Vector&) = delete;
    auto operator=(const Vector&) -> Vector& = delete;

    Vector(Vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    auto operator=(Vector&& other) noexcept -> Vector&
    {
        if (this == &other)
        {
            return *this;
        }

        kernel::free(data_);
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    auto operator[](usize idx) -> T&
    {
        if (idx >= size_)
        {
            kernel::panic("Overflow on Vector operator[]");
        }
        return data_[idx];
    }

    auto operator[](usize idx) const -> const T&
    {
        if (idx >= size_)
        {
            kernel::panic("Overflow on Vector operator[]");
        }
        return data_[idx];
    }

    auto at(usize idx) -> T&
    {
        if (idx >= size_)
        {
            kernel::panic("Overflow on Vector at()");
        }
        return data_[idx];
    }

    auto at(usize idx) const -> const T&
    {
        if (idx >= size_)
        {
            kernel::panic("Overflow on Vector at()");
        }
        return data_[idx];
    }

    auto front() -> T&
    {
        if (empty())
        {
            kernel::panic("front on empty Vector");
        }
        return data_[0];
    }
    auto front() const -> const T&
    {
        if (empty())
        {
            kernel::panic("front on empty Vector");
        }
        return data_[0];
    }
    auto back() -> T&
    {
        if (empty())
        {
            kernel::panic("back on empty Vector");
        }
        return data_[size_ - 1];
    }
    auto back() const -> const T&
    {
        if (empty())
        {
            kernel::panic("back on empty Vector");
        }
        return data_[size_ - 1];
    }
    // clang-format off
    auto data()                    ->       T* { return data_; }
    auto data()     const          -> const T* { return data_; }
    auto begin()                   ->       T* { return data_; }
    auto begin()    const          -> const T* { return data_; }
    auto cbegin()   const          -> const T* { return data_; }
    auto end()                     ->       T* { return data_ == nullptr ? nullptr : data_ + size_; }
    auto end()      const          -> const T* { return data_ == nullptr ? nullptr : data_ + size_; }
    auto cend()     const          -> const T* { return data_ == nullptr ? nullptr : data_ + size_; }
    auto empty()    const noexcept -> bool     { return size_ == 0; }
    auto size()     const noexcept -> usize    { return size_; }
    auto capacity() const noexcept -> usize    { return capacity_; }
    // clang-format on

    auto push_back(T x) -> void
    {
        if (size_ == capacity_)
        {
            const auto new_capacity = next_capacity();
            reserve(new_capacity);
        }

        data_[size_] = x;
        ++size_;
    }

    auto pop_back() -> void
    {
        require_not_empty("pop_back on empty Vector");
        --size_;
        data_[size_] = T{};
    }

    auto resize(usize new_size) -> void
    {
        resize(new_size, T{});
    }

    auto resize(usize new_size, const T& value) -> void
    {
        if (new_size < size_)
        {
            for (auto i = new_size; i < size_; ++i)
            {
                data_[i] = T{};
            }
            size_ = new_size;
            return;
        }

        if (new_size == size_)
        {
            return;
        }

        reserve(new_size);
        for (auto i = size_; i < new_size; ++i)
        {
            data_[i] = value;
        }
        size_ = new_size;
    }

    auto assign(usize count, const T& value) -> void
    {
        reserve(count);

        for (auto i = 0zu; i < count; ++i)
        {
            data_[i] = value;
        }

        for (auto i = count; i < size_; ++i)
        {
            data_[i] = T{};
        }

        size_ = count;
    }

    auto reserve(usize new_capacity) -> void
    {
        if (new_capacity <= capacity_)
        {
            return;
        }

        const auto bytes = bytes_for(new_capacity);
        auto* new_data = static_cast<T*>(kernel::realloc(data_, bytes));
        if (new_data == nullptr)
        {
            kernel::panic("Failed to reallocate on Vector::reserve()");
        }

        data_ = new_data;
        capacity_ = new_capacity;
    }

    auto shrink_to_fit() -> void
    {
        if (size_ == capacity_)
        {
            return;
        }

        if (size_ == 0)
        {
            kernel::free(data_);
            data_ = nullptr;
            capacity_ = 0;
            return;
        }

        const auto bytes = bytes_for(size_);
        auto* new_data = static_cast<T*>(kernel::realloc(data_, bytes));
        if (new_data == nullptr)
        {
            kernel::panic("Failed to reallocate on Vector::shrink_to_fit()");
        }

        data_ = new_data;
        capacity_ = size_;
    }

  private:
    static constexpr auto max_count() -> usize
    {
        return std::numeric_limits<usize>::max() / sizeof(T);
    }

    auto next_capacity() const -> usize
    {
        if (capacity_ == 0)
        {
            return 4;
        }

        if (capacity_ >= max_count())
        {
            kernel::panic("Overflow on Vector capacity growth");
        }

        if (capacity_ > max_count() / 2)
        {
            return max_count();
        }

        return capacity_ * 2;
    }

    static constexpr auto bytes_for(usize count) -> usize
    {
        if (count > max_count())
        {
            kernel::panic("Overflow on Vector allocation size");
        }
        return count * sizeof(T);
    }

    auto require_not_empty(const char* message) const -> void
    {
        if (empty())
        {
            kernel::panic(message);
        }
    }

    T* data_{};
    usize size_{};
    usize capacity_{};
};
}  // namespace lib
