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

    expect(commands[0].writeToBuffer(), [0x0A, 0x03, 0x08, 0xF4, 0x03]);
    expect(commands[1].writeToBuffer(), [0x12, 0x03, 0x08, 0xB6, 0x02]);
    expect(commands[2].writeToBuffer(), [
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
      DeviceCommand.fromBuffer(commands[0].writeToBuffer()).setMaxTemp.value,
      250,
    );
    expect(
      DeviceCommand.fromBuffer(commands[1].writeToBuffer()).setMinTemp.value,
      155,
    );
    final negative = DeviceCommand(setMinTemp: SetMinTemp(value: -55));
    expect(negative.writeToBuffer(), [0x12, 0x02, 0x08, 0x6D]);
    expect(
      DeviceCommand.fromBuffer(negative.writeToBuffer()).setMinTemp.value,
      -55,
    );
    expect(
      DeviceCommand.fromBuffer(
        commands[2].writeToBuffer(),
      ).setCurrentTime.valueMs,
      Int64(1700000000000),
    );
  });
}
