#include "kernel/heap.hpp"
#include "kernel/panic.hpp"
#include "kernel/physical_page_allocator.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void {
    kernel::physical_memory::init();
    kernel::init_heap();

    auto p = static_cast<int *>(kernel::malloc(32));
    if (p == nullptr) {
        kernel::panic("malloc failed");
    }

    kernel::panic("Finished Running");
}
