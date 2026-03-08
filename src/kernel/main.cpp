#include "kernel/console.hpp"
#include "kernel/panic.hpp"

extern "C" [[noreturn]] void kernel_main() {
    kernel::puts("Hello!");
    kernel::write_hex(static_cast<u8>(0xAF));
    kernel::putc('\n');

    asm volatile("ebreak");

    kernel::panic("Finished Running");
}
