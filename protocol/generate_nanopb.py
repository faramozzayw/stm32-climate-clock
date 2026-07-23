#!/usr/bin/env python3
"""Regenerate or verify STM32 nanopb bindings using the nanopb submodule."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import subprocess
import sys
import tempfile


PROTOCOL_DIR = Path(__file__).resolve().parent
REPOSITORY_ROOT = PROTOCOL_DIR.parent
PROTO_FILE = PROTOCOL_DIR / "device.proto"
NANOPB_ROOT = REPOSITORY_ROOT / "third_party" / "nanopb"
NANOPB_GENERATOR = NANOPB_ROOT / "generator" / "nanopb_generator.py"
OUTPUT_DIR = REPOSITORY_ROOT / "stm32" / "Protocol"
GENERATED_FILES = ("device.pb.c", "device.pb.h")


def verify_prerequisites() -> None:
    if not NANOPB_GENERATOR.is_file():
        raise RuntimeError(
            "nanopb submodule is not initialized; run "
            "'git submodule update --init third_party/nanopb'"
        )

    if not PROTO_FILE.is_file():
        raise RuntimeError(f"protobuf schema not found: {PROTO_FILE}")

    try:
        import google.protobuf  # noqa: F401
        import grpc_tools.protoc  # noqa: F401
    except ImportError as error:
        raise RuntimeError(
            "Python packages 'protobuf' and 'grpcio-tools' are required; "
            "install them with 'python -m pip install protobuf grpcio-tools'"
        ) from error


def generate_into(output_directory: Path) -> None:
    subprocess.run(
        [
            sys.executable,
            str(NANOPB_GENERATOR),
            "-I",
            str(PROTOCOL_DIR),
            "-D",
            str(output_directory),
            str(PROTO_FILE),
        ],
        check=True,
    )


def replace_if_changed(source: Path, destination: Path) -> bool:
    generated = source.read_bytes()
    if destination.is_file() and destination.read_bytes() == generated:
        return False

    destination.parent.mkdir(parents=True, exist_ok=True)
    file_descriptor, temporary_name = tempfile.mkstemp(
        prefix=f".{destination.name}.", dir=destination.parent
    )
    try:
        with os.fdopen(file_descriptor, "wb") as temporary_file:
            temporary_file.write(generated)
        os.replace(temporary_name, destination)
    except BaseException:
        try:
            os.unlink(temporary_name)
        except FileNotFoundError:
            pass
        raise

    return True


def generate() -> int:
    with tempfile.TemporaryDirectory(prefix="nanopb-codegen-") as temporary:
        temporary_directory = Path(temporary)
        generate_into(temporary_directory)

        changed = []
        for filename in GENERATED_FILES:
            if replace_if_changed(
                temporary_directory / filename, OUTPUT_DIR / filename
            ):
                changed.append(filename)

    if changed:
        print("Updated STM32 nanopb bindings: " + ", ".join(changed))
    else:
        print("STM32 nanopb bindings are already up to date.")
    return 0


def check() -> int:
    with tempfile.TemporaryDirectory(prefix="nanopb-codegen-check-") as temporary:
        temporary_directory = Path(temporary)
        generate_into(temporary_directory)

        stale = [
            filename
            for filename in GENERATED_FILES
            if not (OUTPUT_DIR / filename).is_file()
            or (temporary_directory / filename).read_bytes()
            != (OUTPUT_DIR / filename).read_bytes()
        ]

    if stale:
        print(
            "STM32 nanopb bindings are stale: "
            + ", ".join(stale)
            + ". Run 'make -C protocol protobuf'.",
            file=sys.stderr,
        )
        return 1

    print("STM32 nanopb bindings match protocol/device.proto.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("generate", "check"))
    arguments = parser.parse_args()

    try:
        verify_prerequisites()
        return generate() if arguments.mode == "generate" else check()
    except (RuntimeError, subprocess.CalledProcessError) as error:
        print(f"nanopb code generation failed: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
