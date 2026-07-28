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
      DeviceCommand(
        setTemperatureUnit: SetTemperatureUnit(
          unit: TemperatureUnit.TEMPERATURE_UNIT_FAHRENHEIT,
        ),
      ),
      DeviceCommand(setMaxHumidity: SetMaxHumidity(value: 600)),
      DeviceCommand(setMinHumidity: SetMinHumidity(value: 300)),
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
    expect(messages[3].writeToBuffer(), [0x0A, 0x04, 0x22, 0x02, 0x08, 0x01]);
    expect(messages[4].writeToBuffer(), [
      0x0A,
      0x05,
      0x2A,
      0x03,
      0x08,
      0xD8,
      0x04,
    ]);
    expect(messages[5].writeToBuffer(), [
      0x0A,
      0x05,
      0x32,
      0x03,
      0x08,
      0xAC,
      0x02,
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
    expect(
      DeviceMessage.fromBuffer(
        messages[3].writeToBuffer(),
      ).command.setTemperatureUnit.unit,
      TemperatureUnit.TEMPERATURE_UNIT_FAHRENHEIT,
    );
    expect(
      DeviceMessage.fromBuffer(
        messages[4].writeToBuffer(),
      ).command.setMaxHumidity.value,
      600,
    );
    expect(
      DeviceMessage.fromBuffer(
        messages[5].writeToBuffer(),
      ).command.setMinHumidity.value,
      300,
    );
  });

  test('serializes and parses environment measurement telemetry', () {
    final telemetry = DeviceTelemetry(
      measurement: EnvironmentMeasurement(
        temperatureTenthsCelsius: -55,
        humidityMilliPercent: 60075,
      ),
    );

    final message = DeviceMessage(telemetry: telemetry);
    final decoded = DeviceMessage.fromBuffer(message.writeToBuffer()).telemetry;

    expect(message.writeToBuffer().length, lessThanOrEqualTo(20));
    expect(decoded.hasMeasurement(), isTrue);
    expect(decoded.measurement.temperatureTenthsCelsius, -55);
    expect(decoded.measurement.hasHumidityMilliPercent(), isTrue);
    expect(decoded.measurement.humidityMilliPercent, 60075);
  });

  test('serializes and parses environment limits telemetry', () {
    final telemetry = DeviceTelemetry(
      limits: EnvironmentLimits(
        minTemperatureTenthsCelsius: 100,
        maxTemperatureTenthsCelsius: 300,
        minHumidityTenthsPercent: 300,
        maxHumidityTenthsPercent: 650,
      ),
    );

    final message = DeviceMessage(telemetry: telemetry);
    final decoded = DeviceMessage.fromBuffer(message.writeToBuffer()).telemetry;

    expect(message.writeToBuffer().length, lessThanOrEqualTo(20));
    expect(decoded.hasLimits(), isTrue);
    expect(decoded.limits.minTemperatureTenthsCelsius, 100);
    expect(decoded.limits.maxTemperatureTenthsCelsius, 300);
    expect(decoded.limits.minHumidityTenthsPercent, 300);
    expect(decoded.limits.maxHumidityTenthsPercent, 650);
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
