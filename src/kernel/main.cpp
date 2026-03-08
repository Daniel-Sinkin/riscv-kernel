#include "common.hpp"
#include "kernel/console.hpp"
#include "kernel/panic.hpp"

extern "C" {
extern std::byte _kernel_end[];
}

namespace kernel {
constexpr usize k_page_size{4096};
constexpr usize k_num_pages{32768};
constexpr usize k_bitmap_size{k_num_pages / 8};

struct Page {
    std::byte data[k_page_size];
};
static_assert(k_num_pages * sizeof(Page) == 128 * 1024 * 1024);

} // namespace kernel

extern "C" [[noreturn]] auto kernel_main() -> void {
    using namespace kernel;
    auto bitmap_start = reinterpret_cast<u8 *>(_kernel_end);
    auto bitmap_end = bitmap_start + k_bitmap_size;

    putc('\n');
    for (auto i = 0zu; i < 10; ++i) {
        write_bits(static_cast<u8>(bitmap_start[i]));
        putc('\n');
    }
    putc('\n');

    for (auto i = 0zu; i < 10; ++i) {
        auto found = false;
        for (auto byte_offset = 0zu; byte_offset < k_bitmap_size; ++byte_offset) {
            const auto byte_ = static_cast<u8>(bitmap_start[byte_offset]);
            for (auto bit_offset = 0zu; bit_offset < 8; ++bit_offset) {
                const auto bit_mask = static_cast<u8>(1 << bit_offset);
                if ((byte_ & bit_mask) == 0) {
                    bitmap_start[byte_offset] |= bit_mask;
                    found = true;
                    const auto page_index = 8 * byte_offset + bit_offset;
                    const auto address = bitmap_end + k_page_size * page_index;
                    auto page = reinterpret_cast<Page *>(address);
                    page->data[0] = std::byte{255};
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
    putc('\n');
    for (auto i = 0zu; i < 10; ++i) {
        write_bits(static_cast<u8>(bitmap_start[i]));
        putc('\n');
    }
    putc('\n');

    putc('\n');
    asm volatile("ebreak");
    panic("Finished Running");
}
