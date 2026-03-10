// lib/vector.hpp

#pragma once

#include "common.hpp"
#include "kernel/console.hpp"

#include <limits>
#include <type_traits>
#include <utility>

namespace lib
{
template <typename T>
class Vector
{
  public:
    Vector() = default;

    ~Vector()
    {
        clear();
        deallocate_raw(data_);
    }

    Vector(const Vector& other)
    {
        copy_from(other);
    }

    auto operator=(const Vector& other) -> Vector&
    {
        if (this == &other)
        {
            return *this;
        }

        clear();
        copy_from(other);
        return *this;
    }

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

        clear();
        deallocate_raw(data_);
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
            kernel::panic("[Exception] Overflow on Vector at()");
        }
        return data_[idx];
    }

    auto at(usize idx) const -> const T&
    {
        if (idx >= size_)
        {
            kernel::panic("[Exception] Overflow on Vector at()");
        }
        return data_[idx];
    }

    auto front() -> T&
    {
        require_not_empty("front");
        return data_[0];
    }
    auto front() const -> const T&
    {
        require_not_empty("front");
        return data_[0];
    }
    auto back() -> T&
    {
        require_not_empty("back");
        return data_[size_ - 1];
    }
    auto back() const -> const T&
    {
        require_not_empty("back");
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

    auto push_back(const T& x) -> void
    {
        emplace_back(x);
    }

    auto push_back(T&& x) -> void
    {
        emplace_back(std::move(x));
    }

    template <typename... Args>
    auto emplace_back(Args&&... args) -> T&
    {
        ensure_capacity_for_one_more();
        new (data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    auto pop_back() -> void
    {
        require_not_empty("pop_back");
        --size_;
        data_[size_].~T();
    }

    auto resize(usize new_size) -> void
        requires(std::is_default_constructible_v<T>)
    {
        if (new_size < size_)
        {
            destroy_range(new_size, size_);
            size_ = new_size;
            return;
        }

        if (new_size == size_)
        {
            return;
        }

        reserve(new_size);
        while (size_ < new_size)
        {
            new (data_ + size_) T();
            ++size_;
        }
    }

    auto resize(usize new_size, const T& value) -> void
    {
        if (new_size < size_)
        {
            destroy_range(new_size, size_);
            size_ = new_size;
            return;
        }

        if (new_size == size_)
        {
            return;
        }

        reserve(new_size);
        while (size_ < new_size)
        {
            new (data_ + size_) T(value);
            ++size_;
        }
    }

    auto assign(usize count, const T& value) -> void
    {
        clear();
        reserve(count);
        while (size_ < count)
        {
            new (data_ + size_) T(value);
            ++size_;
        }
    }

    auto reserve(usize new_capacity) -> void
    {
        if (new_capacity <= capacity_)
        {
            return;
        }

        auto* new_data = allocate_raw(new_capacity);
        relocate_into(new_data);
        destroy_range(0, size_);
        deallocate_raw(data_);
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
            deallocate_raw(data_);
            data_ = nullptr;
            capacity_ = 0;
            return;
        }

        auto* new_data = allocate_raw(size_);
        relocate_into(new_data);
        destroy_range(0, size_);
        deallocate_raw(data_);
        data_ = new_data;
        capacity_ = size_;
    }

    auto clear() noexcept -> void
    {
        destroy_range(0, size_);
        size_ = 0;
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

    static auto allocate_raw(usize count) -> T*
    {
        return static_cast<T*>(::operator new(bytes_for(count)));
    }

    static auto deallocate_raw(T* ptr) noexcept -> void
    {
        ::operator delete(ptr);
    }

    auto ensure_capacity_for_one_more() -> void
    {
        if (size_ == capacity_)
        {
            reserve(next_capacity());
        }
    }

    auto copy_from(const Vector& other) -> void
    {
        if (other.size_ == 0)
        {
            return;
        }

        if (capacity_ < other.size_)
        {
            deallocate_raw(data_);
            data_ = allocate_raw(other.size_);
            capacity_ = other.size_;
        }

        for (auto i = 0zu; i < other.size_; ++i)
        {
            new (data_ + i) T(other.data_[i]);
            ++size_;
        }
    }

    static auto relocate_construct_at(T* dst, T& src) -> void
    {
        if constexpr (std::is_move_constructible_v<T>)
        {
            new (dst) T(std::move(src));
        }
        else
        {
            new (dst) T(src);
        }
    }

    auto relocate_into(T* new_data) -> void
    {
        for (auto i = 0zu; i < size_; ++i)
        {
            relocate_construct_at(new_data + i, data_[i]);
        }
    }

    auto destroy_range(usize begin_idx, usize end_idx) noexcept -> void
    {
        if constexpr (std::is_trivially_destructible_v<T>)
        {
            return;
        }

        while (end_idx > begin_idx)
        {
            --end_idx;
            data_[end_idx].~T();
        }
    }

    auto require_not_empty(const char* fn_name) const -> void
    {
        if (empty())
        {
            kernel::panicf("'%s()' on empty vector", fn_name);
        }
    }

    T* data_{};
    usize size_{};
    usize capacity_{};
};
}  // namespace lib
