String formatHumidityMilliPercent(int humidityMilliPercent) {
  final humidityTenths = (humidityMilliPercent + 50) ~/ 100;
  return formatHumidityTenthsPercent(humidityTenths);
}

String formatHumidityTenthsPercent(int humidityTenthsPercent) {
  return '${humidityTenthsPercent ~/ 10}.${humidityTenthsPercent % 10}%';
}
