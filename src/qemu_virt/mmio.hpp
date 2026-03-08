#pragma once

#include "common.hpp"

namespace qemu_virt {

template <typename T>
inline auto mmio_write(uptr addr, T value) -> void {
    *reinterpret_cast<volatile T *>(addr) = value;
}

template <typename T>
inline auto mmio_read(uptr addr) -> T {
    return *reinterpret_cast<volatile T *>(addr);
}

inline auto mmio_read8(uptr addr) -> u8 {
    return mmio_read<u8>(addr);
}

}  // namespace qemu_virt
