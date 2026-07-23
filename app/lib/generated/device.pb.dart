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

export 'package:protobuf/protobuf.dart' show GeneratedMessageGenericExtensions;

enum DeviceCommand_Command { setMaxTemp, setMinTemp, setCurrentTime, notSet }

/// A command sent from the phone to the STM32 through the ESP32 BLE/UART
/// bridge. Exactly one command must be set.
class DeviceCommand extends $pb.GeneratedMessage {
  factory DeviceCommand({
    SetMaxTemp? setMaxTemp,
    SetMinTemp? setMinTemp,
    SetCurrentTime? setCurrentTime,
  }) {
    final result = create();
    if (setMaxTemp != null) result.setMaxTemp = setMaxTemp;
    if (setMinTemp != null) result.setMinTemp = setMinTemp;
    if (setCurrentTime != null) result.setCurrentTime = setCurrentTime;
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
    0: DeviceCommand_Command.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'DeviceCommand',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..oo(0, [1, 2, 3])
    ..aOM<SetMaxTemp>(1, _omitFieldNames ? '' : 'setMaxTemp',
        subBuilder: SetMaxTemp.create)
    ..aOM<SetMinTemp>(2, _omitFieldNames ? '' : 'setMinTemp',
        subBuilder: SetMinTemp.create)
    ..aOM<SetCurrentTime>(3, _omitFieldNames ? '' : 'setCurrentTime',
        subBuilder: SetCurrentTime.create)
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
  DeviceCommand_Command whichCommand() =>
      _DeviceCommand_CommandByTag[$_whichOneof(0)]!;
  @$pb.TagNumber(1)
  @$pb.TagNumber(2)
  @$pb.TagNumber(3)
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
    ..aI(1, _omitFieldNames ? '' : 'value', fieldType: $pb.PbFieldType.OS3)
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

  @$pb.TagNumber(1)
  $core.int get value => $_getIZ(0);
  @$pb.TagNumber(1)
  set value($core.int value) => $_setSignedInt32(0, value);
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
    ..aI(1, _omitFieldNames ? '' : 'value', fieldType: $pb.PbFieldType.OS3)
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

  @$pb.TagNumber(1)
  $core.int get value => $_getIZ(0);
  @$pb.TagNumber(1)
  set value($core.int value) => $_setSignedInt32(0, value);
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

class DeviceTelemetry extends $pb.GeneratedMessage {
  factory DeviceTelemetry({
    $core.int? currentTemp,
    $core.int? minTemp,
    $core.int? maxTemp,
  }) {
    final result = create();
    if (currentTemp != null) result.currentTemp = currentTemp;
    if (minTemp != null) result.minTemp = minTemp;
    if (maxTemp != null) result.maxTemp = maxTemp;
    return result;
  }

  DeviceTelemetry._();

  factory DeviceTelemetry.fromBuffer($core.List<$core.int> data,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(data, registry);
  factory DeviceTelemetry.fromJson($core.String json,
          [$pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(json, registry);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'DeviceTelemetry',
      package: const $pb.PackageName(_omitMessageNames ? '' : 'device'),
      createEmptyInstance: create)
    ..aI(1, _omitFieldNames ? '' : 'currentTemp',
        fieldType: $pb.PbFieldType.OS3)
    ..aI(2, _omitFieldNames ? '' : 'minTemp', fieldType: $pb.PbFieldType.OS3)
    ..aI(3, _omitFieldNames ? '' : 'maxTemp', fieldType: $pb.PbFieldType.OS3)
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

  /// Temperature in tenths of a degree Celsius.
  @$pb.TagNumber(1)
  $core.int get currentTemp => $_getIZ(0);
  @$pb.TagNumber(1)
  set currentTemp($core.int value) => $_setSignedInt32(0, value);
  @$pb.TagNumber(1)
  $core.bool hasCurrentTemp() => $_has(0);
  @$pb.TagNumber(1)
  void clearCurrentTemp() => $_clearField(1);

  /// Configured minimum temperature in tenths of a degree Celsius.
  @$pb.TagNumber(2)
  $core.int get minTemp => $_getIZ(1);
  @$pb.TagNumber(2)
  set minTemp($core.int value) => $_setSignedInt32(1, value);
  @$pb.TagNumber(2)
  $core.bool hasMinTemp() => $_has(1);
  @$pb.TagNumber(2)
  void clearMinTemp() => $_clearField(2);

  /// Configured maximum temperature in tenths of a degree Celsius.
  @$pb.TagNumber(3)
  $core.int get maxTemp => $_getIZ(2);
  @$pb.TagNumber(3)
  set maxTemp($core.int value) => $_setSignedInt32(2, value);
  @$pb.TagNumber(3)
  $core.bool hasMaxTemp() => $_has(2);
  @$pb.TagNumber(3)
  void clearMaxTemp() => $_clearField(3);
}

const $core.bool _omitFieldNames =
    $core.bool.fromEnvironment('protobuf.omit_field_names');
const $core.bool _omitMessageNames =
    $core.bool.fromEnvironment('protobuf.omit_message_names');
