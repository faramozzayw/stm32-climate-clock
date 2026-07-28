enum LimitAlert { belowMinimum, aboveMaximum }

class LimitAlertTracker {
  LimitAlert? _activeAlert;

  LimitAlert? update({
    required int value,
    required int minimum,
    required int maximum,
  }) {
    final LimitAlert? nextAlert;
    if (value < minimum) {
      nextAlert = LimitAlert.belowMinimum;
    } else if (value > maximum) {
      nextAlert = LimitAlert.aboveMaximum;
    } else {
      nextAlert = null;
    }

    if (nextAlert == _activeAlert) return null;

    _activeAlert = nextAlert;
    return nextAlert;
  }

  void reset() {
    _activeAlert = null;
  }
}
