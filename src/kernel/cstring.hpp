// kernel/cstring.hpp

#pragma once

#include "common.hpp"
#include "lib/span.hpp"

namespace kernel
{

auto memset(void* buf, u8 c, usize n) -> void*;
auto memset(lib::Span<u8> buf, u8 c) -> void;

auto memcpy(void* dst, const void* src, usize n) -> void*;
auto memcpy(lib::Span<u8> dst, lib::Span<const u8> src) -> void;

auto strcpy(char* dst, const char* src) -> char*;
auto strcpy(lib::Span<char> dst, lib::Span<const char> src) -> void;

auto strcmp(const char* s1, const char* s2) -> int;
auto strcmp(lib::Span<const char> s1, lib::Span<const char> s2) -> int;

}  // namespace kernel

extern "C"
{
auto memset(void* buf, int c, usize n) -> void*;
auto memcpy(void* dst, const void* src, usize n) -> void*;
auto strcpy(char* dst, const char* src) -> char*;
auto strcmp(const char* s1, const char* s2) -> int;
}
