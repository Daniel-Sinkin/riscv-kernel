// kernel/cstring.cpp
#include "kernel/cstring.hpp"

#include "kernel/console.hpp"
#include "lib/algorithm.hpp"

namespace kernel
{
auto memset(void* buf, u8 c, usize n) -> void*
{
    auto* buf8 = reinterpret_cast<u8*>(buf);
    while (n-- > 0)
    {
        *buf8 = c;
        ++buf8;
    }
    return buf;
}

auto memset(lib::Span<u8> buf, u8 c) -> void
{
    for (auto& byte : buf)
    {
        byte = c;
    }
}

auto memcpy(void* dst, const void* src, usize n) -> void*
{
    auto dst8 = reinterpret_cast<u8*>(dst);
    auto src8 = reinterpret_cast<const u8*>(src);
    while (n-- > 0)
    {
        *dst8 = *src8;
        ++dst8;
        ++src8;
    }
    return dst;
}

auto memcpy(lib::Span<u8> dst, lib::Span<const u8> src) -> void
{
    if (dst.size() < src.size())
    {
        kernel::panic("memcpy span destination is too small");
    }
    for (auto i = 0zu; i < src.size(); ++i)
    {
        dst[i] = src[i];
    }
}

auto strcpy(char* dst, const char* src) -> char*
{
    auto* out = dst;
    while (true)
    {
        *out = *src;
        if (*src == '\0')
        {
            return dst;
        }
        ++out;
        ++src;
    }
}

auto strcpy(lib::Span<char> dst, lib::Span<const char> src) -> void
{
    if (dst.size() < src.size())
    {
        kernel::panic("strcpy span destination is too small");
    }

    for (auto i = 0zu; i < src.size(); ++i)
    {
        dst[i] = src[i];
        if (src[i] == '\0')
        {
            return;
        }
    }
}

auto strcmp(const char* s1, const char* s2) -> int
{
    while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2)
    {
        ++s1;
        ++s2;
    }

    const auto lhs = static_cast<u8>(*s1);
    const auto rhs = static_cast<u8>(*s2);
    if (lhs == rhs)
    {
        return 0;
    }
    return (lhs < rhs) ? -1 : 1;
}

auto strcmp(lib::Span<const char> s1, lib::Span<const char> s2) -> int
{
    auto i = 0zu;
    while (i < lib::max(s1.size(), s2.size()) && s1[i] == s2[i])
    {
        ++i;
    }

    if (i < lib::max(s1.size(), s2.size()))
    {
        const auto lhs = static_cast<u8>(s1[i]);
        const auto rhs = static_cast<u8>(s2[i]);
        if (lhs == rhs)
        {
            return 0;
        }
        return (lhs < rhs) ? -1 : 1;
    }

    if (s1.size() == s2.size())
    {
        return 0;
    }

    return s1.size() < s2.size() ? -1 : 1;
}
}  // namespace kernel
