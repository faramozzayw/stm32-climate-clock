// Generated-style Dart classes for protocol/device.proto.
// Regenerate this file with protoc when the schema changes.

// ignore_for_file: annotate_overrides, camel_case_types
// ignore_for_file: constant_identifier_names

import 'dart:core' as $core;

import 'package:fixnum/fixnum.dart' as $fixnum;
import 'package:protobuf/protobuf.dart' as $pb;

enum DeviceCommand_Command { setMaxTemp, setMinTemp, setCurrentTime, notSet }

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

  factory DeviceCommand.fromBuffer(
    $core.List<$core.int> data, [
    $pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY,
  ]) => create()..mergeFromBuffer(data, registry);

  static const $core.Map<$core.int, DeviceCommand_Command>
  _DeviceCommand_CommandByTag = {
    1: DeviceCommand_Command.setMaxTemp,
    2: DeviceCommand_Command.setMinTemp,
    3: DeviceCommand_Command.setCurrentTime,
    0: DeviceCommand_Command.notSet,
  };

  static final $pb.BuilderInfo _i =
      $pb.BuilderInfo(
          'DeviceCommand',
          package: const $pb.PackageName('device'),
          createEmptyInstance: create,
        )
        ..oo(0, [1, 2, 3])
        ..aOM<SetMaxTemp>(1, 'setMaxTemp', subBuilder: SetMaxTemp.create)
        ..aOM<SetMinTemp>(2, 'setMinTemp', subBuilder: SetMinTemp.create)
        ..aOM<SetCurrentTime>(
          3,
          'setCurrentTime',
          subBuilder: SetCurrentTime.create,
        )
        ..hasRequiredFields = false;

  @$core.Deprecated('Use deepCopy instead.')
  DeviceCommand clone() => deepCopy();
  @$core.Deprecated('Use rebuild instead.')
  DeviceCommand copyWith(void Function(DeviceCommand) updates) =>
      super.copyWith((message) => updates(message as DeviceCommand))
          as DeviceCommand;

  @$core.override
  $pb.BuilderInfo get info_ => _i;

  static DeviceCommand create() => DeviceCommand._();
  @$core.override
  DeviceCommand createEmptyInstance() => create();
  static DeviceCommand? _defaultInstance;
  static DeviceCommand getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<DeviceCommand>(create);

  DeviceCommand_Command whichCommand() =>
      _DeviceCommand_CommandByTag[$_whichOneof(0)]!;
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
  factory SetMaxTemp({$core.int? value}) {
    final result = create();
    if (value != null) result.value = value;
    return result;
  }

  SetMaxTemp._();

  factory SetMaxTemp.fromBuffer(
    $core.List<$core.int> data, [
    $pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY,
  ]) => create()..mergeFromBuffer(data, registry);

  static final $pb.BuilderInfo _i =
      $pb.BuilderInfo(
          'SetMaxTemp',
          package: const $pb.PackageName('device'),
          createEmptyInstance: create,
        )
        ..a<$core.int>(1, 'value', $pb.PbFieldType.OS3)
        ..hasRequiredFields = false;

  @$core.Deprecated('Use deepCopy instead.')
  SetMaxTemp clone() => deepCopy();
  @$core.Deprecated('Use rebuild instead.')
  SetMaxTemp copyWith(void Function(SetMaxTemp) updates) =>
      super.copyWith((message) => updates(message as SetMaxTemp)) as SetMaxTemp;

  @$core.override
  $pb.BuilderInfo get info_ => _i;
  static SetMaxTemp create() => SetMaxTemp._();
  @$core.override
  SetMaxTemp createEmptyInstance() => create();
  static SetMaxTemp? _defaultInstance;
  static SetMaxTemp getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetMaxTemp>(create);

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
  factory SetMinTemp({$core.int? value}) {
    final result = create();
    if (value != null) result.value = value;
    return result;
  }

  SetMinTemp._();

  factory SetMinTemp.fromBuffer(
    $core.List<$core.int> data, [
    $pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY,
  ]) => create()..mergeFromBuffer(data, registry);

  static final $pb.BuilderInfo _i =
      $pb.BuilderInfo(
          'SetMinTemp',
          package: const $pb.PackageName('device'),
          createEmptyInstance: create,
        )
        ..a<$core.int>(1, 'value', $pb.PbFieldType.OS3)
        ..hasRequiredFields = false;

  @$core.Deprecated('Use deepCopy instead.')
  SetMinTemp clone() => deepCopy();
  @$core.Deprecated('Use rebuild instead.')
  SetMinTemp copyWith(void Function(SetMinTemp) updates) =>
      super.copyWith((message) => updates(message as SetMinTemp)) as SetMinTemp;

  @$core.override
  $pb.BuilderInfo get info_ => _i;
  static SetMinTemp create() => SetMinTemp._();
  @$core.override
  SetMinTemp createEmptyInstance() => create();
  static SetMinTemp? _defaultInstance;
  static SetMinTemp getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetMinTemp>(create);

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
  factory SetCurrentTime({$fixnum.Int64? valueMs}) {
    final result = create();
    if (valueMs != null) result.valueMs = valueMs;
    return result;
  }

  SetCurrentTime._();

  factory SetCurrentTime.fromBuffer(
    $core.List<$core.int> data, [
    $pb.ExtensionRegistry registry = $pb.ExtensionRegistry.EMPTY,
  ]) => create()..mergeFromBuffer(data, registry);

  static final $pb.BuilderInfo _i =
      $pb.BuilderInfo(
          'SetCurrentTime',
          package: const $pb.PackageName('device'),
          createEmptyInstance: create,
        )
        ..a<$fixnum.Int64>(
          1,
          'valueMs',
          $pb.PbFieldType.OU6,
          defaultOrMaker: $fixnum.Int64.ZERO,
        )
        ..hasRequiredFields = false;

  @$core.Deprecated('Use deepCopy instead.')
  SetCurrentTime clone() => deepCopy();
  @$core.Deprecated('Use rebuild instead.')
  SetCurrentTime copyWith(void Function(SetCurrentTime) updates) =>
      super.copyWith((message) => updates(message as SetCurrentTime))
          as SetCurrentTime;

  @$core.override
  $pb.BuilderInfo get info_ => _i;
  static SetCurrentTime create() => SetCurrentTime._();
  @$core.override
  SetCurrentTime createEmptyInstance() => create();
  static SetCurrentTime? _defaultInstance;
  static SetCurrentTime getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<SetCurrentTime>(create);

  @$pb.TagNumber(1)
  $fixnum.Int64 get valueMs => $_getI64(0);
  @$pb.TagNumber(1)
  set valueMs($fixnum.Int64 value) => $_setInt64(0, value);
  @$pb.TagNumber(1)
  $core.bool hasValueMs() => $_has(0);
  @$pb.TagNumber(1)
  void clearValueMs() => $_clearField(1);
}
