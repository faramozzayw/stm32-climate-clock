set shell := ["bash", "-eu", "-o", "pipefail", "-c"]

# Format custom source files without touching CubeMX or third-party code.
format:
    shopt -s globstar nullglob; files=(stm32/Core/Inc/{app,command_receiver,drivers}/**/*.{c,h,cpp,hpp,ino} stm32/Core/Src/{app,command_receiver,drivers}/**/*.{c,h,cpp,hpp,ino} stm32/Core/Inc/utils.h stm32/Core/Inc/temperature_settings.h stm32/Core/Src/utils.c stm32/Core/Src/temperature_settings.c stm32/Core/Src/retarget.c esp32_bridge/bridge_uart/bridge_uart.ino); clang-format -i --style=file -- "${files[@]}"
