// kernel/runtime.cpp

#include "kernel/runtime.hpp"

#include "kernel/console.hpp"
#include "kernel/heap.hpp"

auto operator new(usize size) -> void*
{
    auto* ptr = kernel::malloc(size);
    if (ptr == nullptr)
    {
        PANIC("operator new allocation failed");
    }
    return ptr;
}

auto operator new[](usize size) -> void*
{
    auto* ptr = kernel::malloc(size);
    if (ptr == nullptr)
    {
        PANIC("operator new[] allocation failed");
    }
    return ptr;
}

auto operator delete(void* ptr) noexcept -> void
{
    kernel::free(ptr);
}

auto operator delete[](void* ptr) noexcept -> void
{
    kernel::free(ptr);
}

auto operator new(usize, void* ptr) noexcept -> void*
{
    return ptr;
}
