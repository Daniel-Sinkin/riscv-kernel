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
enum class UartLineStatus : u8 {
    data_ready = 1u << 0,
    overrun_error = 1u << 1,
    parity_error = 1u << 2,
    framing_error = 1u << 3,
    break_interrupt = 1u << 4,
    transmitter_holding_register_empty = 1u << 5,
    transmitter_empty = 1u << 6,
    fifo_error = 1u << 7,
};

constexpr u32 k_syscon_poweroff{0x5555};
constexpr u32 k_syscon_reboot{0x7777};

template <typename T>
inline auto mmio_write(uptr addr, T value) -> void {
    *reinterpret_cast<volatile T *>(addr) = value;
}

inline auto mmio_read8(uptr addr) -> u8 {
    return *reinterpret_cast<volatile u8 *>(addr);
}

inline auto syscon_poweroff() -> void {
    mmio_write(k_syscon_base, k_syscon_poweroff);
}
inline auto syscon_reboot() -> void {
    mmio_write(k_syscon_base, k_syscon_reboot);
}
inline auto read_uart(UartRegister uart_reg) -> u8 {
    return mmio_read8(k_uart_base + std::to_underlying(uart_reg));
}
inline auto write_uart(UartRegister uart_reg, u8 value) -> void {
    mmio_write(k_uart_base + std::to_underlying(uart_reg), value);
}
inline auto write_uart_transmit(u8 value) -> void {
    const auto ready_flag = std::to_underlying(UartLineStatus::transmitter_holding_register_empty);
    const auto lsr = UartRegister::line_status_register;
    while ((read_uart(lsr) & ready_flag) == 0) {
    }
    write_uart(UartRegister::transmitter_holding_register, value);
}
inline auto putc(char c) {
    if (c == '\n') {
        write_uart_transmit(static_cast<u8>('\r'));
    }
    write_uart_transmit(static_cast<u8>(c));
}
inline auto puts(const char *c) -> void {
    while (*c != '\0') {
        putc(*c);
        ++c;
    }
    putc('\n');
}
inline auto write(const char *c, usize n) -> void {
    while (n-- > 0) {
        putc(*c);
        ++c;
    }
}
[[noreturn]] inline auto panic(const char *msg) -> void {
    puts(msg);
    while (true) {
        syscon_poweroff();
        asm volatile("wfi");
    }
}

extern "C" [[noreturn]] void kernel_main() {
    puts("Hello!");
    panic("Finished Running");

    while (true) {
        asm volatile("wfi");
    }
}
