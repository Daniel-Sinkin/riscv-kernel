#include "kernel/panic.hpp"

#include "kernel/console.hpp"
#include "qemu_virt/syscon.hpp"

namespace kernel {

[[noreturn]] auto panic(const char *msg) -> void {
    putsln(msg);
    while (true) {
        qemu_virt::syscon_poweroff();
        asm volatile("wfi");
    }
}

}
