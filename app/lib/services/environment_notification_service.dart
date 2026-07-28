import 'package:flutter/foundation.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';

import '../utils/humidity.dart';
import '../utils/temperature.dart';
import 'limit_alert_tracker.dart';

class EnvironmentNotificationService {
  static const _temperatureNotificationId = 1;
  static const _humidityNotificationId = 2;
  static const _humidityLimitScale = 100;

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
      appName: 'Climate Clock',
      appUserModelId: 'ClimateClock.App',
      guid: '4415f4a4-9186-40b3-9c40-78efd5d75de2',
    ),
    web: WebInitializationSettings(),
  );

  static const _details = NotificationDetails(
    android: AndroidNotificationDetails(
      'environment_alerts',
      'Environment alerts',
      channelDescription:
          'Alerts when temperature or humidity is outside its limits',
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
  final _temperatureAlerts = LimitAlertTracker();
  final _humidityAlerts = LimitAlertTracker();
  bool _available = false;

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
    final alert = _temperatureAlerts.update(
      value: currentTemperature,
      minimum: minTemperature,
      maximum: maxTemperature,
    );

    switch (alert) {
      case null:
        return;
      case LimitAlert.belowMinimum:
        await _show(
          id: _temperatureNotificationId,
          title: 'Temperature below minimum',
          body:
              'Current temperature is '
              '${formatTemperatureWithUnit(currentTemperature, fahrenheit: fahrenheit)}. '
              'Minimum is '
              '${formatTemperatureWithUnit(minTemperature, fahrenheit: fahrenheit)}.',
        );
      case LimitAlert.aboveMaximum:
        await _show(
          id: _temperatureNotificationId,
          title: 'Temperature above maximum',
          body:
              'Current temperature is '
              '${formatTemperatureWithUnit(currentTemperature, fahrenheit: fahrenheit)}. '
              'Maximum is '
              '${formatTemperatureWithUnit(maxTemperature, fahrenheit: fahrenheit)}.',
        );
    }
  }

  Future<void> updateHumidity({
    required int currentHumidityMilliPercent,
    required int minHumidityTenthsPercent,
    required int maxHumidityTenthsPercent,
  }) async {
    final alert = _humidityAlerts.update(
      value: currentHumidityMilliPercent,
      minimum: minHumidityTenthsPercent * _humidityLimitScale,
      maximum: maxHumidityTenthsPercent * _humidityLimitScale,
    );

    switch (alert) {
      case null:
        return;
      case LimitAlert.belowMinimum:
        await _show(
          id: _humidityNotificationId,
          title: 'Humidity below minimum',
          body:
              'Current humidity is '
              '${formatHumidityMilliPercent(currentHumidityMilliPercent)}. '
              'Minimum is '
              '${formatHumidityTenthsPercent(minHumidityTenthsPercent)}.',
        );
      case LimitAlert.aboveMaximum:
        await _show(
          id: _humidityNotificationId,
          title: 'Humidity above maximum',
          body:
              'Current humidity is '
              '${formatHumidityMilliPercent(currentHumidityMilliPercent)}. '
              'Maximum is '
              '${formatHumidityTenthsPercent(maxHumidityTenthsPercent)}.',
        );
    }
  }

  void reset() {
    _temperatureAlerts.reset();
    _humidityAlerts.reset();
  }

  Future<void> _show({
    required int id,
    required String title,
    required String body,
  }) async {
    if (!_available) return;

    await _notifications.show(
      id: id,
      title: title,
      body: body,
      notificationDetails: _details,
    );
  }
}
