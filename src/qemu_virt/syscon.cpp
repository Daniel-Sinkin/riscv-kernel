#include "qemu_virt/syscon.hpp"

#include "qemu_virt/mmio.hpp"

namespace qemu_virt
{

auto syscon_poweroff() -> void
{
    mmio_write(k_syscon_base, k_syscon_poweroff);
}

auto syscon_reboot() -> void
{
    mmio_write(k_syscon_base, k_syscon_reboot);
}

}  // namespace qemu_virt
