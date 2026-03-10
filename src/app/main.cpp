#include "common.hpp"
#include "kernel/heap.hpp"
#include "util.hpp"

#include <kernel/console.hpp>
#include <lib/vector.hpp>

struct NonTrivial
{
    NonTrivial() : data_(0)
    {
        kernel::printfn("NonTrivial() Constructor called");
    }
    NonTrivial(u32 data) : data_{data}
    {
        kernel::printfn("NonTrivial(u32) Constructor called");
    }
    ~NonTrivial()
    {
        kernel::printfn("NonTrivial() Destructor called");
    }

    auto data() const noexcept -> u32
    {
        return data_;
    }

  private:
    u32 data_{};
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    auto p = static_cast<NonTrivial*>(kernel::malloc(sizeof(NonTrivial)));
    ScopeExit se{[&] { kernel::free(p); }};
    if (!p)
    {
        kernel::panic("Failed to malloc()");
    }
    auto nptr = new (p) NonTrivial{5};
    ScopeExit se2{[&] { nptr->~NonTrivial(); }};
    return 0;
}
