import 'package:flutter/material.dart';

import 'app.dart';
import 'services/temperature_notification_service.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  final notifications = TemperatureNotificationService();
  await notifications.initialize();

  runApp(DeviceApp(notifications: notifications));
}
