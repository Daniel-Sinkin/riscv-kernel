// kernel/cstring.cpp
#include "kernel/cstring.hpp"

#include "common.hpp"
#include "kernel/console.hpp"

auto memset(void* buf, u8 c, usize n) -> void*
{
    auto buf8 = reinterpret_cast<u8*>(buf);
    while (n-- > 0)
    {
        *buf8 = c;
    }
    return buf;
}

auto memcpy(void* dst, const void* src, usize n) -> void*;
auto strcpy(char* dst, const char* src) -> char*;
auto strcmp(const char* s1, const char* s2) -> int;
