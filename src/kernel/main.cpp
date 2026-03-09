#include "kernel/console.hpp"
#include "kernel/physical_page_allocator.hpp"
#include "lib/vector.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void
{
    kernel::physical_memory::init();
    kernel::init_heap();

    {
        lib::Vector<int> vec{};
        vec.reserve(100000);
    }

    kernel::panic("Finished Running");
}
