
// kernel/heap.cpp

#include "kernel/heap.hpp"

#include "kernel/console.hpp"
#include "kernel/physical_page_allocator.hpp"

namespace kernel
{
namespace
{

enum BlockFlags : u32
{
    free = (1u << 0),
};

constexpr u32 k_block_magic_mask{0xFFFF0000u};
constexpr u32 k_block_magic_value{0xA1100000u};

struct PageHeader
{
    u32 free_bytes{};
};

struct alignas(8) BlockHeader
{
    u32 flags{};
    u32 size{};
    u32 prev_size{};

    [[nodiscard]] auto is_free() const -> bool
    {
        return (flags & BlockFlags::free) != 0;
    }

    [[nodiscard]] auto has_valid_magic() const -> bool
    {
        return (flags & k_block_magic_mask) == k_block_magic_value;
    }

    auto mark_free() -> void
    {
        flags |= BlockFlags::free;
    }

    auto mark_used() -> void
    {
        flags &= ~BlockFlags::free;
    }
};

physical_memory::Page* g_heap_page{};
bool g_initialized{};

[[maybe_unused]] [[nodiscard]] constexpr auto align_up(usize value, usize alignment) -> usize
{
    const auto mask = alignment - 1;
    return (value + mask) & ~mask;
}

[[nodiscard]] constexpr auto align_up(u32 value, u32 alignment) -> u32
{
    const auto mask = static_cast<u32>(alignment - 1);
    return static_cast<u32>((value + mask) & ~mask);
}

[[nodiscard]] constexpr auto first_block_offset() -> u32
{
    return align_up(static_cast<u32>(sizeof(PageHeader)), static_cast<u32>(alignof(BlockHeader)));
}

[[nodiscard]] constexpr auto usable_block_bytes() -> u32
{
    return static_cast<u32>(physical_memory::k_page_size) - first_block_offset()
           - static_cast<u32>(sizeof(BlockHeader));
}

template <typename T>
[[nodiscard]] auto aligned_ptr(std::byte* ptr) -> T*
{
    return static_cast<T*>(__builtin_assume_aligned(ptr, alignof(T)));
}

[[nodiscard]] auto page_header() -> PageHeader*
{
    return aligned_ptr<PageHeader>(g_heap_page->data);
}

[[nodiscard]] auto page_bytes() -> std::byte*
{
    return g_heap_page->data;
}

[[nodiscard]] auto first_block() -> BlockHeader*
{
    return aligned_ptr<BlockHeader>(page_bytes() + first_block_offset());
}

[[nodiscard]] auto block_payload(BlockHeader* block) -> std::byte*
{
    return reinterpret_cast<std::byte*>(block) + sizeof(BlockHeader);
}

[[nodiscard]] auto block_end(BlockHeader* block) -> std::byte*
{
    return block_payload(block) + block->size;
}

[[nodiscard]] auto page_end() -> std::byte*
{
    return page_bytes() + physical_memory::k_page_size;
}

[[nodiscard]] auto next_block(BlockHeader* block) -> BlockHeader*;

auto initialize_block(BlockHeader* block, u32 size, u32 prev_size, bool is_free) -> void
{
    block->flags = k_block_magic_value;
    block->prev_size = prev_size;
    if (is_free)
    {
        block->mark_free();
    }
    block->size = size;
}

[[nodiscard]] auto is_valid_block(BlockHeader* block) -> bool
{
    if (block == nullptr)
    {
        return false;
    }

    const auto block_addr = reinterpret_cast<uptr>(block);
    const auto page_begin = reinterpret_cast<uptr>(page_bytes());
    const auto page_limit = reinterpret_cast<uptr>(page_end());
    if (block_addr < page_begin || block_addr >= page_limit)
    {
        return false;
    }

    if ((block_addr % alignof(BlockHeader)) != 0)
    {
        return false;
    }

    if (!block->has_valid_magic())
    {
        return false;
    }

    const auto payload_begin = reinterpret_cast<uptr>(block_payload(block));
    const auto payload_end = reinterpret_cast<uptr>(block_end(block));
    if (payload_begin > page_limit || payload_end > page_limit || payload_end < payload_begin)
    {
        return false;
    }

    if (block != first_block())
    {
        const auto prev_addr = block_addr - sizeof(BlockHeader) - block->prev_size;
        if (prev_addr < page_begin || prev_addr >= page_limit)
        {
            return false;
        }

        auto* prev = aligned_ptr<BlockHeader>(reinterpret_cast<std::byte*>(prev_addr));
        if (!prev->has_valid_magic() || next_block(prev) != block)
        {
            return false;
        }
    }

    return true;
}

[[nodiscard]] auto next_block(BlockHeader* block) -> BlockHeader*
{
    auto* next = block_end(block);
    if (next >= page_end())
    {
        return nullptr;
    }
    return aligned_ptr<BlockHeader>(next);
}

[[nodiscard]] auto prev_block(BlockHeader* block) -> BlockHeader*
{
    if (block == first_block())
    {
        return nullptr;
    }

    const auto prev_addr = reinterpret_cast<uptr>(block) - sizeof(BlockHeader) - block->prev_size;
    return aligned_ptr<BlockHeader>(reinterpret_cast<std::byte*>(prev_addr));
}

auto update_next_prev_size(BlockHeader* block) -> void
{
    if (auto* next = next_block(block))
    {
        next->prev_size = block->size;
    }
}

[[nodiscard]] auto compute_free_bytes() -> u32
{
    auto free_bytes = 0u;
    for (auto* block = first_block(); block; block = next_block(block))
    {
        if (!is_valid_block(block))
        {
            panic("heap metadata corruption");
        }

        if (block->is_free())
        {
            free_bytes += block->size;
        }
    }
    return free_bytes;
}

auto refresh_free_bytes() -> void
{
    page_header()->free_bytes = compute_free_bytes();
}

auto merge_with_next(BlockHeader* block) -> bool
{
    auto* next = next_block(block);
    if (next == nullptr)
    {
        return false;
    }

    if (!is_valid_block(next))
    {
        panic("heap metadata corruption");
    }

    if (!next->is_free())
    {
        return false;
    }

    block->size += static_cast<u32>(sizeof(BlockHeader)) + next->size;
    update_next_prev_size(block);
    return true;
}

auto require_initialized() -> void
{
    if (!g_initialized)
    {
        panic("heap used before init");
    }
}

[[nodiscard]] auto require_allocated_block_from_payload(std::byte* addr, const char* message)
    -> BlockHeader*
{
    if (addr < page_bytes() + first_block_offset() + sizeof(BlockHeader) || addr >= page_end())
    {
        panic(message);
    }

    const auto block_addr = reinterpret_cast<uptr>(addr - sizeof(BlockHeader));
    if ((block_addr % alignof(BlockHeader)) != 0)
    {
        panic(message);
    }

    auto* block = aligned_ptr<BlockHeader>(reinterpret_cast<std::byte*>(block_addr));
    if (!is_valid_block(block) || block_payload(block) != addr)
    {
        panic(message);
    }

    if (block->is_free())
    {
        panic("double free");
    }

    return block;
}

}  // namespace

auto init_heap() -> void
{
    g_heap_page = physical_memory::alloc_page();

    for (auto& byte : g_heap_page->data)
    {
        byte = std::byte{0};
    }

    auto* header = page_header();
    header->free_bytes = static_cast<u32>(usable_block_bytes());

    auto* block = first_block();
    initialize_block(block, static_cast<u32>(usable_block_bytes()), 0, true);

    g_initialized = true;
}

auto malloc(usize n) -> void*
{
    require_initialized();

    if (n == 0 || n > usable_block_bytes())
    {
        return nullptr;
    }

    const auto bh_align = static_cast<u32>(alignof(BlockHeader));
    const auto bh_size = static_cast<u32>(sizeof(BlockHeader));
    const auto aligned_n = align_up(static_cast<u32>(n), bh_align);
    if (aligned_n > page_header()->free_bytes)
    {
        return nullptr;
    }

    for (auto* block = first_block(); block; block = next_block(block))
    {
        if (!block->is_free() || block->size < aligned_n)
        {
            continue;
        }
        const auto remaining_size = static_cast<u32>(block->size - aligned_n);
        if (remaining_size > bh_size)
        {
            auto* split_block = aligned_ptr<BlockHeader>(block_payload(block) + aligned_n);
            initialize_block(split_block, remaining_size - bh_size, aligned_n, true);
            block->size = aligned_n;
            update_next_prev_size(split_block);
        }
        block->mark_used();
        refresh_free_bytes();
        return static_cast<void*>(block_payload(block));
    }

    return nullptr;
}

auto realloc(void* ptr, usize n) -> void*
{
    require_initialized();
    if (ptr == nullptr)
    {
        return malloc(n);
    }
    if (n == 0)
    {
        kernel::free(ptr);
        return nullptr;
    }

    if (n > usable_block_bytes())
    {
        return nullptr;
    }

    const auto bh_align = static_cast<u32>(alignof(BlockHeader));
    const auto bh_size = static_cast<u32>(sizeof(BlockHeader));
    const auto aligned_n = align_up(static_cast<u32>(n), bh_align);

    auto* addr = static_cast<std::byte*>(ptr);
    auto* block = require_allocated_block_from_payload(addr, "realloc invalid pointer");
    const auto old_size = block->size;

    if (aligned_n == old_size)
    {
        return ptr;
    }

    if (aligned_n < old_size)
    {
        const auto remaining_size = static_cast<u32>(old_size - aligned_n);
        if (remaining_size > bh_size)
        {
            auto* old_end = block_end(block);
            auto* split_start = block_payload(block) + aligned_n;
            for (auto* byte = split_start; byte < old_end; ++byte)
            {
                *byte = std::byte{0};
            }

            auto* split_block = aligned_ptr<BlockHeader>(split_start);
            initialize_block(split_block, remaining_size - bh_size, aligned_n, true);
            block->size = aligned_n;
            update_next_prev_size(split_block);
            refresh_free_bytes();
        }
        return ptr;
    }

    auto merged_size = old_size;
    for (auto* next = next_block(block); next; next = next_block(next))
    {
        if (!is_valid_block(next))
        {
            panic("heap metadata corruption");
        }

        if (!next->is_free())
        {
            break;
        }

        merged_size += bh_size + next->size;
        if (merged_size >= aligned_n)
        {
            break;
        }
    }

    if (merged_size >= aligned_n)
    {
        const auto remaining_size = static_cast<u32>(merged_size - aligned_n);
        if (remaining_size > bh_size)
        {
            auto* split_block = aligned_ptr<BlockHeader>(block_payload(block) + aligned_n);
            initialize_block(split_block, remaining_size - bh_size, aligned_n, true);
            block->size = aligned_n;
            update_next_prev_size(split_block);
        }
        else
        {
            block->size = merged_size;
            update_next_prev_size(block);
        }
        refresh_free_bytes();
        return ptr;
    }

    auto* ptr_new = static_cast<std::byte*>(malloc(n));
    if (!ptr_new)
    {
        return nullptr;
    }

    for (auto i = 0u; i < old_size; ++i)
    {
        ptr_new[i] = addr[i];
    }

    kernel::free(ptr);
    return ptr_new;
}

auto free(void* ptr) -> void
{
    require_initialized();

    if (ptr == nullptr)
    {
        return;
    }

    auto* addr = static_cast<std::byte*>(ptr);
    auto* block = require_allocated_block_from_payload(addr, "free invalid pointer");

    block->mark_free();

    for (auto* byte = block_payload(block); byte < block_end(block); ++byte)
    {
        *byte = std::byte{0};
    }

    while (merge_with_next(block))
    {
    }

    if (auto* prev = prev_block(block); prev && prev->is_free())
    {
        while (merge_with_next(prev))
        {
        }
    }

    refresh_free_bytes();
}

}  // namespace kernel
