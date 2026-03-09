#pragma once

#include "common.hpp"

namespace kernel::physical_memory {

constexpr usize k_page_size{4_KiB};
constexpr usize k_default_num_pages{32768};

auto init(usize num_pages = k_default_num_pages) -> void;
auto alloc_page() -> std::byte *;
auto free_page(std::byte *page) -> void;
auto free_page(uptr addr) -> void;

} // namespace kernel::physical_memory
