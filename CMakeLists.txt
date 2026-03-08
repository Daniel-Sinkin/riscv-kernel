cmake_minimum_required(VERSION 3.25)

project(riscv_kernel LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(QEMU_SYSTEM_RISCV64 qemu-system-riscv64 REQUIRED)

set(RISCV_ARCH "rv64imac" CACHE STRING "RISC-V ISA string passed to GCC")
set(RISCV_ABI "lp64" CACHE STRING "RISC-V ABI passed to GCC")

add_executable(kernel
    src/start.S
    src/kernel/main.cpp
)

set_target_properties(kernel PROPERTIES SUFFIX ".elf")

target_compile_options(kernel PRIVATE
    -march=${RISCV_ARCH}
    -mabi=${RISCV_ABI}
    -mcmodel=medany
    $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-sized-deallocation>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
)

target_link_options(kernel PRIVATE
    -march=${RISCV_ARCH}
    -mabi=${RISCV_ABI}
    -nostdlib
    -nostartfiles
    -static
    -Wl,--build-id=none
    -Wl,-Map=${CMAKE_BINARY_DIR}/kernel.map
    -T${CMAKE_SOURCE_DIR}/linker.ld
)

add_custom_target(run
    COMMAND ${QEMU_SYSTEM_RISCV64}
            -machine virt
            -m 128M
            -bios none
            -nographic
            -serial mon:stdio
            -kernel $<TARGET_FILE:kernel>
    DEPENDS kernel
    USES_TERMINAL
)
