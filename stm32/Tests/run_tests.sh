#!/usr/bin/env bash
set -euo pipefail

tests_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${tests_dir}"

build_dir=".build"
command_receiver_executable="${build_dir}/command_receiver_tests"
pure_utils_executable="${build_dir}/pure_utils_tests"
telemetry_executable="${build_dir}/telemetry_tests"
hal_uart_receiver_executable="${build_dir}/hal_uart_receiver_tests"
screen_executable="${build_dir}/screen_tests"
lcd1602_executable="${build_dir}/lcd1602_tests"
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
    -I../Core/Inc \
    -I../Protocol \
    -I../../third_party/nanopb \
    test_command_receiver.c \
    ../Core/Src/byte_ring_buffer.c \
    ../Core/Src/command_receiver/uart_frame.c \
    ../Core/Src/command_receiver/device_message_decoder.c \
    ../Core/Src/command_receiver/uart_command_receiver.c \
    ../Protocol/device.pb.c \
    ../../third_party/nanopb/pb_common.c \
    ../../third_party/nanopb/pb_decode.c \
    ../../third_party/nanopb/pb_encode.c \
    -o "${command_receiver_executable}"

"${compiler}" \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror \
    -I../Core/Inc \
    test_pure_utils.c \
    ../Core/Src/temperature.c \
    ../Core/Src/byte_codec.c \
    ../Core/Src/byte_ring_buffer.c \
    ../Core/Src/calendar_time.c \
    -lm \
    -o "${pure_utils_executable}"

"${compiler}" \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror \
    -I../Core/Inc \
    -I../Protocol \
    -I../../third_party/nanopb \
    test_telemetry.c \
    ../Core/Src/telemetry.c \
    ../Core/Src/command_receiver/uart_frame.c \
    ../Protocol/device.pb.c \
    ../../third_party/nanopb/pb_common.c \
    ../../third_party/nanopb/pb_decode.c \
    ../../third_party/nanopb/pb_encode.c \
    -o "${telemetry_executable}"

"${compiler}" \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror \
    -Ifakes \
    -I../Core/Inc \
    test_hal_uart_receiver.c \
    ../Core/Src/platform/hal_uart_receiver.c \
    -o "${hal_uart_receiver_executable}"

"${compiler}" \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror \
    -Ifakes \
    -I../Core/Inc \
    -I../Core/Src \
    test_screen.c \
    ../Core/Src/app/screen.c \
    ../Core/Src/temperature.c \
    -o "${screen_executable}"

"${compiler}" \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror \
    -I../Core/Inc \
    -Ifakes \
    test_lcd1602.c \
    ../Core/Src/drivers/lcd1602.c \
    -o "${lcd1602_executable}"

"${command_receiver_executable}"
"${pure_utils_executable}"
"${telemetry_executable}"
"${hal_uart_receiver_executable}"
"${screen_executable}"
"${lcd1602_executable}"
