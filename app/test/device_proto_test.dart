import 'package:app/generated/device.pb.dart';
import 'package:fixnum/fixnum.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  test('serializes and parses every device command', () {
    final commands = [
      DeviceCommand(setMaxTemp: SetMaxTemp(value: 250)),
      DeviceCommand(setMinTemp: SetMinTemp(value: 155)),
      DeviceCommand(
        setCurrentTime: SetCurrentTime(valueMs: Int64(1700000000000)),
      ),
    ];
    final messages = [
      for (final command in commands) DeviceMessage(command: command),
    ];

    expect(messages[0].writeToBuffer(), [
      0x0A,
      0x05,
      0x0A,
      0x03,
      0x08,
      0xF4,
      0x03,
    ]);
    expect(messages[1].writeToBuffer(), [
      0x0A,
      0x05,
      0x12,
      0x03,
      0x08,
      0xB6,
      0x02,
    ]);
    expect(messages[2].writeToBuffer(), [
      0x0A,
      0x09,
      0x1A,
      0x07,
      0x08,
      0x80,
      0xD0,
      0x95,
      0xFF,
      0xBC,
      0x31,
    ]);

    expect(
      DeviceMessage.fromBuffer(
        messages[0].writeToBuffer(),
      ).command.setMaxTemp.value,
      250,
    );
    expect(
      DeviceMessage.fromBuffer(
        messages[1].writeToBuffer(),
      ).command.setMinTemp.value,
      155,
    );
    final negative = DeviceCommand(setMinTemp: SetMinTemp(value: -55));
    final negativeMessage = DeviceMessage(command: negative);
    expect(negativeMessage.writeToBuffer(), [
      0x0A,
      0x04,
      0x12,
      0x02,
      0x08,
      0x6D,
    ]);
    expect(
      DeviceMessage.fromBuffer(
        negativeMessage.writeToBuffer(),
      ).command.setMinTemp.value,
      -55,
    );
    expect(
      DeviceMessage.fromBuffer(
        messages[2].writeToBuffer(),
      ).command.setCurrentTime.valueMs,
      Int64(1700000000000),
    );
  });

  test('serializes and parses device telemetry', () {
    final telemetry = DeviceTelemetry(
      currentTemp: -55,
      minTemp: 100,
      maxTemp: 300,
    );

    final message = DeviceMessage(telemetry: telemetry);
    final decoded = DeviceMessage.fromBuffer(message.writeToBuffer()).telemetry;

    expect(decoded.currentTemp, -55);
    expect(decoded.minTemp, 100);
    expect(decoded.maxTemp, 300);
  });

  test('serializes and parses bridge status', () {
    final connecting = DeviceMessage(
      bridgeStatus: BridgeStatus(
        bleConnectionState: BleConnectionState.BLE_CONNECTION_STATE_CONNECTING,
      ),
    );
    final connected = DeviceMessage(
      bridgeStatus: BridgeStatus(
        bleConnectionState: BleConnectionState.BLE_CONNECTION_STATE_CONNECTED,
      ),
    );
    final disconnecting = DeviceMessage(
      bridgeStatus: BridgeStatus(
        bleConnectionState:
            BleConnectionState.BLE_CONNECTION_STATE_DISCONNECTING,
      ),
    );
    final disconnected = DeviceMessage(
      bridgeStatus: BridgeStatus(
        bleConnectionState:
            BleConnectionState.BLE_CONNECTION_STATE_DISCONNECTED,
      ),
    );

    expect(
      DeviceMessage.fromBuffer(
        connecting.writeToBuffer(),
      ).bridgeStatus.bleConnectionState,
      BleConnectionState.BLE_CONNECTION_STATE_CONNECTING,
    );
    expect(
      DeviceMessage.fromBuffer(
        connected.writeToBuffer(),
      ).bridgeStatus.bleConnectionState,
      BleConnectionState.BLE_CONNECTION_STATE_CONNECTED,
    );
    expect(
      DeviceMessage.fromBuffer(
        disconnecting.writeToBuffer(),
      ).bridgeStatus.bleConnectionState,
      BleConnectionState.BLE_CONNECTION_STATE_DISCONNECTING,
    );
    expect(
      DeviceMessage.fromBuffer(
        disconnected.writeToBuffer(),
      ).bridgeStatus.bleConnectionState,
      BleConnectionState.BLE_CONNECTION_STATE_DISCONNECTED,
    );
  });
}
