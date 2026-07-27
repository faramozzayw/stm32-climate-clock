set shell := ["bash", "-eu", "-o", "pipefail", "-c"]

# Format custom source files without touching CubeMX or third-party code.
[group("lint")]
format:
    shopt -s globstar nullglob; files=(shared/uart_framing/*.{c,h} stm32/Core/Inc/{app,command_receiver,drivers,platform,utils}/**/*.{c,h,cpp,hpp,ino} stm32/Core/Src/{app,command_receiver,drivers,platform,utils}/**/*.{c,h,cpp,hpp,ino} stm32/Core/Inc/{app_settings,telemetry}.h stm32/Core/Src/{app_settings,telemetry}.c stm32/Core/Src/retarget.c esp32_bridge/bridge_uart/*.{c,h,ino}); clang-format -i --style=file -- "${files[@]}"

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

[group("test")]
ctest:
    cmake -S stm32/Tests -B stm32/Tests/.build/cmake -G Ninja -DCMAKE_C_COMPILER="${CC:-gcc}"
    cmake --build stm32/Tests/.build/cmake
    ctest --test-dir stm32/Tests/.build/cmake --output-on-failure
