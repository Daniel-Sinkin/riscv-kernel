#pragma once

#include "common.hpp"

namespace kernel::physical_memory {

constexpr usize k_page_size{4_KiB};
constexpr usize k_default_num_pages{32768};
static_assert(k_page_size * k_default_num_pages == 128_MiB);

struct alignas(8) Page {
    std::byte data[k_page_size];
};

auto init(usize num_pages = k_default_num_pages) -> void;
auto alloc_page() -> Page *;
auto free_page(uptr addr) -> void;
auto free_page(Page *page) -> void;

}
