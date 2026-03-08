#include <cstddef>
#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using uptr = std::uintptr_t;
using usize = std::size_t;

// mmio = memory mapped IO
constexpr uptr k_qemu_test_mmio_base{0x00100000};
constexpr u32 k_mmio_poweroff{0x5555};
constexpr u32 k_mmio_reboot{0x7777};

template <typename T>
inline auto mmio_write(uptr addr, T value) -> void {
    *reinterpret_cast<volatile T *>(addr) = value;
}

extern "C" [[noreturn]] void kernel_main() {
    mmio_write(k_qemu_test_mmio_base, k_mmio_poweroff);

    while (true) {
        asm volatile("wfi");
    }
}
