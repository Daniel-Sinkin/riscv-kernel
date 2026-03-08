#pragma once

#include <concepts>

#include "common.hpp"

namespace kernel {

auto putc(char c) -> void;
auto puts(const char *c) -> void;
auto putsln(const char *c) -> void;
auto write(const char *c, usize n) -> void;

template <std::unsigned_integral U>
inline auto write_hex(U x) -> void {
    putc('0');
    putc('x');
    for (auto i = static_cast<int>(sizeof(U) * 2 - 1); i >= 0; --i) {
        const auto nibble = static_cast<u8>((x >> (4 * i)) & 0x0F);
        if (nibble < 10) {
            putc(static_cast<char>('0' + nibble));
        } else {
            putc(static_cast<char>('A' + (nibble - 10)));
        }
    }
}

}  // namespace kernel
