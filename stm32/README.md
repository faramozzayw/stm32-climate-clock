# STM32 project notes

nanopb is integrated through the repository-level `third_party/nanopb` Git
submodule. The runtime wrapper files in `Core/Src/nanopb` keep nanopb sources
inside CubeIDE's existing `Core` source entry, so CubeMX cannot delete the
submodule when regenerating the STM32 project. Generated message bindings live
in `Protocol`.

Forwarding headers in `Core/Inc` use CubeIDE's existing include path, so nanopb
does not require a custom `.cproject` source or include entry. Regenerating code
from `temp.ioc` may still rewrite `.cproject`; after regeneration, verify that
both Debug and Release retain the `Protocol` source entry.

## Host tests

The host suite uses
[ThrowTheSwitch Unity](https://github.com/ThrowTheSwitch/Unity) and compiles
production modules against small HAL fakes. Unity and nanopb are Git
submodules, so initialize dependencies before running the tests:

```console
git submodule update --init --recursive
```

Configure, build, and run the suite from the repository root:

```console
cmake -S stm32/Tests -B stm32/Tests/.build/cmake -G Ninja \
  -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build stm32/Tests/.build/cmake
ctest --test-dir stm32/Tests/.build/cmake --output-on-failure
```

The CMake project supports standard host toolchains and treats compiler warnings
as errors.
