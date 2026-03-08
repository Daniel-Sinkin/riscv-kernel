#include "riscv/trap.hpp"

#include <concepts>

#include "kernel/console.hpp"
#include "kernel/panic.hpp"

namespace {

constexpr u64 k_trap_interrupt_bit{u64{1} << 63};
constexpr usize k_trap_label_width{5};

auto trap_cause_name(bool is_interrupt, u64 cause_code) -> const char * {
    if (is_interrupt) {
        switch (cause_code) {
        case 1:
            return "Supervisor software interrupt";
        case 3:
            return "Machine software interrupt";
        case 5:
            return "Supervisor timer interrupt";
        case 7:
            return "Machine timer interrupt";
        case 9:
            return "Supervisor external interrupt";
        case 11:
            return "Machine external interrupt";
        case 13:
            return "Counter-overflow interrupt";
        default:
            return "Unknown interrupt";
        }
    }

    switch (cause_code) {
    case 0:
        return "Instruction address misaligned";
    case 1:
        return "Instruction access fault";
    case 2:
        return "Illegal instruction";
    case 3:
        return "Breakpoint";
    case 4:
        return "Load address misaligned";
    case 5:
        return "Load access fault";
    case 6:
        return "Store or AMO address misaligned";
    case 7:
        return "Store or AMO access fault";
    case 8:
        return "Environment call from U-mode";
    case 9:
        return "Environment call from S-mode";
    case 11:
        return "Environment call from M-mode";
    case 12:
        return "Instruction page fault";
    case 13:
        return "Load page fault";
    case 15:
        return "Store or AMO page fault";
    default:
        return "Unknown exception";
    }
}

auto write_label(const char *name) -> void {
    usize name_length = 0;
    while (name[name_length] != '\0') {
        ++name_length;
    }
    kernel::puts(name);
    while (name_length < k_trap_label_width) {
        kernel::putc(' ');
        ++name_length;
    }
    kernel::puts(": ");
}

template <std::unsigned_integral U>
auto write_named_hex(const char *name, U value) -> void {
    write_label(name);
    kernel::write_hex(value);
    kernel::putc('\n');
}

auto write_named_text(const char *name, const char *value) -> void {
    write_label(name);
    kernel::puts(value);
    kernel::putc('\n');
}

}  // namespace

extern "C" [[noreturn]] void trap_handler(u64 mcause, u64 mepc, u64 mtval) {
    const auto is_interrupt = (mcause & k_trap_interrupt_bit) != 0;
    const auto cause_code = mcause & ~k_trap_interrupt_bit;

    kernel::putsln("Trap handler");
    write_named_text("kind", is_interrupt ? "interrupt" : "exception");
    write_named_text("cause", trap_cause_name(is_interrupt, cause_code));
    write_named_hex("mepc", mepc);
    write_named_hex("mtval", mtval);
    kernel::panic("trap_handler panic");
}
