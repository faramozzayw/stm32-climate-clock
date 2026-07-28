import 'package:app/widgets/limit_command.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  testWidgets('shows the current device limit and input suffix', (
    tester,
  ) async {
    final controller = TextEditingController(text: '60.0');
    addTearDown(controller.dispose);

    await tester.pumpWidget(
      MaterialApp(
        home: Scaffold(
          body: LimitCommand(
            label: 'Maximum humidity',
            hint: 'Upper comfort limit',
            icon: Icons.water_drop,
            accent: Colors.blue,
            surface: Colors.white,
            controller: controller,
            currentLimitText: '65.0%',
            suffix: '%',
            allowSigned: false,
            onSend: null,
          ),
        ),
      ),
    );

    expect(find.text('Current device limit: 65.0%'), findsOneWidget);
    expect(find.text('%'), findsOneWidget);
  });
}
