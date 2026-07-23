#!/usr/bin/env python3
"""Regenerate Flutter protobuf bindings from protocol/device.proto."""

from __future__ import annotations

import os
from pathlib import Path
import shutil
import sys


REPOSITORY_ROOT = Path(__file__).resolve().parent.parent
PROTO_FILE = REPOSITORY_ROOT / "protocol" / "device.proto"
OUTPUT_DIR = REPOSITORY_ROOT / "app" / "lib" / "generated"


def find_dart_plugin() -> str:
    plugin = shutil.which("protoc-gen-dart")
    if plugin:
        return plugin

    local_app_data = os.environ.get("LOCALAPPDATA")
    if local_app_data:
        plugin = (
            Path(local_app_data)
            / "Pub"
            / "Cache"
            / "bin"
            / "protoc-gen-dart.bat"
        )
        if plugin.is_file():
            return str(plugin)

    raise RuntimeError(
        "Dart protoc plugin is not installed; run "
        "'dart pub global activate protoc_plugin'"
    )


def main() -> int:
    plugin = find_dart_plugin()
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    try:
        from grpc_tools import protoc
    except ImportError as error:
        raise RuntimeError(
            "Python package 'grpcio-tools' is required; install it with "
            "'python -m pip install grpcio-tools'"
        ) from error

    result = protoc.main(
        [
            "protoc",
            f"-I{PROTO_FILE.parent}",
            f"--plugin=protoc-gen-dart={plugin}",
            f"--dart_out={OUTPUT_DIR}",
            str(PROTO_FILE),
        ]
    )
    if result != 0:
        return result

    print("Updated Flutter protobuf bindings.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as error:
        print(f"Dart protobuf generation failed: {error}", file=sys.stderr)
        raise SystemExit(1) from error
