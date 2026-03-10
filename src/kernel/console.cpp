// kernel/console.cpp

#include "kernel/console.hpp"

#include "qemu_virt/syscon.hpp"
#include "qemu_virt/uart.hpp"
#include "util.hpp"

#include <cstdarg>

namespace kernel
{

namespace
{

auto write_format_specifier(char specifier, va_list& vargs) -> bool
{
    switch (specifier)
    {
        case '\0':
            putc('%');
            return false;
        case '%':
            putc('%');
            return true;
        case 's':
            {
                auto s = va_arg(vargs, const char*);
                if (s == nullptr)
                {
                    s = "(null)";
                }
                while (*s != '\0')
                {
                    putc(*s);
                    ++s;
                }
                return true;
            }
        case 'd':
            {
                const auto value = va_arg(vargs, int);
                write_number(value);
                return true;
            }
        case 'x':
            {
                const auto value = va_arg(vargs, unsigned int);
                write_hex(value);
                return true;
            }
        default:
            putc('%');
            putc(specifier);
            return true;
    }
}

auto vprintf(const char* fmt, va_list& vargs) -> void
{
    while (*fmt != '\0')
    {
        if (*fmt != '%')
        {
            putc(*fmt);
            ++fmt;
            continue;
        }

        ++fmt;
        if (!write_format_specifier(*fmt, vargs))
        {
            return;
        }

        ++fmt;
    }
}

}  // namespace

auto printf(const char* fmt, ...) -> void
{
    va_list vargs;
    va_start(vargs, fmt);
    ScopeExit se{[&] { va_end(vargs); }};
    vprintf(fmt, vargs);
}
auto printfn(const char* fmt, ...) -> void
{
    va_list vargs;
    va_start(vargs, fmt);
    ScopeExit se{[&] { va_end(vargs); }};
    vprintf(fmt, vargs);
    putc('\n');
};

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

[[noreturn]] auto _panic() -> void
{
    while (true)
    {
        qemu_virt::syscon_poweroff();
        asm volatile("wfi");
    }
}

[[noreturn]] auto panic(const char* msg) -> void
{
    putsln(msg);
    _panic();
}

[[noreturn]] auto panicf(const char* fmt, ...) -> void
{
    va_list vargs;
    va_start(vargs, fmt);
    ScopeExit se{[&] { va_end(vargs); }};
    vprintf(fmt, vargs);
    va_end(vargs);
    _panic();
}

}  // namespace kernel
