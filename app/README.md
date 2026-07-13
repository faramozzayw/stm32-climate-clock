# Temperature Controller

Minimal Android Flutter application that finds `ESP32_BRIDGE` over BLE and
writes protobuf `DeviceCommand` messages to its Nordic UART RX characteristic.

Run it on an Android phone with:

```sh
flutter run
```

The temperature fields accept signed Celsius with exactly zero or one decimal digit.
They are converted to tenths of a degree before protobuf serialization. The
time command sends the phone's Unix epoch time in milliseconds.
