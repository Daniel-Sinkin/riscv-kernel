#include "kernel/console.hpp"
#include "kernel/heap.hpp"
#include "kernel/physical_page_allocator.hpp"
#include "lib/array.hpp"

extern "C" [[noreturn]] auto kernel_main() -> void
{
    kernel::physical_memory::init();
    kernel::init_heap();

    ds_lib::Array<int, 3> values{};
    values[0] = 137;
    values[1] = 42;
    values[2] = 7;

    kernel::write_number(values.front());
    kernel::putc('\n');
    kernel::write_number(values.back());
    kernel::putc('\n');

    const auto& const_values = values;
    for (auto it = const_values.cbegin(); it != const_values.cend(); ++it)
    {
        kernel::write_number(*it);
        kernel::putc('\n');
    }

    auto* ints = static_cast<int*>(kernel::malloc(2 * sizeof(int)));
    if (ints == nullptr)
    {
        kernel::panic("malloc failed");
    }

    ints[0] = 123;
    ints[1] = 456;

    ints = static_cast<int*>(kernel::realloc(ints, 4 * sizeof(int)));
    if (ints == nullptr)
    {
        kernel::panic("realloc grow failed");
    }

    kernel::write_number(ints[0]);
    kernel::putc('\n');
    kernel::write_number(ints[1]);
    kernel::putc('\n');

    ints = static_cast<int*>(kernel::realloc(ints, sizeof(int)));
    if (ints == nullptr)
    {
        kernel::panic("realloc shrink failed");
    }

    kernel::write_number(ints[0]);
    kernel::putc('\n');
    kernel::free(ints);

    kernel::panic("Finished Running");
}
