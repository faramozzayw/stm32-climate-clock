// This is a generated file - do not edit.
//
// Generated from device.proto.

// @dart = 3.3

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names
// ignore_for_file: curly_braces_in_flow_control_structures
// ignore_for_file: deprecated_member_use_from_same_package, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_relative_imports

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

class BleConnectionState extends $pb.ProtobufEnum {
  static const BleConnectionState BLE_CONNECTION_STATE_DISCONNECTED =
      BleConnectionState._(
          0, _omitEnumNames ? '' : 'BLE_CONNECTION_STATE_DISCONNECTED');
  static const BleConnectionState BLE_CONNECTION_STATE_CONNECTING =
      BleConnectionState._(
          1, _omitEnumNames ? '' : 'BLE_CONNECTION_STATE_CONNECTING');
  static const BleConnectionState BLE_CONNECTION_STATE_CONNECTED =
      BleConnectionState._(
          2, _omitEnumNames ? '' : 'BLE_CONNECTION_STATE_CONNECTED');

  static const $core.List<BleConnectionState> values = <BleConnectionState>[
    BLE_CONNECTION_STATE_DISCONNECTED,
    BLE_CONNECTION_STATE_CONNECTING,
    BLE_CONNECTION_STATE_CONNECTED,
  ];

  static final $core.List<BleConnectionState?> _byValue =
      $pb.ProtobufEnum.$_initByValueList(values, 2);
  static BleConnectionState? valueOf($core.int value) =>
      value < 0 || value >= _byValue.length ? null : _byValue[value];

  const BleConnectionState._(super.value, super.name);
}

const $core.bool _omitEnumNames =
    $core.bool.fromEnvironment('protobuf.omit_enum_names');
