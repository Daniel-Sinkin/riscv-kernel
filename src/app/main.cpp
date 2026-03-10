#include "common.hpp"
#include "kernel/console.hpp"
#include "lib/optional.hpp"

struct NonTrivial
{
    NonTrivial() : data_(0)
    {
        kernel::printfn("NonTrivial() Constructor called");
    }
    template <typename T>
        requires requires(T t) {
            { static_cast<u32>(t) };
        }
    NonTrivial(T data) : data_{static_cast<u32>(data)}
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
    lib::Optional<NonTrivial> o{std::in_place, 5};
    lib::Optional<NonTrivial> copied{o};
    lib::Optional<NonTrivial> moved{std::move(copied)};

    kernel::printfn("%d", static_cast<int>(moved->data()));
    kernel::printfn("%d", static_cast<int>(copied.has_value()));

    return 0;
}
