#!/usr/bin/env bash
set -euo pipefail

tests_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${tests_dir}"

build_dir=".build"
test_executable="${build_dir}/command_receiver_tests"
compiler="${CC:-gcc}"

if ! command -v "${compiler}" >/dev/null 2>&1; then
    echo "Error: ${compiler} was not found on PATH." >&2
    exit 1
fi

mkdir -p "${build_dir}"

"${compiler}" \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror \
    -Ifakes \
    -I../Core/Inc \
    -I../Protocol \
    -I../Middlewares/Third_Party/nanopb \
    test_command_receiver.c \
    ../Core/Src/command_receiver/uart_frame.c \
    ../Core/Src/command_receiver/device_command_decoder.c \
    ../Core/Src/command_receiver/uart_command_receiver.c \
    ../Protocol/device.pb.c \
    ../Middlewares/Third_Party/nanopb/pb_common.c \
    ../Middlewares/Third_Party/nanopb/pb_decode.c \
    ../Middlewares/Third_Party/nanopb/pb_encode.c \
    -o "${test_executable}"

"${test_executable}"
