import 'package:app/utils/temperature.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  test('converts fixed-point temperatures between Celsius and Fahrenheit', () {
    expect(celsiusToFahrenheitTenths(0), 320);
    expect(celsiusToFahrenheitTenths(100), 500);
    expect(celsiusToFahrenheitTenths(-400), -400);

    expect(fahrenheitToCelsiusTenths(320), 0);
    expect(fahrenheitToCelsiusTenths(500), 100);
    expect(fahrenheitToCelsiusTenths(-400), -400);
  });

  test('formats a Celsius temperature for the selected display unit', () {
    expect(formatTemperatureWithUnit(0, fahrenheit: false), '0.0 °C');
    expect(formatTemperatureWithUnit(0, fahrenheit: true), '32.0 °F');
    expect(formatTemperatureWithUnit(-55, fahrenheit: false), '-5.5 °C');
  });

  test('formats fixed-point tenths with one decimal place', () {
    expect(formatTemperatureTenths(123), '12.3');
    expect(formatTemperatureTenths(-5), '-0.5');
  });
}
