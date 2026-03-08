#include <cstddef>
#include <cstdint>
#include <utility>

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

constexpr uptr k_syscon_base{0x00100000};
// UART (Universal Asynchronous Receiver/Transmitter) device
constexpr uptr k_uart_base{0x10000000};
enum class UartRegister : usize {
    receiver_buffer_register = 0x00,
    transmitter_holding_register = 0x00,
    divisor_latch_low = 0x00,

    interrupt_enable_register = 0x01,
    divisor_latch_high = 0x01,

    interrupt_identification_register = 0x02,
    fifo_control_register = 0x02,

    line_control_register = 0x03,
    modem_control_register = 0x04,
    line_status_register = 0x05,
    modem_status_register = 0x06,
    scratch_register = 0x07,
};
;

constexpr u32 k_syscon_poweroff{0x5555};
constexpr u32 k_syscon_reboot{0x7777};

template <typename T>
inline auto mmio_write(uptr addr, T value) -> void {
    *reinterpret_cast<volatile T *>(addr) = value;
}

inline auto syscon_poweroff() -> void {
    mmio_write(k_syscon_base, k_syscon_poweroff);
}
inline auto syscon_reboot() -> void {
    mmio_write(k_syscon_base, k_syscon_reboot);
}
inline auto write_uart(UartRegister uart_reg, char value) {
    mmio_write(k_uart_base + std::to_underlying(uart_reg), value);
}
inline auto putc(char value) {
    write_uart(UartRegister::transmitter_holding_register, value);
}

extern "C" [[noreturn]] void kernel_main() {
    putc('A');
    syscon_poweroff();

    while (true) {
        asm volatile("wfi");
    }
}
