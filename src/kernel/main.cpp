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

    static char arg0[] = "kernel";
    static char arg1[] = "--demo";
    static char* argv[] = {arg0, arg1, nullptr};
    const auto argc = 2;

    if (auto retcode = main(argc, argv); retcode != 0)
    {
        PANIC("Got errorcode %d in main()", retcode);
    }
    PANIC("Finished running normally");

    PANIC("Trying to exit kernel_main() illegally!");
}
