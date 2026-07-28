import 'dart:async';

import 'package:fixnum/fixnum.dart';
import 'package:flutter/material.dart';

import '../generated/device.pb.dart';
import '../services/device_ble_service.dart';
import '../services/environment_notification_service.dart';
import '../utils/humidity.dart';
import '../utils/temperature.dart';
import '../widgets/connection_card.dart';
import '../widgets/current_conditions_card.dart';
import '../widgets/limit_command.dart';
import '../widgets/page_header.dart';
import '../widgets/time_sync_card.dart';

class DevicePage extends StatefulWidget {
  const DevicePage({required this.notifications, super.key});

  final EnvironmentNotificationService notifications;

  @override
  State<DevicePage> createState() => _DevicePageState();
}

class _DevicePageState extends State<DevicePage> {
  final _maxController = TextEditingController(text: '25.0');
  final _minController = TextEditingController(text: '15.5');
  final _maxHumidityController = TextEditingController(text: '60.0');
  final _minHumidityController = TextEditingController(text: '30.0');
  final _ble = DeviceBleService();

  StreamSubscription<bool>? _connectionSubscription;
  StreamSubscription<DeviceTelemetry>? _telemetrySubscription;
  bool _busy = false;
  bool _fahrenheit = false;
  String _status = 'Disconnected';
  int? _currentTemperature;
  int? _currentHumidityMilliPercent;
  int? _deviceMinTemperature;
  int? _deviceMaxTemperature;
  int? _deviceMinHumidityTenthsPercent;
  int? _deviceMaxHumidityTenthsPercent;

  bool get _connected => _ble.isConnected;

  @override
  void initState() {
    super.initState();
    _connectionSubscription = _ble.connectionChanges.listen((connected) {
      if (!connected && mounted) {
        setState(() {
          _status = 'Disconnected';
          _currentTemperature = null;
          _currentHumidityMilliPercent = null;
          _deviceMinTemperature = null;
          _deviceMaxTemperature = null;
          _deviceMinHumidityTenthsPercent = null;
          _deviceMaxHumidityTenthsPercent = null;
        });
        widget.notifications.reset();
      }
    });
    _telemetrySubscription = _ble.telemetry.listen(
      _applyTelemetry,
      onError: (Object error) => _showMessage(_friendlyError(error)),
    );
  }

  @override
  void dispose() {
    _connectionSubscription?.cancel();
    _telemetrySubscription?.cancel();
    unawaited(_ble.dispose());
    _maxController.dispose();
    _minController.dispose();
    _maxHumidityController.dispose();
    _minHumidityController.dispose();
    super.dispose();
  }

  Future<void> _connect() async {
    if (_busy) return;
    setState(() => _busy = true);

    try {
      await widget.notifications.requestPermission();
      await _ble.connect(
        onStatus: (status) {
          if (mounted) setState(() => _status = status);
        },
      );
      if (mounted) {
        setState(() => _status = 'Connected to ${DeviceBleService.deviceName}');
      }
      await _sendTemperatureUnit(announce: false);
    } catch (error) {
      if (mounted) setState(() => _status = _friendlyError(error));
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  Future<void> _disconnect() async {
    if (_busy) return;
    setState(() {
      _busy = true;
      _status = 'Disconnecting…';
    });

    try {
      await _ble.disconnect();
    } catch (error) {
      if (mounted) setState(() => _status = _friendlyError(error));
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  Future<void> _sendTemperature({required bool maximum}) async {
    final controller = maximum ? _maxController : _minController;
    final tenths = _parseTemperature(controller.text);
    if (tenths == null) {
      _showMessage('Enter a valid temperature with one decimal place.');
      return;
    }

    final command = maximum
        ? DeviceCommand(setMaxTemp: SetMaxTemp(value: tenths))
        : DeviceCommand(setMinTemp: SetMinTemp(value: tenths));
    await _send(
      command,
      maximum ? 'Maximum temperature sent' : 'Minimum temperature sent',
    );
  }

  Future<void> _sendCurrentTime() async {
    final now = DateTime.now();
    final deviceTimeMilliseconds =
        now.millisecondsSinceEpoch + now.timeZoneOffset.inMilliseconds;
    final command = DeviceCommand(
      setCurrentTime: SetCurrentTime(valueMs: Int64(deviceTimeMilliseconds)),
    );
    await _send(command, 'Current device time sent');
  }

  Future<void> _sendHumidity({required bool maximum}) async {
    final controller = maximum
        ? _maxHumidityController
        : _minHumidityController;
    final tenthsPercent = _parseHumidity(controller.text);
    if (tenthsPercent == null) {
      _showMessage('Enter humidity from 0.0% to 100.0%.');
      return;
    }

    final command = maximum
        ? DeviceCommand(setMaxHumidity: SetMaxHumidity(value: tenthsPercent))
        : DeviceCommand(setMinHumidity: SetMinHumidity(value: tenthsPercent));
    await _send(
      command,
      maximum ? 'Maximum humidity sent' : 'Minimum humidity sent',
    );
  }

  Future<void> _sendTemperatureUnit({bool announce = true}) async {
    final command = DeviceCommand(
      setTemperatureUnit: SetTemperatureUnit(
        unit: _fahrenheit
            ? TemperatureUnit.TEMPERATURE_UNIT_FAHRENHEIT
            : TemperatureUnit.TEMPERATURE_UNIT_CELSIUS,
      ),
    );
    await _send(command, announce ? 'Temperature unit changed' : null);
  }

  Future<void> _send(DeviceCommand command, String? successMessage) async {
    try {
      await _ble.send(command);
      if (successMessage != null) {
        _showMessage(successMessage);
      }
    } catch (error) {
      _showMessage(_friendlyError(error));
    }
  }

  int? _parseTemperature(String text) {
    final displayedTenths = _parseDisplayedTenths(text);
    if (displayedTenths == null) return null;

    final celsiusTenths = _toCelsius(displayedTenths, _fahrenheit);
    return celsiusTenths >= -32768 && celsiusTenths <= 32767
        ? celsiusTenths
        : null;
  }

  int? _parseHumidity(String text) {
    final humidityTenthsPercent = _parseDisplayedTenths(text);
    if (humidityTenthsPercent == null) return null;
    return humidityTenthsPercent >= 0 && humidityTenthsPercent <= 1000
        ? humidityTenthsPercent
        : null;
  }

  int? _parseDisplayedTenths(String text) {
    if (!RegExp(r'^-?\d+(?:\.\d)?$').hasMatch(text.trim())) return null;
    final value = double.tryParse(text.trim());
    if (value == null) return null;
    return (value * 10).round();
  }

  void _changeTemperatureUnit(bool fahrenheit) {
    if (_fahrenheit == fahrenheit) return;

    _convertController(_minController, _fahrenheit, fahrenheit);
    _convertController(_maxController, _fahrenheit, fahrenheit);
    setState(() => _fahrenheit = fahrenheit);

    if (_connected) {
      unawaited(_sendTemperatureUnit());
    }
  }

  void _convertController(
    TextEditingController controller,
    bool fromFahrenheit,
    bool toFahrenheit,
  ) {
    final displayedTenths = _parseDisplayedTenths(controller.text);
    if (displayedTenths == null) return;

    final celsiusTenths = _toCelsius(displayedTenths, fromFahrenheit);
    final converted = _fromCelsius(celsiusTenths, toFahrenheit);
    controller.text = formatTemperatureTenths(converted);
  }

  int _toCelsius(int temperatureTenths, bool fahrenheit) {
    if (!fahrenheit) return temperatureTenths;
    return fahrenheitToCelsiusTenths(temperatureTenths);
  }

  int _fromCelsius(int celsiusTenths, bool fahrenheit) {
    if (!fahrenheit) return celsiusTenths;
    return celsiusToFahrenheitTenths(celsiusTenths);
  }

  void _applyTelemetry(DeviceTelemetry telemetry) {
    if (!mounted) return;

    switch (telemetry.whichData()) {
      case DeviceTelemetry_Data.measurement:
        _applyMeasurement(telemetry.measurement);
      case DeviceTelemetry_Data.limits:
        _applyLimits(telemetry.limits);
      case DeviceTelemetry_Data.notSet:
        return;
    }
  }

  void _applyMeasurement(EnvironmentMeasurement measurement) {
    final currentHumidity = measurement.hasHumidityMilliPercent()
        ? measurement.humidityMilliPercent
        : null;

    setState(() {
      _currentTemperature = measurement.temperatureTenthsCelsius;
      _currentHumidityMilliPercent = currentHumidity;
    });

    final minTemperature = _deviceMinTemperature;
    final maxTemperature = _deviceMaxTemperature;
    if (minTemperature != null && maxTemperature != null) {
      unawaited(
        widget.notifications
            .updateTemperature(
              currentTemperature: measurement.temperatureTenthsCelsius,
              minTemperature: minTemperature,
              maxTemperature: maxTemperature,
              fahrenheit: _fahrenheit,
            )
            .catchError(
              (Object error) =>
                  debugPrint('Temperature notification failed: $error'),
            ),
      );
    }

    final minHumidity = _deviceMinHumidityTenthsPercent;
    final maxHumidity = _deviceMaxHumidityTenthsPercent;
    if (currentHumidity != null && minHumidity != null && maxHumidity != null) {
      unawaited(
        widget.notifications
            .updateHumidity(
              currentHumidityMilliPercent: currentHumidity,
              minHumidityTenthsPercent: minHumidity,
              maxHumidityTenthsPercent: maxHumidity,
            )
            .catchError(
              (Object error) =>
                  debugPrint('Humidity notification failed: $error'),
            ),
      );
    }
  }

  void _applyLimits(EnvironmentLimits limits) {
    setState(() {
      _deviceMinTemperature = limits.minTemperatureTenthsCelsius;
      _deviceMaxTemperature = limits.maxTemperatureTenthsCelsius;
      _deviceMinHumidityTenthsPercent = limits.minHumidityTenthsPercent;
      _deviceMaxHumidityTenthsPercent = limits.maxHumidityTenthsPercent;
    });
  }

  String _friendlyError(Object error) {
    final message = error.toString().replaceFirst(
      RegExp(r'^\w+(?:Exception)?: '),
      '',
    );
    return message.isEmpty ? 'Bluetooth operation failed.' : message;
  }

  void _showMessage(String message) {
    if (!mounted) return;
    ScaffoldMessenger.of(context)
      ..hideCurrentSnackBar()
      ..showSnackBar(SnackBar(content: Text(message)));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [Color(0xFFF5F0FF), Color(0xFFF0FAF8)],
          ),
        ),
        child: SafeArea(
          child: ListView(
            padding: const EdgeInsets.fromLTRB(20, 24, 20, 32),
            children: [
              PageHeader(
                fahrenheit: _fahrenheit,
                onTemperatureUnitChanged: _changeTemperatureUnit,
              ),
              const SizedBox(height: 24),
              ConnectionCard(
                connected: _connected,
                busy: _busy,
                status: _status,
                onPressed: _busy ? null : (_connected ? _disconnect : _connect),
              ),
              const SizedBox(height: 14),
              CurrentConditionsCard(
                temperatureTenths: _currentTemperature,
                humidityMilliPercent: _currentHumidityMilliPercent,
                fahrenheit: _fahrenheit,
              ),
              const SizedBox(height: 26),
              Text(
                'Temperature range',
                style: Theme.of(context).textTheme.titleMedium,
              ),
              const SizedBox(height: 12),
              LimitCommand(
                label: 'Maximum temperature',
                hint: 'Upper comfort limit',
                icon: Icons.local_fire_department_rounded,
                accent: const Color(0xFFFF756C),
                surface: const Color(0xFFFFF0EE),
                controller: _maxController,
                currentLimitText: _deviceMaxTemperature == null
                    ? '--.- °${_fahrenheit ? 'F' : 'C'}'
                    : formatTemperatureWithUnit(
                        _deviceMaxTemperature!,
                        fahrenheit: _fahrenheit,
                      ),
                suffix: '°${_fahrenheit ? 'F' : 'C'}',
                allowSigned: true,
                onSend: _connected
                    ? () => _sendTemperature(maximum: true)
                    : null,
              ),
              const SizedBox(height: 14),
              LimitCommand(
                label: 'Minimum temperature',
                hint: 'Lower comfort limit',
                icon: Icons.ac_unit_rounded,
                accent: const Color(0xFF5799E5),
                surface: const Color(0xFFECF5FF),
                controller: _minController,
                currentLimitText: _deviceMinTemperature == null
                    ? '--.- °${_fahrenheit ? 'F' : 'C'}'
                    : formatTemperatureWithUnit(
                        _deviceMinTemperature!,
                        fahrenheit: _fahrenheit,
                      ),
                suffix: '°${_fahrenheit ? 'F' : 'C'}',
                allowSigned: true,
                onSend: _connected
                    ? () => _sendTemperature(maximum: false)
                    : null,
              ),
              const SizedBox(height: 18),
              Text(
                'Humidity range',
                style: Theme.of(context).textTheme.titleMedium,
              ),
              const SizedBox(height: 12),
              LimitCommand(
                label: 'Maximum humidity',
                hint: 'Upper comfort limit',
                icon: Icons.water_drop_rounded,
                accent: const Color(0xFF388FA8),
                surface: const Color(0xFFEAF8FB),
                controller: _maxHumidityController,
                currentLimitText: _deviceMaxHumidityTenthsPercent == null
                    ? '--.-%'
                    : formatHumidityTenthsPercent(
                        _deviceMaxHumidityTenthsPercent!,
                      ),
                suffix: '%',
                allowSigned: false,
                onSend: _connected ? () => _sendHumidity(maximum: true) : null,
              ),
              const SizedBox(height: 14),
              LimitCommand(
                label: 'Minimum humidity',
                hint: 'Lower comfort limit',
                icon: Icons.water_drop_outlined,
                accent: const Color(0xFF597CBF),
                surface: const Color(0xFFEEF2FC),
                controller: _minHumidityController,
                currentLimitText: _deviceMinHumidityTenthsPercent == null
                    ? '--.-%'
                    : formatHumidityTenthsPercent(
                        _deviceMinHumidityTenthsPercent!,
                      ),
                suffix: '%',
                allowSigned: false,
                onSend: _connected ? () => _sendHumidity(maximum: false) : null,
              ),
              const SizedBox(height: 18),
              TimeSyncCard(enabled: _connected, onPressed: _sendCurrentTime),
            ],
          ),
        ),
      ),
    );
  }
}
