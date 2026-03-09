#include "kernel/panic.hpp"
#include "kernel/physical_page_allocator.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void {
    kernel::physical_memory::init();

    auto *page = kernel::physical_memory::alloc_page();
    page[0] = std::byte{255};

    kernel::panic("Finished Running");
}
