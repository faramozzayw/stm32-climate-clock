import 'package:flutter/material.dart';

import 'app.dart';
import 'services/environment_notification_service.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  final notifications = EnvironmentNotificationService();
  await notifications.initialize();

  runApp(DeviceApp(notifications: notifications));
}
