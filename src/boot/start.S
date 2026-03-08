# .section gives code until next .section or EOF a string identifer
# text == executable
# init b.c. this is initialisation code,
# so basically we say this is the executable initialisaiton code
# This puts the following emitted code into the section named .text.init
.section .text.init 


# Makes _start globally visible, b.c. of ENTRY(_start) this is the entry point
.globl _start
# Declares that _start is a function symbol
.type _start, @function
# Begin of _start function
_start:
    # .option is assembler directive for changing assembler option
    # Save the assembler's current option settings
    .option push
    # Disable relaxation (roughly disallows replacing instructions) in this region
    .option norelax
    # la                = load address
    # gp                = RISC-V global pointer register
    # __global_pointer$ = linker-defined symbol; the $ is just convention
    la gp, __global_pointer$
    # Restore the assembler's option settings to before
    .option pop

    # la          = load address
    # sp          = stack pointer
    # __stack_top = Linker-defined symbol pointing
    #               to the start (i.e. top) of the stack
    la sp, __stack_top

    la t0, trap_entry
    csrw mtvec, t0

    # Calls into the kernel_main function in the main.cpp
    call kernel_main

# Local numeric label
1: 
    # (W)ait (F)or (I)nterrupt
    wfi
    # jump to the nearest previous '1:' label
    j 1b


# symbol_size(_start) = current_address - address(_start)
.size _start, . - _start
# End of _start function

.globl trap_entry
.type trap_entry, @function
.align 2
trap_entry:
    csrr a0, mcause
    csrr a1, mepc
    csrr a2, mtval
    call trap_handler

2:
    wfi
    j 2b
.size trap_entry, . - trap_entry
