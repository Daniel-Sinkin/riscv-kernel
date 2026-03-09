#include "kernel/heap.hpp"

#include "kernel/panic.hpp"
#include "kernel/physical_page_allocator.hpp"

namespace kernel {
namespace {

enum BlockFlags : u32 {
    free = (1u << 0),
};

struct PageHeader {
    u32 free_bytes{};
};

struct alignas(8) BlockHeader {
    u32 flags{};
    u32 size{};

    [[nodiscard]] auto is_free() const -> bool {
        return (flags & BlockFlags::free) != 0;
    }

    auto mark_free() -> void {
        flags |= BlockFlags::free;
    }

    auto mark_used() -> void {
        flags &= ~BlockFlags::free;
    }
};

physical_memory::Page *g_heap_page{};
bool g_initialized{};

[[maybe_unused]] [[nodiscard]] constexpr auto align_up(usize value, usize alignment) -> usize {
    const auto mask = alignment - 1;
    return (value + mask) & ~mask;
}

[[nodiscard]] constexpr auto align_up(u32 value, u32 alignment) -> u32 {
    const auto mask = static_cast<u32>(alignment - 1);
    return static_cast<u32>((value + mask) & ~mask);
}

[[nodiscard]] constexpr auto first_block_offset() -> u32 {
    return align_up(static_cast<u32>(sizeof(PageHeader)), static_cast<u32>(alignof(BlockHeader)));
}

[[nodiscard]] constexpr auto usable_block_bytes() -> u32 {
    return static_cast<u32>(physical_memory::k_page_size) - first_block_offset() - static_cast<u32>(sizeof(BlockHeader));
}

template <typename T>
[[nodiscard]] auto aligned_ptr(std::byte *ptr) -> T * {
    return static_cast<T *>(__builtin_assume_aligned(ptr, alignof(T)));
}

[[nodiscard]] auto page_header() -> PageHeader * {
    return aligned_ptr<PageHeader>(g_heap_page->data);
}

[[nodiscard]] auto page_bytes() -> std::byte * {
    return g_heap_page->data;
}

[[nodiscard]] auto first_block() -> BlockHeader * {
    return aligned_ptr<BlockHeader>(page_bytes() + first_block_offset());
}

[[nodiscard]] auto block_payload(BlockHeader *block) -> std::byte * {
    return reinterpret_cast<std::byte *>(block) + sizeof(BlockHeader);
}

[[nodiscard]] auto block_end(BlockHeader *block) -> std::byte * {
    return block_payload(block) + block->size;
}

[[nodiscard]] auto page_end() -> std::byte * {
    return page_bytes() + physical_memory::k_page_size;
}

[[nodiscard]] auto next_block(BlockHeader *block) -> BlockHeader * {
    auto *next = block_end(block);
    if (next >= page_end()) {
        return nullptr;
    }
    return aligned_ptr<BlockHeader>(next);
}

auto require_initialized() -> void {
    if (!g_initialized) {
        panic("heap used before init");
    }
}

}

auto init_heap() -> void {
    g_heap_page = physical_memory::alloc_page();

    for (auto &byte : g_heap_page->data) {
        byte = std::byte{0};
    }

    auto *header = page_header();
    header->free_bytes = static_cast<u32>(usable_block_bytes());

    auto *block = first_block();
    block->mark_free();
    block->size = static_cast<u32>(usable_block_bytes());

    g_initialized = true;
}

auto malloc(usize n) -> void * {
    require_initialized();

    if (n == 0 || n > usable_block_bytes()) {
        return nullptr;
    }

    const auto bh_align = static_cast<u32>(alignof(BlockHeader));
    const auto bh_size = static_cast<u32>(sizeof(BlockHeader));
    const auto aligned_n = align_up(static_cast<u32>(n), bh_align);
    if (aligned_n > page_header()->free_bytes) {
        return nullptr;
    }

    for (auto *block = first_block(); block; block = next_block(block)) {
        if (!block->is_free() || block->size < aligned_n) {
            continue;
        }
        const auto original_size = block->size;
        const auto remaining_size = static_cast<u32>(block->size - aligned_n);
        if (remaining_size > bh_size) {
            auto *split_block = aligned_ptr<BlockHeader>(block_payload(block) + aligned_n);
            split_block->flags = 0;
            split_block->mark_free();
            split_block->size = remaining_size - bh_size;
            block->size = aligned_n;
            page_header()->free_bytes -= aligned_n + bh_size;
        } else {
            page_header()->free_bytes -= original_size;
        }
        block->mark_used();
        return static_cast<void *>(block_payload(block));
    }

    return nullptr;
}

auto realloc(void *ptr) -> void * {
    require_initialized();

    if (ptr == nullptr) {
        panic("Called realloc with nullptr");
        return nullptr;
    }
    return nullptr;
}

auto free(void *ptr) -> void {
    require_initialized();

    if (ptr == nullptr) {
        return;
    }

    auto *addr = static_cast<std::byte *>(ptr);
    if (addr < page_bytes() + first_block_offset() + sizeof(BlockHeader) || addr >= page_end()) {
        panic("free out of heap page range");
    }

    auto *block = aligned_ptr<BlockHeader>(addr - sizeof(BlockHeader));
    if (block->is_free()) {
        panic("double free");
    }

    block->mark_free();
    page_header()->free_bytes += block->size;

    for (auto *byte = block_payload(block); byte < block_end(block); ++byte) {
        *byte = std::byte{0};
    }
}

}
