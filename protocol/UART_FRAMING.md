# UART command framing

Flutter sends one raw `device.DeviceCommand` protobuf message per BLE write. The ESP32 bridge wraps that BLE payload before forwarding it to the STM32 UART; the BLE representation itself is unchanged.

## Why framing is needed

BLE delivers each command to the ESP32 as a distinct write, but UART is only a continuous stream of bytes: it does not preserve message boundaries. Without framing, the STM32 cannot reliably tell where one protobuf message ends and the next begins, especially if commands arrive back-to-back or a byte is dropped or corrupted.

The magic bytes let the receiver find the start of the next frame, the length identifies the complete protobuf payload, and the CRC rejects damaged frames. Together they allow the STM32 parser to recover from line noise or incomplete data instead of accidentally decoding subsequent bytes as part of the wrong command.

Each UART frame is:

| Offset | Size | Field |
| --- | ---: | --- |
| 0 | 1 | Magic byte `0xA5` |
| 1 | 1 | Magic byte `0x5A` |
| 2 | 2 | Protobuf payload length, unsigned little-endian |
| 4 | N | Raw protobuf payload (1-32 bytes) |
| 4 + N | 2 | CRC-16/CCITT-FALSE, unsigned little-endian |

The CRC starts at `0xFFFF`, uses polynomial `0x1021`, and covers the two encoded length bytes followed by the protobuf payload. It does not cover the magic bytes or the CRC field.

For example, `SetMaxTemp(value: 250)` is transmitted as `A5 5A 05 00 0A 03 08 F4 03 DC 13`.

## Regenerating the STM32 bindings

The checked-in `stm32/Protocol/device.pb.c` and `device.pb.h` files are generated from `device.proto` by the generator in the nanopb Git submodule. From the repository root, run:

```sh
make -C protocol protobuf
```

The generator requires Python with the `protobuf` and `grpcio-tools` packages. Use `make -C protocol check-stm32` to verify that the checked-in files match the schema without modifying them. If the default Python command is unavailable, override it, for example with `make -C protocol protobuf PYTHON=python`.
