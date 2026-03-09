#include "kernel/console.hpp"
#include "kernel/physical_page_allocator.hpp"
#include "lib/array.hpp"
#include "lib/vector.hpp"

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

    ds_lib::Vector<int> ints{};
    ints.push_back(123);
    ints.push_back(456);
    ints.push_back(789);

    kernel::write_number(ints.front());
    kernel::putc('\n');
    kernel::write_number(ints.back());
    kernel::putc('\n');

    for (auto it = ints.cbegin(); it != ints.cend(); ++it)
    {
        kernel::write_number(*it);
        kernel::putc('\n');
    }

    ints.pop_back();
    ints.shrink_to_fit();

    kernel::write_number(ints.back());
    kernel::putc('\n');

    ints.resize(4, 900);
    kernel::write_number(ints.back());
    kernel::putc('\n');

    ints.assign(3, 11);
    for (auto it = ints.cbegin(); it != ints.cend(); ++it)
    {
        kernel::write_number(*it);
        kernel::putc('\n');
    }

    kernel::panic("Finished Running");
}
