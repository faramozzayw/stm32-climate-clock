import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:permission_handler/permission_handler.dart';

import '../generated/device.pb.dart';

class DeviceBleService {
  static const deviceName = 'ESP32_BRIDGE';

  static final _serviceUuid = Guid('6E400001-B5A3-F393-E0A9-E50E24DCCA9E');
  static final _rxUuid = Guid('6E400002-B5A3-F393-E0A9-E50E24DCCA9E');

  final _connectionChanges = StreamController<bool>.broadcast();
  BluetoothDevice? _device;
  BluetoothCharacteristic? _rxCharacteristic;
  StreamSubscription<BluetoothConnectionState>? _deviceStateSubscription;

  bool get isConnected => _rxCharacteristic != null;
  Stream<bool> get connectionChanges => _connectionChanges.stream;

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

      await _deviceStateSubscription?.cancel();
      _deviceStateSubscription = foundDevice.connectionState.listen((state) {
        if (state == BluetoothConnectionState.disconnected) {
          _clearConnection();
        }
      });

      _device = foundDevice;
      _rxCharacteristic = characteristic;
      _connectionChanges.add(true);
    } catch (_) {
      if (foundDevice != null) await foundDevice.disconnect();
      rethrow;
    } finally {
      await scanSubscription?.cancel();
      if (FlutterBluePlus.isScanningNow) await FlutterBluePlus.stopScan();
    }
  }

  Future<void> disconnect() async {
    final device = _device;
    if (device == null) return;
    await device.disconnect();
    _clearConnection();
  }

  Future<void> send(DeviceCommand command) async {
    final characteristic = _rxCharacteristic;
    if (characteristic == null) {
      throw StateError('Connect to $deviceName first.');
    }

    await characteristic.write(
      command.writeToBuffer(),
      withoutResponse: characteristic.properties.writeWithoutResponse,
    );
  }

  Future<void> dispose() async {
    await _deviceStateSubscription?.cancel();
    await _device?.disconnect();
    await _connectionChanges.close();
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
    _device = null;
    _rxCharacteristic = null;
    if (wasConnected && !_connectionChanges.isClosed) {
      _connectionChanges.add(false);
    }
  }
}
