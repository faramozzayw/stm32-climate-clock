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
    'N1cnJlbnRUaW1lSABSDnNldEN1cnJlbnRUaW1lQgkKB2NvbW1hbmQ=');

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
    {'1': 'value', '3': 1, '4': 1, '5': 17, '10': 'value'},
  ],
};

/// Descriptor for `SetMaxHumidity`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setMaxHumidityDescriptor = $convert
    .base64Decode('Cg5TZXRNYXhIdW1pZGl0eRIUCgV2YWx1ZRgBIAEoEVIFdmFsdWU=');

@$core.Deprecated('Use setMinHumidityDescriptor instead')
const SetMinHumidity$json = {
  '1': 'SetMinHumidity',
  '2': [
    {'1': 'value', '3': 1, '4': 1, '5': 17, '10': 'value'},
  ],
};

/// Descriptor for `SetMinHumidity`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List setMinHumidityDescriptor = $convert
    .base64Decode('Cg5TZXRNaW5IdW1pZGl0eRIUCgV2YWx1ZRgBIAEoEVIFdmFsdWU=');

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

@$core.Deprecated('Use deviceTelemetryDescriptor instead')
const DeviceTelemetry$json = {
  '1': 'DeviceTelemetry',
  '2': [
    {'1': 'current_temp', '3': 1, '4': 1, '5': 17, '10': 'currentTemp'},
  ],
};

/// Descriptor for `DeviceTelemetry`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List deviceTelemetryDescriptor = $convert.base64Decode(
    'Cg9EZXZpY2VUZWxlbWV0cnkSIQoMY3VycmVudF90ZW1wGAEgASgRUgtjdXJyZW50VGVtcA==');
