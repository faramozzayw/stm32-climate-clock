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

import 'package:fixnum/fixnum.dart' as $fixnum;
import 'package:protobuf/protobuf.dart' as $pb;

import 'device.pbenum.dart';

export 'package:protobuf/protobuf.dart' show GeneratedMessageGenericExtensions;

export 'device.pbenum.dart';

enum DeviceMessage_Payload { command, telemetry, bridgeStatus, notSet }

/// Envelope carried by every BLE and framed UART payload.
class DeviceMessage extends $pb.GeneratedMessage {
  factory DeviceMessage({
    DeviceCommand? command,
    DeviceTelemetry? telemetry,
    BridgeStatus? bridgeStatus,
  }) {
    final result = create();
    if (command != null) result.command = command;
    if (telemetry != null) result.telemetry = telemetry;
    if (bridgeStatus != null) result.bridgeStatus = bridgeStatus;
    return result;
  }

  DeviceMessage._();

  factory DeviceMessage.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory DeviceMessage.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static const $core.Map<$core.int, DeviceMessage_Payload>
      _DeviceMessage_PayloadByTag = {
    1: DeviceMessage_Payload.command,
    2: DeviceMessage_Payload.telemetry,
    3: DeviceMessage_Payload.bridgeStatus,
    0: DeviceMessage_Payload.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'DeviceMessage',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..oo(0, [1, 2, 3])
    ..aOM<DeviceCommand>(1, _omitFieldNames ? '' : 'command',
        subBuilder: DeviceCommand.create)
    ..aOM<DeviceTelemetry>(2, _omitFieldNames ? '' : 'telemetry',
        subBuilder: DeviceTelemetry.create)
    ..aOM<BridgeStatus>(3, _omitFieldNames ? '' : 'bridgeStatus',
        subBuilder: BridgeStatus.create)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  DeviceMessage clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  DeviceMessage copyWith(void Function(DeviceMessage) updates) =>
      super.copyWith((message) => updates(message as DeviceMessage))
          as DeviceMessage;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static DeviceMessage create() => DeviceMessage._();
  @$core.override
  DeviceMessage createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static DeviceMessage getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<DeviceMessage>(create);
  static DeviceMessage? _defaultInstance;

  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  @$pb.TagNumber(3)
  DeviceMessage_Payload whichPayload() =>
      _DeviceMessage_PayloadByTag[$_whichOneof(0)]!;
  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  @$pb.TagNumber(3)
  void clearPayload() => $_clearField($_whichOneof(0));

  @$pb.TagNumber(1)
  DeviceCommand get command => $_getN(0);
  @$pb.TagNumber(1)
  set command(DeviceCommand value) => $_setField(1, value);
  @$pb.TagNumber(1)
  $core.bool hasCommand() => $_has(0);
  @$pb.TagNumber(1)
  void clearCommand() => $_clearField(1);
  @$pb.TagNumber(1)
  DeviceCommand ensureCommand() => $_ensure(0);

  @$pb.TagNumber(2)
  DeviceTelemetry get telemetry => $_getN(1);
  @$pb.TagNumber(2)
  set telemetry(DeviceTelemetry value) => $_setField(2, value);
  @$pb.TagNumber(2)
  $core.bool hasTelemetry() => $_has(1);
  @$pb.TagNumber(2)
  void clearTelemetry() => $_clearField(2);
  @$pb.TagNumber(2)
  DeviceTelemetry ensureTelemetry() => $_ensure(1);

  @$pb.TagNumber(3)
  BridgeStatus get bridgeStatus => $_getN(2);
  @$pb.TagNumber(3)
  set bridgeStatus(BridgeStatus value) => $_setField(3, value);
  @$pb.TagNumber(3)
  $core.bool hasBridgeStatus() => $_has(2);
  @$pb.TagNumber(3)
  void clearBridgeStatus() => $_clearField(3);
  @$pb.TagNumber(3)
  BridgeStatus ensureBridgeStatus() => $_ensure(2);
}

enum DeviceCommand_Command {
  setMaxTemp,
  setMinTemp,
  setCurrentTime,
  setTemperatureUnit,
  setMaxHumidity,
  setMinHumidity,
  notSet
}

/// A command sent from a client device to the STM32 through the ESP32 BLE/UART
/// bridge. Exactly one command must be set.
class DeviceCommand extends $pb.GeneratedMessage {
  factory DeviceCommand({
    SetMaxTemp? setMaxTemp,
    SetMinTemp? setMinTemp,
    SetCurrentTime? setCurrentTime,
    SetTemperatureUnit? setTemperatureUnit,
    SetMaxHumidity? setMaxHumidity,
    SetMinHumidity? setMinHumidity,
  }) {
    final result = create();
    if (setMaxTemp != null) result.setMaxTemp = setMaxTemp;
    if (setMinTemp != null) result.setMinTemp = setMinTemp;
    if (setCurrentTime != null) result.setCurrentTime = setCurrentTime;
    if (setTemperatureUnit != null)
      result.setTemperatureUnit = setTemperatureUnit;
    if (setMaxHumidity != null) result.setMaxHumidity = setMaxHumidity;
    if (setMinHumidity != null) result.setMinHumidity = setMinHumidity;
    return result;
  }

  DeviceCommand._();

  factory DeviceCommand.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory DeviceCommand.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static const $core.Map<$core.int, DeviceCommand_Command>
      _DeviceCommand_CommandByTag = {
    1: DeviceCommand_Command.setMaxTemp,
    2: DeviceCommand_Command.setMinTemp,
    3: DeviceCommand_Command.setCurrentTime,
    4: DeviceCommand_Command.setTemperatureUnit,
    5: DeviceCommand_Command.setMaxHumidity,
    6: DeviceCommand_Command.setMinHumidity,
    0: DeviceCommand_Command.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'DeviceCommand',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..oo(0, [1, 2, 3, 4, 5, 6])
    ..aOM<SetMaxTemp>(1, _omitFieldNames ? '' : 'setMaxTemp',
        subBuilder: SetMaxTemp.create)
    ..aOM<SetMinTemp>(2, _omitFieldNames ? '' : 'setMinTemp',
        subBuilder: SetMinTemp.create)
    ..aOM<SetCurrentTime>(3, _omitFieldNames ? '' : 'setCurrentTime',
        subBuilder: SetCurrentTime.create)
    ..aOM<SetTemperatureUnit>(4, _omitFieldNames ? '' : 'setTemperatureUnit',
        subBuilder: SetTemperatureUnit.create)
    ..aOM<SetMaxHumidity>(5, _omitFieldNames ? '' : 'setMaxHumidity',
        subBuilder: SetMaxHumidity.create)
    ..aOM<SetMinHumidity>(6, _omitFieldNames ? '' : 'setMinHumidity',
        subBuilder: SetMinHumidity.create)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  DeviceCommand clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  DeviceCommand copyWith(void Function(DeviceCommand) updates) =>
      super.copyWith((message) => updates(message as DeviceCommand))
          as DeviceCommand;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static DeviceCommand create() => DeviceCommand._();
  @$core.override
  DeviceCommand createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static DeviceCommand getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<DeviceCommand>(create);
  static DeviceCommand? _defaultInstance;

  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  @$pb.TagNumber(3)
  @$pb.TagNumber(4)
  @$pb.TagNumber(5)
  @$pb.TagNumber(6)
  DeviceCommand_Command whichCommand() =>
      _DeviceCommand_CommandByTag[$_whichOneof(0)]!;
  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  @$pb.TagNumber(3)
  @$pb.TagNumber(4)
  @$pb.TagNumber(5)
  @$pb.TagNumber(6)
  void clearCommand() => $_clearField($_whichOneof(0));

  @$pb.TagNumber(1)
  SetMaxTemp get setMaxTemp => $_getN(0);
  @$pb.TagNumber(1)
  set setMaxTemp(SetMaxTemp value) => $_setField(1, value);
  @$pb.TagNumber(1)
  $core.bool hasSetMaxTemp() => $_has(0);
  @$pb.TagNumber(1)
  void clearSetMaxTemp() => $_clearField(1);
  @$pb.TagNumber(1)
  SetMaxTemp ensureSetMaxTemp() => $_ensure(0);

  @$pb.TagNumber(2)
  SetMinTemp get setMinTemp => $_getN(1);
  @$pb.TagNumber(2)
  set setMinTemp(SetMinTemp value) => $_setField(2, value);
  @$pb.TagNumber(2)
  $core.bool hasSetMinTemp() => $_has(1);
  @$pb.TagNumber(2)
  void clearSetMinTemp() => $_clearField(2);
  @$pb.TagNumber(2)
  SetMinTemp ensureSetMinTemp() => $_ensure(1);

  @$pb.TagNumber(3)
  SetCurrentTime get setCurrentTime => $_getN(2);
  @$pb.TagNumber(3)
  set setCurrentTime(SetCurrentTime value) => $_setField(3, value);
  @$pb.TagNumber(3)
  $core.bool hasSetCurrentTime() => $_has(2);
  @$pb.TagNumber(3)
  void clearSetCurrentTime() => $_clearField(3);
  @$pb.TagNumber(3)
  SetCurrentTime ensureSetCurrentTime() => $_ensure(2);

  @$pb.TagNumber(4)
  SetTemperatureUnit get setTemperatureUnit => $_getN(3);
  @$pb.TagNumber(4)
  set setTemperatureUnit(SetTemperatureUnit value) => $_setField(4, value);
  @$pb.TagNumber(4)
  $core.bool hasSetTemperatureUnit() => $_has(3);
  @$pb.TagNumber(4)
  void clearSetTemperatureUnit() => $_clearField(4);
  @$pb.TagNumber(4)
  SetTemperatureUnit ensureSetTemperatureUnit() => $_ensure(3);

  @$pb.TagNumber(5)
  SetMaxHumidity get setMaxHumidity => $_getN(4);
  @$pb.TagNumber(5)
  set setMaxHumidity(SetMaxHumidity value) => $_setField(5, value);
  @$pb.TagNumber(5)
  $core.bool hasSetMaxHumidity() => $_has(4);
  @$pb.TagNumber(5)
  void clearSetMaxHumidity() => $_clearField(5);
  @$pb.TagNumber(5)
  SetMaxHumidity ensureSetMaxHumidity() => $_ensure(4);

  @$pb.TagNumber(6)
  SetMinHumidity get setMinHumidity => $_getN(5);
  @$pb.TagNumber(6)
  set setMinHumidity(SetMinHumidity value) => $_setField(6, value);
  @$pb.TagNumber(6)
  $core.bool hasSetMinHumidity() => $_has(5);
  @$pb.TagNumber(6)
  void clearSetMinHumidity() => $_clearField(6);
  @$pb.TagNumber(6)
  SetMinHumidity ensureSetMinHumidity() => $_ensure(5);
}

class SetMaxTemp extends $pb.GeneratedMessage {
  factory SetMaxTemp({
    $core.int? value,
  }) {
    final result = create();
    if (value != null) result.value = value;
    return result;
  }

  SetMaxTemp._();

  factory SetMaxTemp.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory SetMaxTemp.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'SetMaxTemp',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'value', fieldType: $pb.PbFieldType.OS3)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMaxTemp clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMaxTemp copyWith(void Function(SetMaxTemp) updates) =>
      super.copyWith((message) => updates(message as SetMaxTemp)) as SetMaxTemp;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static SetMaxTemp create() => SetMaxTemp._();
  @$core.override
  SetMaxTemp createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static SetMaxTemp getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetMaxTemp>(create);
  static SetMaxTemp? _defaultInstance;

  /// Temperature in tenths of a degree Celsius.
  /// Examples: 250 means 25.0 C and 155 means 15.5 C.
  /// Valid range: -32768..32767 (int16). Receivers must reject values outside it.
  @$pb.TagNumber(1)
  $core.int get value => $_getIZ(0);
  @$pb.TagNumber(1)
  set value($core.int value) => $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasValue() => $_has(0);
  @$pb.TagNumber(1)
  void clearValue() => $_clearField(1);
}

class SetMinTemp extends $pb.GeneratedMessage {
  factory SetMinTemp({
    $core.int? value,
  }) {
    final result = create();
    if (value != null) result.value = value;
    return result;
  }

  SetMinTemp._();

  factory SetMinTemp.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory SetMinTemp.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'SetMinTemp',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'value', fieldType: $pb.PbFieldType.OS3)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMinTemp clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMinTemp copyWith(void Function(SetMinTemp) updates) =>
      super.copyWith((message) => updates(message as SetMinTemp)) as SetMinTemp;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static SetMinTemp create() => SetMinTemp._();
  @$core.override
  SetMinTemp createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static SetMinTemp getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetMinTemp>(create);
  static SetMinTemp? _defaultInstance;

  /// Temperature in tenths of a degree Celsius.
  /// Examples: 250 means 25.0 C and 155 means 15.5 C.
  /// Valid range: -32768..32767 (int16). Receivers must reject values outside it.
  @$pb.TagNumber(1)
  $core.int get value => $_getIZ(0);
  @$pb.TagNumber(1)
  set value($core.int value) => $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasValue() => $_has(0);
  @$pb.TagNumber(1)
  void clearValue() => $_clearField(1);
}

class SetMaxHumidity extends $pb.GeneratedMessage {
  factory SetMaxHumidity({
    $core.int? value,
  }) {
    final result = create();
    if (value != null) result.value = value;
    return result;
  }

  SetMaxHumidity._();

  factory SetMaxHumidity.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory SetMaxHumidity.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'SetMaxHumidity',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'value', fieldType: $pb.PbFieldType.OU3)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMaxHumidity clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMaxHumidity copyWith(void Function(SetMaxHumidity) updates) =>
      super.copyWith((message) => updates(message as SetMaxHumidity))
          as SetMaxHumidity;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static SetMaxHumidity create() => SetMaxHumidity._();
  @$core.override
  SetMaxHumidity createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static SetMaxHumidity getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetMaxHumidity>(create);
  static SetMaxHumidity? _defaultInstance;

  /// Relative humidity in tenths of a percent. Valid range: 0..1000.
  @$pb.TagNumber(1)
  $core.int get value => $_getIZ(0);
  @$pb.TagNumber(1)
  set value($core.int value) => $_setUnsignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasValue() => $_has(0);
  @$pb.TagNumber(1)
  void clearValue() => $_clearField(1);
}

class SetMinHumidity extends $pb.GeneratedMessage {
  factory SetMinHumidity({
    $core.int? value,
  }) {
    final result = create();
    if (value != null) result.value = value;
    return result;
  }

  SetMinHumidity._();

  factory SetMinHumidity.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory SetMinHumidity.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'SetMinHumidity',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'value', fieldType: $pb.PbFieldType.OU3)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMinHumidity clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetMinHumidity copyWith(void Function(SetMinHumidity) updates) =>
      super.copyWith((message) => updates(message as SetMinHumidity))
          as SetMinHumidity;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static SetMinHumidity create() => SetMinHumidity._();
  @$core.override
  SetMinHumidity createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static SetMinHumidity getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetMinHumidity>(create);
  static SetMinHumidity? _defaultInstance;

  /// Relative humidity in tenths of a percent. Valid range: 0..1000.
  @$pb.TagNumber(1)
  $core.int get value => $_getIZ(0);
  @$pb.TagNumber(1)
  set value($core.int value) => $_setUnsignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasValue() => $_has(0);
  @$pb.TagNumber(1)
  void clearValue() => $_clearField(1);
}

class SetCurrentTime extends $pb.GeneratedMessage {
  factory SetCurrentTime({
    $fixnum.Int64? valueMs,
  }) {
    final result = create();
    if (valueMs != null) result.valueMs = valueMs;
    return result;
  }

  SetCurrentTime._();

  factory SetCurrentTime.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory SetCurrentTime.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'SetCurrentTime',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..a<$fixnum.Int64>(1, _omitFieldNames ? '' : 'valueMs', $pb.PbFieldType.OU6,
        defaultOrMaker: $fixnum.Int64.ZERO)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetCurrentTime clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetCurrentTime copyWith(void Function(SetCurrentTime) updates) =>
      super.copyWith((message) => updates(message as SetCurrentTime))
          as SetCurrentTime;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static SetCurrentTime create() => SetCurrentTime._();
  @$core.override
  SetCurrentTime createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static SetCurrentTime getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetCurrentTime>(create);
  static SetCurrentTime? _defaultInstance;

  /// Current time in milliseconds.
  @$pb.TagNumber(1)
  $fixnum.Int64 get valueMs => $_getI64(0);
  @$pb.TagNumber(1)
  set valueMs($fixnum.Int64 value) => $_setInt64(0, value);
  @$pb.TagNumber(1)
  $core.bool hasValueMs() => $_has(0);
  @$pb.TagNumber(1)
  void clearValueMs() => $_clearField(1);
}

class SetTemperatureUnit extends $pb.GeneratedMessage {
  factory SetTemperatureUnit({
    TemperatureUnit? unit,
  }) {
    final result = create();
    if (unit != null) result.unit = unit;
    return result;
  }

  SetTemperatureUnit._();

  factory SetTemperatureUnit.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory SetTemperatureUnit.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'SetTemperatureUnit',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aE<TemperatureUnit>(1, _omitFieldNames ? '' : 'unit',
        enumValues: TemperatureUnit.values)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetTemperatureUnit clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  SetTemperatureUnit copyWith(void Function(SetTemperatureUnit) updates) =>
      super.copyWith((message) => updates(message as SetTemperatureUnit))
          as SetTemperatureUnit;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static SetTemperatureUnit create() => SetTemperatureUnit._();
  @$core.override
  SetTemperatureUnit createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static SetTemperatureUnit getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetTemperatureUnit>(create);
  static SetTemperatureUnit? _defaultInstance;

  @$pb.TagNumber(1)
  TemperatureUnit get unit => $_getN(0);
  @$pb.TagNumber(1)
  set unit(TemperatureUnit value) => $_setField(1, value);
  @$pb.TagNumber(1)
  $core.bool hasUnit() => $_has(0);
  @$pb.TagNumber(1)
  void clearUnit() => $_clearField(1);
}

enum DeviceTelemetry_Data { measurement, limits, notSet }

class DeviceTelemetry extends $pb.GeneratedMessage {
  factory DeviceTelemetry({
    EnvironmentMeasurement? measurement,
    EnvironmentLimits? limits,
  }) {
    final result = create();
    if (measurement != null) result.measurement = measurement;
    if (limits != null) result.limits = limits;
    return result;
  }

  DeviceTelemetry._();

  factory DeviceTelemetry.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory DeviceTelemetry.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static const $core.Map<$core.int, DeviceTelemetry_Data>
      _DeviceTelemetry_DataByTag = {
    1: DeviceTelemetry_Data.measurement,
    2: DeviceTelemetry_Data.limits,
    0: DeviceTelemetry_Data.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'DeviceTelemetry',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..oo(0, [1, 2])
    ..aOM<EnvironmentMeasurement>(1, _omitFieldNames ? '' : 'measurement',
        subBuilder: EnvironmentMeasurement.create)
    ..aOM<EnvironmentLimits>(2, _omitFieldNames ? '' : 'limits',
        subBuilder: EnvironmentLimits.create)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  DeviceTelemetry clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  DeviceTelemetry copyWith(void Function(DeviceTelemetry) updates) =>
      super.copyWith((message) => updates(message as DeviceTelemetry))
          as DeviceTelemetry;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static DeviceTelemetry create() => DeviceTelemetry._();
  @$core.override
  DeviceTelemetry createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static DeviceTelemetry getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<DeviceTelemetry>(create);
  static DeviceTelemetry? _defaultInstance;

  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  DeviceTelemetry_Data whichData() =>
      _DeviceTelemetry_DataByTag[$_whichOneof(0)]!;
  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  void clearData() => $_clearField($_whichOneof(0));

  @$pb.TagNumber(1)
  EnvironmentMeasurement get measurement => $_getN(0);
  @$pb.TagNumber(1)
  set measurement(EnvironmentMeasurement value) => $_setField(1, value);
  @$pb.TagNumber(1)
  $core.bool hasMeasurement() => $_has(0);
  @$pb.TagNumber(1)
  void clearMeasurement() => $_clearField(1);
  @$pb.TagNumber(1)
  EnvironmentMeasurement ensureMeasurement() => $_ensure(0);

  @$pb.TagNumber(2)
  EnvironmentLimits get limits => $_getN(1);
  @$pb.TagNumber(2)
  set limits(EnvironmentLimits value) => $_setField(2, value);
  @$pb.TagNumber(2)
  $core.bool hasLimits() => $_has(1);
  @$pb.TagNumber(2)
  void clearLimits() => $_clearField(2);
  @$pb.TagNumber(2)
  EnvironmentLimits ensureLimits() => $_ensure(1);
}

class EnvironmentMeasurement extends $pb.GeneratedMessage {
  factory EnvironmentMeasurement({
    $core.int? temperatureTenthsCelsius,
    $core.int? humidityMilliPercent,
  }) {
    final result = create();
    if (temperatureTenthsCelsius != null)
      result.temperatureTenthsCelsius = temperatureTenthsCelsius;
    if (humidityMilliPercent != null)
      result.humidityMilliPercent = humidityMilliPercent;
    return result;
  }

  EnvironmentMeasurement._();

  factory EnvironmentMeasurement.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory EnvironmentMeasurement.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'EnvironmentMeasurement',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'temperatureTenthsCelsius',
        fieldType: $pb.PbFieldType.OS3)
    ..aI(2, _omitFieldNames ? '' : 'humidityMilliPercent',
        fieldType: $pb.PbFieldType.OU3)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  EnvironmentMeasurement clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  EnvironmentMeasurement copyWith(
          void Function(EnvironmentMeasurement) updates) =>
      super.copyWith((message) => updates(message as EnvironmentMeasurement))
          as EnvironmentMeasurement;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static EnvironmentMeasurement create() => EnvironmentMeasurement._();
  @$core.override
  EnvironmentMeasurement createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static EnvironmentMeasurement getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<EnvironmentMeasurement>(create);
  static EnvironmentMeasurement? _defaultInstance;

  /// Temperature in tenths of a degree Celsius.
  @$pb.TagNumber(1)
  $core.int get temperatureTenthsCelsius => $_getIZ(0);
  @$pb.TagNumber(1)
  set temperatureTenthsCelsius($core.int value) => $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasTemperatureTenthsCelsius() => $_has(0);
  @$pb.TagNumber(1)
  void clearTemperatureTenthsCelsius() => $_clearField(1);

  /// Relative humidity in thousandths of a percent.
  @$pb.TagNumber(2)
  $core.int get humidityMilliPercent => $_getIZ(1);
  @$pb.TagNumber(2)
  set humidityMilliPercent($core.int value) => $_setUnsignedInt32(1, value);
  @$pb.TagNumber(2)
  $core.bool hasHumidityMilliPercent() => $_has(1);
  @$pb.TagNumber(2)
  void clearHumidityMilliPercent() => $_clearField(2);
}

class EnvironmentLimits extends $pb.GeneratedMessage {
  factory EnvironmentLimits({
    $core.int? minTemperatureTenthsCelsius,
    $core.int? maxTemperatureTenthsCelsius,
    $core.int? minHumidityTenthsPercent,
    $core.int? maxHumidityTenthsPercent,
  }) {
    final result = create();
    if (minTemperatureTenthsCelsius != null)
      result.minTemperatureTenthsCelsius = minTemperatureTenthsCelsius;
    if (maxTemperatureTenthsCelsius != null)
      result.maxTemperatureTenthsCelsius = maxTemperatureTenthsCelsius;
    if (minHumidityTenthsPercent != null)
      result.minHumidityTenthsPercent = minHumidityTenthsPercent;
    if (maxHumidityTenthsPercent != null)
      result.maxHumidityTenthsPercent = maxHumidityTenthsPercent;
    return result;
  }

  EnvironmentLimits._();

  factory EnvironmentLimits.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory EnvironmentLimits.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'EnvironmentLimits',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'minTemperatureTenthsCelsius',
        fieldType: $pb.PbFieldType.OS3)
    ..aI(2, _omitFieldNames ? '' : 'maxTemperatureTenthsCelsius',
        fieldType: $pb.PbFieldType.OS3)
    ..aI(3, _omitFieldNames ? '' : 'minHumidityTenthsPercent',
        fieldType: $pb.PbFieldType.OU3)
    ..aI(4, _omitFieldNames ? '' : 'maxHumidityTenthsPercent',
        fieldType: $pb.PbFieldType.OU3)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  EnvironmentLimits clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  EnvironmentLimits copyWith(void Function(EnvironmentLimits) updates) =>
      super.copyWith((message) => updates(message as EnvironmentLimits))
          as EnvironmentLimits;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static EnvironmentLimits create() => EnvironmentLimits._();
  @$core.override
  EnvironmentLimits createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static EnvironmentLimits getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<EnvironmentLimits>(create);
  static EnvironmentLimits? _defaultInstance;

  /// Configured minimum temperature in tenths of a degree Celsius.
  @$pb.TagNumber(1)
  $core.int get minTemperatureTenthsCelsius => $_getIZ(0);
  @$pb.TagNumber(1)
  set minTemperatureTenthsCelsius($core.int value) =>
      $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasMinTemperatureTenthsCelsius() => $_has(0);
  @$pb.TagNumber(1)
  void clearMinTemperatureTenthsCelsius() => $_clearField(1);

  /// Configured maximum temperature in tenths of a degree Celsius.
  @$pb.TagNumber(2)
  $core.int get maxTemperatureTenthsCelsius => $_getIZ(1);
  @$pb.TagNumber(2)
  set maxTemperatureTenthsCelsius($core.int value) =>
      $_setSignedInt32(1, value);
  @$pb.TagNumber(2)
  $core.bool hasMaxTemperatureTenthsCelsius() => $_has(1);
  @$pb.TagNumber(2)
  void clearMaxTemperatureTenthsCelsius() => $_clearField(2);

  /// Configured minimum relative humidity in tenths of a percent.
  @$pb.TagNumber(3)
  $core.int get minHumidityTenthsPercent => $_getIZ(2);
  @$pb.TagNumber(3)
  set minHumidityTenthsPercent($core.int value) => $_setUnsignedInt32(2, value);
  @$pb.TagNumber(3)
  $core.bool hasMinHumidityTenthsPercent() => $_has(2);
  @$pb.TagNumber(3)
  void clearMinHumidityTenthsPercent() => $_clearField(3);

  /// Configured maximum relative humidity in tenths of a percent.
  @$pb.TagNumber(4)
  $core.int get maxHumidityTenthsPercent => $_getIZ(3);
  @$pb.TagNumber(4)
  set maxHumidityTenthsPercent($core.int value) => $_setUnsignedInt32(3, value);
  @$pb.TagNumber(4)
  $core.bool hasMaxHumidityTenthsPercent() => $_has(3);
  @$pb.TagNumber(4)
  void clearMaxHumidityTenthsPercent() => $_clearField(4);
}

/// BLE connection state reported by the ESP32 bridge to the STM32.
class BridgeStatus extends $pb.GeneratedMessage {
  factory BridgeStatus({
    BleConnectionState? bleConnectionState,
  }) {
    final result = create();
    if (bleConnectionState != null)
      result.bleConnectionState = bleConnectionState;
    return result;
  }

  BridgeStatus._();

  factory BridgeStatus.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory BridgeStatus.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'BridgeStatus',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aE<BleConnectionState>(1, _omitFieldNames ? '' : 'bleConnectionState',
        enumValues: BleConnectionState.values)
    ..hasRequiredFields = false;

  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  BridgeStatus clone() => deepCopy();
  @$core.Deprecated('See https://github.com/google/protobuf.dart/issues/998.')
  BridgeStatus copyWith(void Function(BridgeStatus) updates) =>
      super.copyWith((message) => updates(message as BridgeStatus))
          as BridgeStatus;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static BridgeStatus create() => BridgeStatus._();
  @$core.override
  BridgeStatus createEmptyInstance() => create();
  @$core.pragma('dart2js:noInline')
  static BridgeStatus getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<BridgeStatus>(create);
  static BridgeStatus? _defaultInstance;

  @$pb.TagNumber(1)
  BleConnectionState get bleConnectionState => $_getN(0);
  @$pb.TagNumber(1)
  set bleConnectionState(BleConnectionState value) => $_setField(1, value);
  @$pb.TagNumber(1)
  $core.bool hasBleConnectionState() => $_has(0);
  @$pb.TagNumber(1)
  void clearBleConnectionState() => $_clearField(1);
}

const $core.bool _omitFieldNames =
    $core.bool.fromEnvironment('protobuf.omit_field_names');
const $core.bool _omitMessageNames =
    $core.bool.fromEnvironment('protobuf.omit_message_names');
