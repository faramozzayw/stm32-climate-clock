import 'package:app/services/limit_alert_tracker.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  test('reports only new limit violations', () {
    final tracker = LimitAlertTracker();

    expect(tracker.update(value: 20, minimum: 10, maximum: 30), isNull);
    expect(
      tracker.update(value: 5, minimum: 10, maximum: 30),
      LimitAlert.belowMinimum,
    );
    expect(tracker.update(value: 4, minimum: 10, maximum: 30), isNull);
    expect(
      tracker.update(value: 35, minimum: 10, maximum: 30),
      LimitAlert.aboveMaximum,
    );
    expect(tracker.update(value: 20, minimum: 10, maximum: 30), isNull);
    expect(
      tracker.update(value: 5, minimum: 10, maximum: 30),
      LimitAlert.belowMinimum,
    );
  });

  test('reset allows the active violation to be reported again', () {
    final tracker = LimitAlertTracker();

    expect(
      tracker.update(value: 5, minimum: 10, maximum: 30),
      LimitAlert.belowMinimum,
    );

    tracker.reset();

    expect(
      tracker.update(value: 5, minimum: 10, maximum: 30),
      LimitAlert.belowMinimum,
    );
  });
}
