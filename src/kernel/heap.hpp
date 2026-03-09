// kernel/heap.hpp

#pragma once

#include "common.hpp"

namespace kernel
{

auto init_heap() -> void;
auto malloc(usize n) -> void*;
auto realloc(void* ptr, usize n) -> void*;
auto free(void* ptr) -> void;

}  // namespace kernel
