// This is a generated file - do not edit.
//
// Generated from device.proto.

// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names
// ignore_for_file: curly_braces_in_flow_control_structures
// ignore_for_file: deprecated_member_use_from_same_package, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_relative_imports
// ignore_for_file: unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use temperatureUnitDescriptor instead')
const TemperatureUnit$json = {
  '1': 'TemperatureUnit',
  '2': [
    {'1': 'TEMPERATURE_UNIT_CELSIUS', '2': 0},
    {'1': 'TEMPERATURE_UNIT_FAHRENHEIT', '2': 1},
  ],
};

/// Descriptor for `TemperatureUnit`. Decode as a `google.protobuf.EnumDescriptorProto`.
final $typed_data.Uint8List temperatureUnitDescriptor = $convert.base64Decode(
    'Cg9UZW1wZXJhdHVyZVVuaXQSHAoYVEVNUEVSQVRVUkVfVU5JVF9DRUxTSVVTEAASHwobVEVNUE'
    'VSQVRVUkVfVU5JVF9GQUhSRU5IRUlUEAE=');

@$core.Deprecated('Use bleConnectionStateDescriptor instead')
const BleConnectionState$json = {
  '1': 'BleConnectionState',
  '2': [
    {'1': 'BLE_CONNECTION_STATE_DISCONNECTED', '2': 0},
    {'1': 'BLE_CONNECTION_STATE_CONNECTING', '2': 1},
    {'1': 'BLE_CONNECTION_STATE_CONNECTED', '2': 2},
    {'1': 'BLE_CONNECTION_STATE_DISCONNECTING', '2': 3},
  ],
};

/// Descriptor for `BleConnectionState`. Decode as a `google.protobuf.EnumDescriptorProto`.
final $typed_data.Uint8List bleConnectionStateDescriptor = $convert.base64Decode(
    'ChJCbGVDb25uZWN0aW9uU3RhdGUSJQohQkxFX0NPTk5FQ1RJT05fU1RBVEVfRElTQ09OTkVDVE'
    'VEEAASIwofQkxFX0NPTk5FQ1RJT05fU1RBVEVfQ09OTkVDVElORxABEiIKHkJMRV9DT05ORUNU'
    'SU9OX1NUQVRFX0NPTk5FQ1RFRBACEiYKIkJMRV9DT05ORUNUSU9OX1NUQVRFX0RJU0NPTk5FQ1'
    'RJTkcQAw==');

@$core.Deprecated('Use deviceMessageDescriptor instead')
const DeviceMessage$json = {
  '1': 'DeviceMessage',
  '2': [
    {
      '1': 'command',
      '3': 1,
      '4': 1,
      '5': 11,
      '6': '.device.DeviceCommand',
      '9': 0,
      '10': 'command'
    },
    {
      '1': 'telemetry',
      '3': 2,
      '4': 1,
      '5': 11,
      '6': '.device.DeviceTelemetry',
      '9': 0,
      '10': 'telemetry'
    },
    {
      '1': 'bridge_status',
      '3': 3,
      '4': 1,
      '5': 11,
      '6': '.device.BridgeStatus',
      '9': 0,
      '10': 'bridgeStatus'
    },
  ],
  '8': [
    {'1': 'payload'},
  ],
};

/// Descriptor for `DeviceMessage`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List deviceMessageDescriptor = $convert.base64Decode(
    'Cg1EZXZpY2VNZXNzYWdlEjEKB2NvbW1hbmQYASABKAsyFS5kZXZpY2UuRGV2aWNlQ29tbWFuZE'
    'gAUgdjb21tYW5kEjcKCXRlbGVtZXRyeRgCIAEoCzIXLmRldmljZS5EZXZpY2VUZWxlbWV0cnlI'
    'AFIJdGVsZW1ldHJ5EjsKDWJyaWRnZV9zdGF0dXMYAyABKAsyFC5kZXZpY2UuQnJpZGdlU3RhdH'
    'VzSABSDGJyaWRnZVN0YXR1c0IJCgdwYXlsb2Fk');

@$core.Deprecated('Use deviceCommandDescriptor instead')
const DeviceCommand$json = {
  '1': 'DeviceCommand',
  '2': [
    {
      '1': 'set_max_temp',
      '3': 1,
      '4': 1,
      '5': 11,
      '6': '.device.SetMaxTemp',
      '9': 0,
      '10': 'setMaxTemp'
    },
    {
      '1': 'set_min_temp',
      '3': 2,
      '4': 1,
      '5': 11,
      '6': '.device.SetMinTemp',
      '9': 0,
      '10': 'setMinTemp'
    },
    {
      '1': 'set_current_time',
      '3': 3,
      '4': 1,
      '5': 11,
      '6': '.device.SetCurrentTime',
      '9': 0,
      '10': 'setCurrentTime'
    },
    {
      '1': 'set_temperature_unit',
      '3': 4,
      '4': 1,
      '5': 11,
      '6': '.device.SetTemperatureUnit',
      '9': 0,
      '10': 'setTemperatureUnit'
    },
    {
      '1': 'set_max_humidity',
      '3': 5,
      '4': 1,
      '5': 11,
      '6': '.device.SetMaxHumidity',
      '9': 0,
      '10': 'setMaxHumidity'
    },
    {
      '1': 'set_min_humidity',
      '3': 6,
      '4': 1,
      '5': 11,
      '6': '.device.SetMinHumidity',
      '9': 0,
      '10': 'setMinHumidity'
    },
  ],
  '8': [
    {'1': 'command'},
  ],
};

/// Descriptor for `DeviceCommand`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List deviceCommandDescriptor = $convert.base64Decode(
    'Cg1EZXZpY2VDb21tYW5kEjYKDHNldF9tYXhfdGVtcBgBIAEoCzISLmRldmljZS5TZXRNYXhUZW'
    '1wSABSCnNldE1heFRlbXASNgoMc2V0X21pbl90ZW1wGAIgASgLMhIuZGV2aWNlLlNldE1pblRl'
    'bXBIAFIKc2V0TWluVGVtcBJCChBzZXRfY3VycmVudF90aW1lGAMgASgLMhYuZGV2aWNlLlNldE'
    'N1cnJlbnRUaW1lSABSDnNldEN1cnJlbnRUaW1lEk4KFHNldF90ZW1wZXJhdHVyZV91bml0GAQg'
    'ASgLMhouZGV2aWNlLlNldFRlbXBlcmF0dXJlVW5pdEgAUhJzZXRUZW1wZXJhdHVyZVVuaXQSQg'
    'oQc2V0X21heF9odW1pZGl0eRgFIAEoCzIWLmRldmljZS5TZXRNYXhIdW1pZGl0eUgAUg5zZXRN'
    'YXhIdW1pZGl0eRJCChBzZXRfbWluX2h1bWlkaXR5GAYgASgLMhYuZGV2aWNlLlNldE1pbkh1bW'
    'lkaXR5SABSDnNldE1pbkh1bWlkaXR5QgkKB2NvbW1hbmQ=');

@$core.Deprecated('Use setMaxTempDescriptor instead')
const SetMaxTemp$json = {
  '1': 'SetMaxTemp',
  '2': [
    {'1': 'value', '3': 1, '4': 1, '5': 17, '10': 'value'},
  ],
};

/// Descriptor for `SetMaxTemp`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setMaxTempDescriptor =
    $convert.base64Decode('CgpTZXRNYXhUZW1wEhQKBXZhbHVlGAEgASgRUgV2YWx1ZQ==');

@$core.Deprecated('Use setMinTempDescriptor instead')
const SetMinTemp$json = {
  '1': 'SetMinTemp',
  '2': [
    {'1': 'value', '3': 1, '4': 1, '5': 17, '10': 'value'},
  ],
};

/// Descriptor for `SetMinTemp`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setMinTempDescriptor =
    $convert.base64Decode('CgpTZXRNaW5UZW1wEhQKBXZhbHVlGAEgASgRUgV2YWx1ZQ==');

@$core.Deprecated('Use setMaxHumidityDescriptor instead')
const SetMaxHumidity$json = {
  '1': 'SetMaxHumidity',
  '2': [
    {'1': 'value', '3': 1, '4': 1, '5': 13, '10': 'value'},
  ],
};

/// Descriptor for `SetMaxHumidity`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setMaxHumidityDescriptor = $convert
    .base64Decode('Cg5TZXRNYXhIdW1pZGl0eRIUCgV2YWx1ZRgBIAEoDVIFdmFsdWU=');

@$core.Deprecated('Use setMinHumidityDescriptor instead')
const SetMinHumidity$json = {
  '1': 'SetMinHumidity',
  '2': [
    {'1': 'value', '3': 1, '4': 1, '5': 13, '10': 'value'},
  ],
};

/// Descriptor for `SetMinHumidity`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setMinHumidityDescriptor = $convert
    .base64Decode('Cg5TZXRNaW5IdW1pZGl0eRIUCgV2YWx1ZRgBIAEoDVIFdmFsdWU=');

@$core.Deprecated('Use setCurrentTimeDescriptor instead')
const SetCurrentTime$json = {
  '1': 'SetCurrentTime',
  '2': [
    {'1': 'value_ms', '3': 1, '4': 1, '5': 4, '10': 'valueMs'},
  ],
};

/// Descriptor for `SetCurrentTime`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setCurrentTimeDescriptor = $convert.base64Decode(
    'Cg5TZXRDdXJyZW50VGltZRIZCgh2YWx1ZV9tcxgBIAEoBFIHdmFsdWVNcw==');

@$core.Deprecated('Use setTemperatureUnitDescriptor instead')
const SetTemperatureUnit$json = {
  '1': 'SetTemperatureUnit',
  '2': [
    {
      '1': 'unit',
      '3': 1,
      '4': 1,
      '5': 14,
      '6': '.device.TemperatureUnit',
      '10': 'unit'
    },
  ],
};

/// Descriptor for `SetTemperatureUnit`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setTemperatureUnitDescriptor = $convert.base64Decode(
    'ChJTZXRUZW1wZXJhdHVyZVVuaXQSKwoEdW5pdBgBIAEoDjIXLmRldmljZS5UZW1wZXJhdHVyZV'
    'VuaXRSBHVuaXQ=');

@$core.Deprecated('Use deviceTelemetryDescriptor instead')
const DeviceTelemetry$json = {
  '1': 'DeviceTelemetry',
  '2': [
    {
      '1': 'measurement',
      '3': 1,
      '4': 1,
      '5': 11,
      '6': '.device.EnvironmentMeasurement',
      '9': 0,
      '10': 'measurement'
    },
    {
      '1': 'limits',
      '3': 2,
      '4': 1,
      '5': 11,
      '6': '.device.EnvironmentLimits',
      '9': 0,
      '10': 'limits'
    },
  ],
  '8': [
    {'1': 'data'},
  ],
};

/// Descriptor for `DeviceTelemetry`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List deviceTelemetryDescriptor = $convert.base64Decode(
    'Cg9EZXZpY2VUZWxlbWV0cnkSQgoLbWVhc3VyZW1lbnQYASABKAsyHi5kZXZpY2UuRW52aXJvbm'
    '1lbnRNZWFzdXJlbWVudEgAUgttZWFzdXJlbWVudBIzCgZsaW1pdHMYAiABKAsyGS5kZXZpY2Uu'
    'RW52aXJvbm1lbnRMaW1pdHNIAFIGbGltaXRzQgYKBGRhdGE=');

@$core.Deprecated('Use environmentMeasurementDescriptor instead')
const EnvironmentMeasurement$json = {
  '1': 'EnvironmentMeasurement',
  '2': [
    {
      '1': 'temperature_tenths_celsius',
      '3': 1,
      '4': 1,
      '5': 17,
      '10': 'temperatureTenthsCelsius'
    },
    {
      '1': 'humidity_milli_percent',
      '3': 2,
      '4': 1,
      '5': 13,
      '9': 0,
      '10': 'humidityMilliPercent',
      '17': true
    },
  ],
  '8': [
    {'1': '_humidity_milli_percent'},
  ],
};

/// Descriptor for `EnvironmentMeasurement`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List environmentMeasurementDescriptor = $convert.base64Decode(
    'ChZFbnZpcm9ubWVudE1lYXN1cmVtZW50EjwKGnRlbXBlcmF0dXJlX3RlbnRoc19jZWxzaXVzGA'
    'EgASgRUhh0ZW1wZXJhdHVyZVRlbnRoc0NlbHNpdXMSOQoWaHVtaWRpdHlfbWlsbGlfcGVyY2Vu'
    'dBgCIAEoDUgAUhRodW1pZGl0eU1pbGxpUGVyY2VudIgBAUIZChdfaHVtaWRpdHlfbWlsbGlfcG'
    'VyY2VudA==');

@$core.Deprecated('Use environmentLimitsDescriptor instead')
const EnvironmentLimits$json = {
  '1': 'EnvironmentLimits',
  '2': [
    {
      '1': 'min_temperature_tenths_celsius',
      '3': 1,
      '4': 1,
      '5': 17,
      '10': 'minTemperatureTenthsCelsius'
    },
    {
      '1': 'max_temperature_tenths_celsius',
      '3': 2,
      '4': 1,
      '5': 17,
      '10': 'maxTemperatureTenthsCelsius'
    },
    {
      '1': 'min_humidity_tenths_percent',
      '3': 3,
      '4': 1,
      '5': 13,
      '10': 'minHumidityTenthsPercent'
    },
    {
      '1': 'max_humidity_tenths_percent',
      '3': 4,
      '4': 1,
      '5': 13,
      '10': 'maxHumidityTenthsPercent'
    },
  ],
};

/// Descriptor for `EnvironmentLimits`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List environmentLimitsDescriptor = $convert.base64Decode(
    'ChFFbnZpcm9ubWVudExpbWl0cxJDCh5taW5fdGVtcGVyYXR1cmVfdGVudGhzX2NlbHNpdXMYAS'
    'ABKBFSG21pblRlbXBlcmF0dXJlVGVudGhzQ2Vsc2l1cxJDCh5tYXhfdGVtcGVyYXR1cmVfdGVu'
    'dGhzX2NlbHNpdXMYAiABKBFSG21heFRlbXBlcmF0dXJlVGVudGhzQ2Vsc2l1cxI9ChttaW5faH'
    'VtaWRpdHlfdGVudGhzX3BlcmNlbnQYAyABKA1SGG1pbkh1bWlkaXR5VGVudGhzUGVyY2VudBI9'
    'ChttYXhfaHVtaWRpdHlfdGVudGhzX3BlcmNlbnQYBCABKA1SGG1heEh1bWlkaXR5VGVudGhzUG'
    'VyY2VudA==');

@$core.Deprecated('Use bridgeStatusDescriptor instead')
const BridgeStatus$json = {
  '1': 'BridgeStatus',
  '2': [
    {
      '1': 'ble_connection_state',
      '3': 1,
      '4': 1,
      '5': 14,
      '6': '.device.BleConnectionState',
      '10': 'bleConnectionState'
    },
  ],
};

/// Descriptor for `BridgeStatus`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List bridgeStatusDescriptor = $convert.base64Decode(
    'CgxCcmlkZ2VTdGF0dXMSTAoUYmxlX2Nvbm5lY3Rpb25fc3RhdGUYASABKA4yGi5kZXZpY2UuQm'
    'xlQ29ubmVjdGlvblN0YXRlUhJibGVDb25uZWN0aW9uU3RhdGU=');
