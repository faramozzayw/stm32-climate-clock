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

The command receiver tests compile the production frame parser, protobuf decoder,
and UART receiver against a small HAL UART fake. Run them from `stm32` with:

```console
bash ./Tests/run_tests.sh
```

The runner requires `gcc` on `PATH` and treats compiler warnings as errors.
