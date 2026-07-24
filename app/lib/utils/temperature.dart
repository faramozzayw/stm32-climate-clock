int celsiusToFahrenheitTenths(int celsiusTenths) {
  return ((celsiusTenths * 9) / 5 + 320).round();
}

int fahrenheitToCelsiusTenths(int fahrenheitTenths) {
  return (((fahrenheitTenths - 320) * 5) / 9).round();
}

String formatTemperatureTenths(int temperatureTenths) {
  final magnitude = temperatureTenths.abs();
  final sign = temperatureTenths < 0 ? '-' : '';
  return '$sign${magnitude ~/ 10}.${magnitude % 10}';
}

String formatTemperatureWithUnit(
  int celsiusTenths, {
  required bool fahrenheit,
}) {
  final displayTemperature = fahrenheit
      ? celsiusToFahrenheitTenths(celsiusTenths)
      : celsiusTenths;
  final unit = fahrenheit ? 'F' : 'C';
  return '${formatTemperatureTenths(displayTemperature)} °$unit';
}
