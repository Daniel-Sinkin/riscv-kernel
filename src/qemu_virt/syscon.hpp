#pragma once

#include "common.hpp"

namespace qemu_virt {

constexpr uptr k_syscon_base{0x00100000};
constexpr u32 k_syscon_poweroff{0x5555};
constexpr u32 k_syscon_reboot{0x7777};

auto syscon_poweroff() -> void;
auto syscon_reboot() -> void;

} // namespace qemu_virt
