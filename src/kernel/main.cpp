extern "C" [[noreturn]] void kernel_main() {
    for (;;) {
        asm volatile("wfi");
    }
}
