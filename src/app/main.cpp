#include "common.hpp"
#include "kernel/console.hpp"
#include "lib/expected.hpp"
#include "lib/optional.hpp"
#include "lib/vector.hpp"

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
    lib::Optional<int> empty{};
    lib::Expected<int, int> ok{42};
    lib::Expected<int, int> err{lib::Unexpected<int>{9}};
    lib::Vector<NonTrivial> vec{};
    NonTrivial eleven{11};
    vec.push_back(eleven);
    vec.emplace_back(12);

    kernel::printfn("%d", static_cast<int>(moved->data()));
    kernel::printfn("%d", static_cast<int>(copied.has_value()));
    kernel::printfn("%d", empty.value_or(7));
    kernel::printfn("%d", ok.value_or(0));
    kernel::printfn("%d", err.value_or(0));
    kernel::printfn("%d", err.error());
    kernel::printfn("%d", static_cast<int>(vec.front().data()));
    kernel::printfn("%d", static_cast<int>(vec.back().data()));
    vec.pop_back();
    kernel::printfn("%d", static_cast<int>(vec.size()));

    return 0;
}
