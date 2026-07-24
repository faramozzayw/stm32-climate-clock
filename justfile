set shell := ["bash", "-eu", "-o", "pipefail", "-c"]

# Format custom source files without touching CubeMX or third-party code.
[group("lint")]
format:
    shopt -s globstar nullglob; files=(shared/uart_framing/*.{c,h} stm32/Core/Inc/{app,command_receiver,drivers}/**/*.{c,h,cpp,hpp,ino} stm32/Core/Src/{app,command_receiver,drivers}/**/*.{c,h,cpp,hpp,ino} stm32/Core/Inc/utils.h stm32/Core/Inc/temperature_settings.h stm32/Core/Src/utils.c stm32/Core/Src/temperature_settings.c stm32/Core/Src/retarget.c esp32_bridge/bridge_uart/*.{c,h,ino}); clang-format -i --style=file -- "${files[@]}"

# Regenerate STM32 nanopb and Flutter bindings from protocol/device.proto.
[group("generation")]
protobuf:
    python protocol/generate_nanopb.py generate
    python protocol/generate_dart.py

# Configure Flutter's Android SDK, accept its licenses, and validate the toolchain.
[group("android")]
android-setup:
    sdk_path="${ANDROID_SDK_ROOT:-${LOCALAPPDATA}/Android/Sdk}"; flutter config --android-sdk "$sdk_path"
    flutter doctor --android-licenses
    flutter doctor

# Build a universal release APK for direct installation.
[group("android")]
apk:
    cd app && flutter build apk --release
