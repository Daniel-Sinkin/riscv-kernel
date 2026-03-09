#include "kernel/physical_page_allocator.hpp"

#include "kernel/panic.hpp"

extern "C" {
extern std::byte _kernel_end[];
}

namespace {

class PhysicalPageAllocator {
public:
    auto init(usize num_pages) -> void {
        bitmap_start_ = reinterpret_cast<u8 *>(_kernel_end);
        bitmap_size_ = (num_pages + 7) / 8;
        for (auto i = 0zu; i < bitmap_size_; ++i) {
            bitmap_start_[i] = 0;
        }
        pages_start_ =
            align_up(reinterpret_cast<uptr>(bitmap_start_ + bitmap_size_), kernel::physical_memory::k_page_size);
        num_pages_ = num_pages;
        initialized_ = true;
    }

    auto alloc_page() -> std::byte * {
        require_initialized("alloc_page on not init");

        for (auto byte_offset = 0zu; byte_offset < bitmap_size_; ++byte_offset) {
            const auto byte_ = static_cast<u8>(bitmap_start_[byte_offset]);
            for (auto bit_offset = 0zu; bit_offset < 8; ++bit_offset) {
                const auto page_index = 8 * byte_offset + bit_offset;
                if (page_index >= num_pages_) {
                    break;
                }

                const auto bit_mask = static_cast<u8>(1u << bit_offset);
                if ((byte_ & bit_mask) == 0) {
                    bitmap_start_[byte_offset] |= bit_mask;
                    return reinterpret_cast<std::byte *>(
                        pages_start_ + kernel::physical_memory::k_page_size * page_index);
                }
            }
        }

        kernel::panic("No free page left");
    }

    auto free_page(std::byte *page) -> void {
        free_page(reinterpret_cast<uptr>(page));
    }

    auto free_page(uptr addr) -> void {
        require_initialized("free_page on not init");

        const auto pages_end = pages_start_ + num_pages_ * kernel::physical_memory::k_page_size;
        if (addr < pages_start_ || addr >= pages_end) {
            kernel::panic("free_page out of range");
        }

        if ((addr - pages_start_) % kernel::physical_memory::k_page_size != 0) {
            kernel::panic("free_page misaligned");
        }

        const auto page_index = (addr - pages_start_) / kernel::physical_memory::k_page_size;
        const auto byte_offset = page_index / 8;
        const auto bit_offset = page_index % 8;
        const auto bit_mask = static_cast<u8>(1u << bit_offset);

        if ((bitmap_start_[byte_offset] & bit_mask) == 0) {
            kernel::panic("free_page on free page");
        }

        bitmap_start_[byte_offset] =
            static_cast<u8>(bitmap_start_[byte_offset] & static_cast<u8>(~bit_mask));

        auto *page_bytes = reinterpret_cast<std::byte *>(addr);
        for (auto i = 0zu; i < kernel::physical_memory::k_page_size; ++i) {
            page_bytes[i] = std::byte{0};
        }
    }

private:
    static constexpr auto align_up(uptr value, usize alignment) -> uptr {
        const auto mask = static_cast<uptr>(alignment - 1);
        return (value + mask) & ~mask;
    }

    auto require_initialized(const char *message) const -> void {
        if (!initialized_) {
            kernel::panic(message);
        }
    }

    u8 *bitmap_start_{};
    usize bitmap_size_{};
    uptr pages_start_{};
    usize num_pages_{};
    bool initialized_{false};
};

PhysicalPageAllocator g_allocator{};

} // namespace

static_assert(kernel::physical_memory::k_default_num_pages * kernel::physical_memory::k_page_size == 128_MiB);

namespace kernel::physical_memory {

auto init(usize num_pages) -> void {
    g_allocator.init(num_pages);
}

auto alloc_page() -> std::byte * {
    return g_allocator.alloc_page();
}

auto free_page(std::byte *page) -> void {
    g_allocator.free_page(page);
}

auto free_page(uptr addr) -> void {
    g_allocator.free_page(addr);
}

} // namespace kernel::physical_memory
