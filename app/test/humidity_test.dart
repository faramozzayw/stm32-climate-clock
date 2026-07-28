import 'package:app/utils/humidity.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  test('formats milli-percent humidity with one decimal place', () {
    expect(formatHumidityMilliPercent(0), '0.0%');
    expect(formatHumidityMilliPercent(60049), '60.0%');
    expect(formatHumidityMilliPercent(60050), '60.1%');
    expect(formatHumidityMilliPercent(100000), '100.0%');
  });

  test('formats tenths-percent humidity limits', () {
    expect(formatHumidityTenthsPercent(0), '0.0%');
    expect(formatHumidityTenthsPercent(300), '30.0%');
    expect(formatHumidityTenthsPercent(1000), '100.0%');
  });
}
