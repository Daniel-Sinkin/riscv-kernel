// kernel/console.hpp

#pragma once

#include "common.hpp"

#include <concepts>
#include <type_traits>

namespace kernel
{

auto putc(char c) -> void;
auto putc(u8 value) -> void;
auto putc(std::byte value) -> void;
auto puts(const char* c) -> void;
auto putsln(const char* c) -> void;
auto write(const char* c, usize n) -> void;
[[noreturn]] auto panic(const char* msg) -> void;

template <std::unsigned_integral U>
inline auto write_hex(U x) -> void
{
    putc('0');
    putc('x');
    for (auto i = static_cast<int>(sizeof(U) * 2 - 1); i >= 0; --i)
    {
        const auto nibble = static_cast<u8>((x >> (4 * i)) & 0x0F);
        if (nibble < 10)
        {
            putc(static_cast<char>('0' + nibble));
        }
        else
        {
            putc(static_cast<char>('A' + (nibble - 10)));
        }
    }
}

template <std::unsigned_integral U>
inline auto write_bits(U x) -> void
{
    putc('0');
    putc('b');
    for (auto i = static_cast<int>(sizeof(U) * 8 - 1); i >= 0; --i)
    {
        const auto bit = static_cast<u8>((x >> i) & 0x01);
        putc(static_cast<char>('0' + bit));
    }
}

template <std::integral I>
inline auto write_number(I x) -> void
{
    using U = std::make_unsigned_t<I>;

    U magnitude{};
    if constexpr (std::signed_integral<I>)
    {
        if (x < 0)
        {
            putc('-');
            // Based on
            // https://github.com/nuta/operating-system-in-1000-lines/issues/64
            magnitude = static_cast<U>(-(x + 1)) + 1;
        }
        else
        {
            magnitude = static_cast<U>(x);
        }
    }
    else
    {
        magnitude = x;
    }

    if (magnitude == 0)
    {
        putc('0');
        return;
    }

    char digits[sizeof(U) * 3];
    auto count = 0zu;
    while (magnitude > 0)
    {
        digits[count] = static_cast<char>('0' + (magnitude % 10));
        magnitude /= 10;
        ++count;
    }

    while (count > 0)
    {
        --count;
        putc(digits[count]);
    }
}

auto printf(const char* fmt, ...) -> void;

}  // namespace kernel
