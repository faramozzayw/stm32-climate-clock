import 'package:app/widgets/current_conditions_card.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  testWidgets('shows live temperature and humidity', (tester) async {
    await tester.pumpWidget(
      const MaterialApp(
        home: Scaffold(
          body: CurrentConditionsCard(
            temperatureTenths: 253,
            humidityMilliPercent: 60075,
            fahrenheit: false,
          ),
        ),
      ),
    );

    expect(find.text('Current conditions'), findsOneWidget);
    expect(find.text('25.3 °C'), findsOneWidget);
    expect(find.text('60.1%'), findsOneWidget);
  });

  testWidgets('shows placeholders while waiting for telemetry', (tester) async {
    await tester.pumpWidget(
      const MaterialApp(
        home: Scaffold(
          body: CurrentConditionsCard(
            temperatureTenths: null,
            humidityMilliPercent: null,
            fahrenheit: true,
          ),
        ),
      ),
    );

    expect(find.text('Waiting for device data'), findsOneWidget);
    expect(find.text('--.- °F'), findsOneWidget);
    expect(find.text('--.-%'), findsOneWidget);
  });
}
