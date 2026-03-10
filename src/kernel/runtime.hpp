// kernel/runtime.hpp

#pragma once

#include <cstddef>

auto operator new(std::size_t size) -> void*;
auto operator new[](std::size_t size) -> void*;
auto operator delete(void* ptr) noexcept -> void;
auto operator delete[](void* ptr) noexcept -> void;
auto operator new(std::size_t, void* ptr) noexcept -> void*;
