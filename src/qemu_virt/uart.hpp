#pragma once

#include "common.hpp"

namespace qemu_virt {

constexpr uptr k_uart_base{0x10000000};

enum class UartRegister : u8 {
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

auto read_uart(UartRegister uart_reg) -> u8;
auto write_uart(UartRegister uart_reg, u8 value) -> void;
auto write_uart_transmit(u8 value) -> void;

} // namespace qemu_virt
