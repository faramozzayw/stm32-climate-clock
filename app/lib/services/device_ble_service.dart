import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:permission_handler/permission_handler.dart';

import '../generated/device.pb.dart';

class DeviceBleService {
  static const deviceName = 'ClimateClock';

  static final _serviceUuid = Guid('6E400001-B5A3-F393-E0A9-E50E24DCCA9E');
  static final _rxUuid = Guid('6E400002-B5A3-F393-E0A9-E50E24DCCA9E');
  static final _txUuid = Guid('6E400003-B5A3-F393-E0A9-E50E24DCCA9E');

  final _connectionChanges = StreamController<bool>.broadcast();
  final _telemetry = StreamController<DeviceTelemetry>.broadcast();
  BluetoothDevice? _device;
  BluetoothCharacteristic? _rxCharacteristic;
  BluetoothCharacteristic? _txCharacteristic;
  StreamSubscription<BluetoothConnectionState>? _deviceStateSubscription;
  StreamSubscription<List<int>>? _telemetrySubscription;
  Completer<void>? _disconnectCompleter;
  Future<void>? _disconnectOperation;

  bool get isConnected => _rxCharacteristic != null;
  Stream<bool> get connectionChanges => _connectionChanges.stream;
  Stream<DeviceTelemetry> get telemetry => _telemetry.stream;

  Future<void> connect({void Function(String status)? onStatus}) async {
    await _requestPermissions();

    StreamSubscription<List<ScanResult>>? scanSubscription;
    BluetoothDevice? foundDevice;
    try {
      onStatus?.call('Scanning for $deviceName…');
      final completer = Completer<BluetoothDevice>();
      scanSubscription = FlutterBluePlus.onScanResults.listen((results) {
        for (final result in results) {
          if (result.advertisementData.advName == deviceName &&
              !completer.isCompleted) {
            completer.complete(result.device);
          }
        }
      });

      await FlutterBluePlus.startScan(
        withServices: [_serviceUuid],
        timeout: const Duration(seconds: 10),
      );
      foundDevice = await completer.future.timeout(
        const Duration(seconds: 11),
        onTimeout: () => throw TimeoutException('$deviceName was not found.'),
      );
      await FlutterBluePlus.stopScan();

      onStatus?.call('Connecting…');
      await foundDevice.connect(
        license: License.nonprofit,
        timeout: const Duration(seconds: 10),
      );
      final services = await foundDevice.discoverServices();
      final service = services.where((item) => item.uuid == _serviceUuid).first;
      final characteristic = service.characteristics
          .where((item) => item.uuid == _rxUuid)
          .first;
      final txCharacteristic = service.characteristics
          .where((item) => item.uuid == _txUuid)
          .first;

      await _telemetrySubscription?.cancel();
      _telemetrySubscription = txCharacteristic.onValueReceived.listen(
        _decodeTelemetry,
      );
      await txCharacteristic.setNotifyValue(true);

      await _deviceStateSubscription?.cancel();
      _deviceStateSubscription = foundDevice.connectionState.listen((state) {
        if (state == BluetoothConnectionState.disconnected) {
          final completer = _disconnectCompleter;
          if (completer != null && !completer.isCompleted) {
            completer.complete();
          } else {
            _clearConnection();
          }
        }
      });

      _device = foundDevice;
      _rxCharacteristic = characteristic;
      _txCharacteristic = txCharacteristic;
      _connectionChanges.add(true);
    } catch (_) {
      await _telemetrySubscription?.cancel();
      _telemetrySubscription = null;
      if (foundDevice != null) await foundDevice.disconnect();
      rethrow;
    } finally {
      await scanSubscription?.cancel();
      if (FlutterBluePlus.isScanningNow) await FlutterBluePlus.stopScan();
    }
  }

  Future<void> disconnect() {
    final pendingOperation = _disconnectOperation;
    if (pendingOperation != null) return pendingOperation;

    final device = _device;
    if (device == null) return Future.value();

    final operation = _disconnectDevice(device);
    _disconnectOperation = operation;
    return operation.whenComplete(() {
      if (identical(_disconnectOperation, operation)) {
        _disconnectOperation = null;
      }
    });
  }

  Future<void> _disconnectDevice(BluetoothDevice device) async {
    final linkDisconnected = Completer<void>();
    _disconnectCompleter = linkDisconnected;

    try {
      await _signalDisconnecting();
      await device.disconnect();
      await _waitForBleLinkToClose(linkDisconnected);

      try {
        await _waitForBridgeToBecomeAvailable(device);
      } finally {
        _clearConnection();
      }
    } finally {
      if (identical(_disconnectCompleter, linkDisconnected)) {
        _disconnectCompleter = null;
      }
    }
  }

  Future<void> _signalDisconnecting() async {
    final txCharacteristic = _txCharacteristic;
    if (txCharacteristic == null) return;

    try {
      // The ESP32 observes this change and reports DISCONNECTING to the STM32.
      await txCharacteristic.setNotifyValue(false);
    } catch (_) {
      // The physical link may already be gone.
    }
  }

  Future<void> _waitForBleLinkToClose(Completer<void> linkDisconnected) async {
    await linkDisconnected.future.timeout(
      const Duration(seconds: 10),
      onTimeout: () =>
          throw TimeoutException('$deviceName did not finish disconnecting.'),
    );
  }

  Future<void> _waitForBridgeToBecomeAvailable(BluetoothDevice device) async {
    final advertising = Completer<void>();
    StreamSubscription<List<ScanResult>>? scanSubscription;

    try {
      scanSubscription = FlutterBluePlus.onScanResults.listen((results) {
        final foundDevice = results.any(
          (result) => result.device.remoteId == device.remoteId,
        );
        if (foundDevice && !advertising.isCompleted) {
          advertising.complete();
        }
      });

      await FlutterBluePlus.startScan(
        withServices: [_serviceUuid],
        timeout: const Duration(seconds: 10),
      );

      // Advertising resumes after the ESP32 reports DISCONNECTED to the STM32.
      await advertising.future.timeout(
        const Duration(seconds: 11),
        onTimeout: () =>
            throw TimeoutException('$deviceName did not resume advertising.'),
      );
    } finally {
      await scanSubscription?.cancel();
      if (FlutterBluePlus.isScanningNow) {
        await FlutterBluePlus.stopScan();
      }
    }
  }

  Future<void> send(DeviceCommand command) async {
    final characteristic = _rxCharacteristic;
    if (characteristic == null) {
      throw StateError('Connect to $deviceName first.');
    }

    await characteristic.write(
      DeviceMessage(command: command).writeToBuffer(),
      withoutResponse: characteristic.properties.writeWithoutResponse,
    );
  }

  Future<void> dispose() async {
    await _telemetrySubscription?.cancel();
    await disconnect();
    await _deviceStateSubscription?.cancel();
    await _connectionChanges.close();
    await _telemetry.close();
  }

  Future<void> _requestPermissions() async {
    if (defaultTargetPlatform != TargetPlatform.android) return;

    final permissions = await [
      Permission.bluetoothScan,
      Permission.bluetoothConnect,
      Permission.locationWhenInUse,
    ].request();
    if (permissions[Permission.bluetoothScan]?.isPermanentlyDenied ?? false) {
      throw StateError('Bluetooth permission is permanently denied.');
    }
  }

  void _clearConnection() {
    final wasConnected = isConnected;
    unawaited(_telemetrySubscription?.cancel());
    _telemetrySubscription = null;
    _device = null;
    _rxCharacteristic = null;
    _txCharacteristic = null;
    if (wasConnected && !_connectionChanges.isClosed) {
      _connectionChanges.add(false);
    }
  }

  void _decodeTelemetry(List<int> value) {
    try {
      final message = DeviceMessage.fromBuffer(value);
      if (!message.hasTelemetry()) {
        throw const FormatException('Expected DeviceTelemetry payload.');
      }
      if (!_telemetry.isClosed) {
        _telemetry.add(message.telemetry);
      }
    } catch (error, stackTrace) {
      if (!_telemetry.isClosed) {
        _telemetry.addError(error, stackTrace);
      }
    }
  }
}
