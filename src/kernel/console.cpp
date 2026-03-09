// kernel/console.cpp

#include "kernel/console.hpp"

#include "qemu_virt/syscon.hpp"
#include "qemu_virt/uart.hpp"

namespace kernel
{

auto putc(char c) -> void
{
    if (c == '\n')
    {
        qemu_virt::write_uart_transmit(static_cast<u8>('\r'));
    }
    qemu_virt::write_uart_transmit(static_cast<u8>(c));
}

auto putc(u8 value) -> void
{
    putc(static_cast<char>(value));
}

auto putc(std::byte value) -> void
{
    putc(std::to_integer<u8>(value));
}

auto puts(const char* c) -> void
{
    while (*c != '\0')
    {
        putc(*c);
        ++c;
    }
}

auto putsln(const char* c) -> void
{
    puts(c);
    putc('\n');
}

auto write(const char* c, usize n) -> void
{
    while (n-- > 0)
    {
        putc(*c);
        ++c;
    }
}

[[noreturn]] auto panic(const char* msg) -> void
{
    putsln(msg);
    while (true)
    {
        qemu_virt::syscon_poweroff();
        asm volatile("wfi");
    }
}

}  // namespace kernel
