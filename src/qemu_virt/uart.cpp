#include "qemu_virt/uart.hpp"

#include "qemu_virt/mmio.hpp"

#include <utility>

namespace qemu_virt
{

auto read_uart(UartRegister uart_reg) -> u8
{
    return mmio_read8(k_uart_base + std::to_underlying(uart_reg));
}

auto write_uart(UartRegister uart_reg, u8 value) -> void
{
    mmio_write(k_uart_base + std::to_underlying(uart_reg), value);
}

auto write_uart_transmit(u8 value) -> void
{
    const auto ready_flag = std::to_underlying(UartLineStatus::transmitter_holding_register_empty);
    while ((read_uart(UartRegister::line_status_register) & ready_flag) == 0)
    {
    }
    write_uart(UartRegister::transmitter_holding_register, value);
}

}  // namespace qemu_virt
