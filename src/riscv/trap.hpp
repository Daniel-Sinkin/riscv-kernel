#pragma once

#include "common.hpp"

extern "C" [[noreturn]] void trap_handler(u64 mcause, u64 mepc, u64 mtval);
