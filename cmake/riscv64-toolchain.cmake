set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(RISCV_TOOLCHAIN_PREFIX "" CACHE STRING "Prefix for the RISC-V GNU toolchain, for example riscv64-unknown-elf or /opt/homebrew/bin/riscv64-unknown-elf")

if(NOT RISCV_TOOLCHAIN_PREFIX)
    foreach(candidate
        riscv64-unknown-elf
        riscv64-none-elf
        riscv64-elf
    )
        find_program(candidate_cxx NAMES ${candidate}-g++)
        if(candidate_cxx)
            set(RISCV_TOOLCHAIN_PREFIX "${candidate}")
            break()
        endif()
    endforeach()
endif()

if(NOT RISCV_TOOLCHAIN_PREFIX)
    message(FATAL_ERROR
        "No RISC-V GCC cross-toolchain was found in PATH. "
        "Install one and rerun CMake with "
        "-DRISCV_TOOLCHAIN_PREFIX=<prefix>, for example riscv64-unknown-elf."
    )
endif()

set(CMAKE_C_COMPILER "${RISCV_TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${RISCV_TOOLCHAIN_PREFIX}-g++")
set(CMAKE_ASM_COMPILER "${RISCV_TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_AR "${RISCV_TOOLCHAIN_PREFIX}-ar")
set(CMAKE_RANLIB "${RISCV_TOOLCHAIN_PREFIX}-ranlib")
set(CMAKE_OBJCOPY "${RISCV_TOOLCHAIN_PREFIX}-objcopy" CACHE FILEPATH "objcopy for the RISC-V toolchain")
set(CMAKE_OBJDUMP "${RISCV_TOOLCHAIN_PREFIX}-objdump" CACHE FILEPATH "objdump for the RISC-V toolchain")
