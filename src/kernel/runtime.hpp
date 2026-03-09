// kernel/runtime.hpp

#pragma once

#include "common.hpp"

auto operator new(usize size) -> void*;
auto operator new[](usize size) -> void*;
auto operator delete(void* ptr) noexcept -> void;
auto operator delete[](void* ptr) noexcept -> void;
auto operator new(usize, void* ptr) noexcept -> void*;
