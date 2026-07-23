# STM32 project notes

nanopb is integrated through the `Middlewares/Third_Party/nanopb` Git submodule and manual entries in `.cproject`. Generated message bindings live in `Protocol`. Regenerating code from `temp.ioc` may rewrite `.cproject` and remove the nanopb or `Protocol` source-folder/include-path entries; after regeneration, verify both Debug and Release.

## Host tests

The command receiver tests compile the production frame parser, protobuf decoder,
and UART receiver against a small HAL UART fake. Run them from `stm32` with:

```console
bash ./Tests/run_tests.sh
```

The runner requires `gcc` on `PATH` and treats compiler warnings as errors.
