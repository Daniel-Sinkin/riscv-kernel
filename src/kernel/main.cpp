// kernel/main.cpp

#include "app/main.cpp"

#include "kernel/console.hpp"
#include "kernel/heap.hpp"
#include "kernel/physical_page_allocator.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void
{
    using namespace kernel;
    physical_memory::init();
    init_heap();

    if (auto retcode = main(); retcode != 0)
    {
        panicf("Got errorcode %d in main()", retcode);
    }
    panic("Finished running normally");

    panic("Trying to exit kernel_main() illegally!");
}
