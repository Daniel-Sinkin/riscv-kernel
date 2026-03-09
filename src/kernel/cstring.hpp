// kernel/cstring.hpp

#pragma once
#include "common.hpp"

auto memset(void* buf, u8 c, usize n) -> void*;
auto memcpy(void* dst, const void* src, usize n) -> void*;
auto strcpy(char* dst, const char* src) -> char*;
auto strcmp(const char* s1, const char* s2) -> int;
