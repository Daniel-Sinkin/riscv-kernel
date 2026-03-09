#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
ACTION="${1:-build}"
RISCV_ARCH="${RISCV_ARCH:-rv64imac_zicsr}"
RISCV_ABI="${RISCV_ABI:-lp64}"

find_program() {
    local candidate
    for candidate in "$@"; do
        if command -v "${candidate}" >/dev/null 2>&1; then
            command -v "${candidate}"
            return 0
        fi
    done
    return 1
}

find_toolchain_prefix() {
    local prefix
    for prefix in \
        "${RISCV_TOOLCHAIN_PREFIX:-}" \
        "riscv64-unknown-elf" \
        "riscv64-none-elf" \
        "riscv64-elf" \
        "/opt/homebrew/bin/riscv64-unknown-elf" \
        "/opt/homebrew/bin/riscv64-none-elf" \
        "/opt/homebrew/bin/riscv64-elf" \
        "/usr/local/bin/riscv64-unknown-elf" \
        "/usr/local/bin/riscv64-none-elf" \
        "/usr/local/bin/riscv64-elf"
    do
        [ -n "${prefix}" ] || continue
        if [ -x "${prefix}-g++" ]; then
            printf '%s\n' "${prefix}"
            return 0
        fi
        if command -v "${prefix}-g++" >/dev/null 2>&1; then
            printf '%s\n' "${prefix}"
            return 0
        fi
    done
    return 1
}

usage() {
    cat <<'EOF'
usage: ./build.sh [build|run|configure]

  build      configure and build the kernel (default)
  run        configure, build, and start QEMU
  configure  only run CMake configure

optional environment variables:
  RISCV_TOOLCHAIN_PREFIX   e.g. riscv64-unknown-elf or /opt/homebrew/bin/riscv64-unknown-elf
  RISCV_ARCH               defaults to rv64imac_zicsr
  RISCV_ABI                defaults to lp64
  BUILD_DIR                alternate build directory
EOF
}

if [[ "${ACTION}" != "build" && "${ACTION}" != "run" && "${ACTION}" != "configure" ]]; then
    usage
    exit 1
fi

if ! CMAKE_BIN="$(find_program cmake)"; then
    echo "error: cmake is not installed or not in PATH." >&2
    exit 1
fi

if ! NINJA_BIN="$(find_program ninja /opt/homebrew/bin/ninja /usr/local/bin/ninja)"; then
    echo "error: ninja is not installed or not in PATH." >&2
    exit 1
fi

if ! TOOLCHAIN_PREFIX="$(find_toolchain_prefix)"; then
    cat >&2 <<'EOF'
error: no RISC-V GCC cross-toolchain was found.

Install one of the usual prefixes, or set RISCV_TOOLCHAIN_PREFIX manually.
Examples:
  RISCV_TOOLCHAIN_PREFIX=riscv64-unknown-elf ./build.sh
  RISCV_TOOLCHAIN_PREFIX=/opt/homebrew/bin/riscv64-unknown-elf ./build.sh
EOF
    exit 1
fi

CONFIGURE_ARGS=(
    -S "${ROOT_DIR}"
    -B "${BUILD_DIR}"
    -G Ninja
    "-DCMAKE_MAKE_PROGRAM=${NINJA_BIN}"
    "-DCMAKE_TOOLCHAIN_FILE=${ROOT_DIR}/cmake/riscv64-toolchain.cmake"
    "-DRISCV_TOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX}"
    "-DRISCV_ARCH=${RISCV_ARCH}"
    "-DRISCV_ABI=${RISCV_ABI}"
)

echo "Configuring with toolchain prefix: ${TOOLCHAIN_PREFIX}"
"${CMAKE_BIN}" "${CONFIGURE_ARGS[@]}"

if [[ "${ACTION}" == "configure" ]]; then
    exit 0
fi

if [[ "${ACTION}" == "build" ]]; then
    "${CMAKE_BIN}" --build "${BUILD_DIR}" --target kernel
    exit 0
fi

if ! find_program qemu-system-riscv64 >/dev/null; then
    echo "error: qemu-system-riscv64 is not installed or not in PATH." >&2
    exit 1
fi

"${CMAKE_BIN}" --build "${BUILD_DIR}" --target run
