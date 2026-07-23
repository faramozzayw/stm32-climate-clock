# UART framing

Portable C framing shared by the STM32 and ESP32 firmware. The library has no
HAL, Arduino, BLE, protobuf, dynamic-allocation, or operating-system
dependencies.

Each frame contains:

| Bytes | Content |
| --- | --- |
| 2 | Magic bytes `A5 5A` |
| 2 | Little-endian payload length |
| 0–32 | Payload |
| 2 | Little-endian CRC-16/CCITT-FALSE |

The CRC covers the two length bytes and the payload. Receivers use it to reject
corrupted frames before decoding their protobuf payload.

`uart_frame_encode()` creates a complete frame in a caller-owned buffer.
`uart_frame_parser_process()` accepts one byte at a time, making it safe to use
with UART reads that split or combine messages.
