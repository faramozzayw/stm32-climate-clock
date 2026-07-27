import 'package:flutter/foundation.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';

import '../utils/temperature.dart';

enum _TemperatureState { normal, belowMinimum, aboveMaximum }

class TemperatureNotificationService {
  static const _notificationId = 1;
  static const _settings = InitializationSettings(
    android: AndroidInitializationSettings('ic_temperature_notification'),
    iOS: DarwinInitializationSettings(
      requestAlertPermission: false,
      requestSoundPermission: false,
      requestBadgePermission: false,
    ),
    macOS: DarwinInitializationSettings(
      requestAlertPermission: false,
      requestSoundPermission: false,
      requestBadgePermission: false,
    ),
    linux: LinuxInitializationSettings(defaultActionName: 'Open'),
    windows: WindowsInitializationSettings(
      appName: 'Temperature Controller',
      appUserModelId: 'TemperatureController.App',
      guid: '4415f4a4-9186-40b3-9c40-78efd5d75de2',
    ),
    web: WebInitializationSettings(),
  );
  static const _details = NotificationDetails(
    android: AndroidNotificationDetails(
      'temperature_alerts',
      'Temperature alerts',
      channelDescription: 'Alerts when the temperature is outside its limits',
      importance: Importance.high,
      priority: Priority.high,
    ),
    iOS: DarwinNotificationDetails(),
    macOS: DarwinNotificationDetails(),
    linux: LinuxNotificationDetails(urgency: LinuxNotificationUrgency.normal),
    windows: WindowsNotificationDetails(
      duration: WindowsNotificationDuration.long,
    ),
    web: WebNotificationDetails(),
  );

  final _notifications = FlutterLocalNotificationsPlugin();
  bool _available = false;
  _TemperatureState _temperatureState = _TemperatureState.normal;

  Future<void> initialize() async {
    try {
      _available =
          await _notifications.initialize(settings: _settings) ?? false;
    } catch (error) {
      debugPrint('Local notifications are unavailable: $error');
    }
  }

  Future<void> requestPermission() async {
    if (!_available) return;

    try {
      if (kIsWeb) {
        await _notifications
            .resolvePlatformSpecificImplementation<
              WebFlutterLocalNotificationsPlugin
            >()
            ?.requestNotificationsPermission();
        return;
      }

      switch (defaultTargetPlatform) {
        case TargetPlatform.android:
          await _notifications
              .resolvePlatformSpecificImplementation<
                AndroidFlutterLocalNotificationsPlugin
              >()
              ?.requestNotificationsPermission();
        case TargetPlatform.iOS:
          await _notifications
              .resolvePlatformSpecificImplementation<
                IOSFlutterLocalNotificationsPlugin
              >()
              ?.requestPermissions(alert: true, badge: true, sound: true);
        case TargetPlatform.macOS:
          await _notifications
              .resolvePlatformSpecificImplementation<
                MacOSFlutterLocalNotificationsPlugin
              >()
              ?.requestPermissions(alert: true, badge: true, sound: true);
        case TargetPlatform.fuchsia:
        case TargetPlatform.linux:
        case TargetPlatform.windows:
          break;
      }
    } catch (error) {
      debugPrint('Notification permission request failed: $error');
    }
  }

  Future<void> updateTemperature({
    required int currentTemperature,
    required int minTemperature,
    required int maxTemperature,
    required bool fahrenheit,
  }) async {
    final _TemperatureState nextState;
    if (currentTemperature < minTemperature) {
      nextState = _TemperatureState.belowMinimum;
    } else if (currentTemperature > maxTemperature) {
      nextState = _TemperatureState.aboveMaximum;
    } else {
      nextState = _TemperatureState.normal;
    }

    if (nextState == _temperatureState) return;
    _temperatureState = nextState;

    switch (nextState) {
      case _TemperatureState.normal:
        return;
      case _TemperatureState.belowMinimum:
        await _show(
          title: '🥶 Temperature below minimum',
          body:
              '🌡️ Current temperature is '
              '${formatTemperatureWithUnit(currentTemperature, fahrenheit: fahrenheit)}. '
              'Minimum is '
              '${formatTemperatureWithUnit(minTemperature, fahrenheit: fahrenheit)}.',
        );
      case _TemperatureState.aboveMaximum:
        await _show(
          title: '🔥 Temperature above maximum',
          body:
              '🌡️ Current temperature is '
              '${formatTemperatureWithUnit(currentTemperature, fahrenheit: fahrenheit)}. '
              'Maximum is '
              '${formatTemperatureWithUnit(maxTemperature, fahrenheit: fahrenheit)}.',
        );
    }
  }

  void resetTemperatureState() {
    _temperatureState = _TemperatureState.normal;
  }

  Future<void> _show({required String title, required String body}) async {
    if (!_available) return;

    await _notifications.show(
      id: _notificationId,
      title: title,
      body: body,
      notificationDetails: _details,
    );
  }
}
