cmake_minimum_required(VERSION 3.20)
project(climateclock_host_tests LANGUAGES C)

include(CTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
set(STM32 "${CMAKE_CURRENT_LIST_DIR}/..")
set(UNITY "${ROOT}/third_party/unity")
set(NANOPB "${ROOT}/third_party/nanopb")

if(NOT EXISTS "${UNITY}/src/unity.c" OR NOT EXISTS "${NANOPB}/pb_common.c")
	message(FATAL_ERROR
		"Git submodules are missing. Run "
		"'git submodule update --init --recursive'.")
endif()

if(MSVC)
	add_compile_options(/W4 /WX)
	set(MATH_LIBRARY)
else()
	add_compile_options(-Wall -Wextra -Werror)
	set(MATH_LIBRARY m)
endif()

add_library(unity STATIC "${UNITY}/src/unity.c")
target_include_directories(unity PUBLIC "${UNITY}/src")

add_library(device_protocol STATIC
	"${STM32}/Protocol/device.pb.c"
	"${NANOPB}/pb_common.c"
	"${NANOPB}/pb_decode.c"
	"${NANOPB}/pb_encode.c")
target_include_directories(device_protocol PUBLIC
	"${STM32}/Protocol"
	"${NANOPB}")

add_library(uart_framing STATIC
	"${ROOT}/shared/uart_framing/uart_framing.c")
target_include_directories(uart_framing PUBLIC
	"${ROOT}/shared/uart_framing")

function(add_climateclock_test target)
	add_executable(${target} ${ARGN})
	target_include_directories(${target} PRIVATE "${STM32}/Core/Inc")
	target_link_libraries(${target} PRIVATE unity)
	string(REGEX REPLACE "_tests$" "" test_name "${target}")
	add_test(NAME "${test_name}" COMMAND ${target})
endfunction()

add_climateclock_test(command_receiver_tests
	test_command_receiver.c
	"${STM32}/Core/Src/utils/byte_ring_buffer.c"
	"${STM32}/Core/Src/command_receiver/device_message_decoder.c"
	"${STM32}/Core/Src/command_receiver/uart_command_receiver.c")
target_link_libraries(command_receiver_tests PRIVATE
	device_protocol uart_framing)

add_climateclock_test(pure_utils_tests
	test_pure_utils.c
	"${STM32}/Core/Src/utils/temperature.c"
	"${STM32}/Core/Src/utils/byte_codec.c"
	"${STM32}/Core/Src/utils/byte_ring_buffer.c"
	"${STM32}/Core/Src/utils/calendar_time.c")
target_link_libraries(pure_utils_tests PRIVATE ${MATH_LIBRARY})

add_climateclock_test(telemetry_tests
	test_telemetry.c
	"${STM32}/Core/Src/telemetry.c")
target_link_libraries(telemetry_tests PRIVATE
	device_protocol uart_framing)

add_climateclock_test(app_settings_tests
	test_app_settings.c
	"${STM32}/Core/Src/app_settings.c"
	"${STM32}/Core/Src/utils/byte_codec.c")
target_include_directories(app_settings_tests BEFORE PRIVATE fakes)

add_climateclock_test(hal_uart_receiver_tests
	test_hal_uart_receiver.c
	"${STM32}/Core/Src/platform/hal_uart_receiver.c")
target_include_directories(hal_uart_receiver_tests BEFORE PRIVATE fakes)

add_climateclock_test(screen_tests
	test_screen.c
	"${STM32}/Core/Src/app/screen.c"
	"${STM32}/Core/Src/utils/temperature.c")
target_include_directories(screen_tests BEFORE PRIVATE
	fakes "${STM32}/Core/Src")

add_climateclock_test(lcd1602_tests
	test_lcd1602.c
	"${STM32}/Core/Src/drivers/lcd1602.c")
target_include_directories(lcd1602_tests PRIVATE fakes)
