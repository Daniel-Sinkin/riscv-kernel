// kernel/main.cpp

#include "kernel/console.hpp"
#include "kernel/heap.hpp"
#include "kernel/physical_page_allocator.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void
{
    using namespace kernel;
    physical_memory::init();
    init_heap();

    panic("Finished Running");
}
