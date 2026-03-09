// common.hpp

#pragma once

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

constexpr auto operator""_KiB(unsigned long long value) -> usize
{
    return static_cast<usize>(value) * 1024;
}

constexpr auto operator""_MiB(unsigned long long value) -> usize
{
    return static_cast<usize>(value) * 1024_KiB;
}

constexpr auto operator""_GiB(unsigned long long value) -> usize
{
    return static_cast<usize>(value) * 1024_MiB;
}
