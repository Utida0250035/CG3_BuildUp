#!/usr/bin/env python3
"""Smoke-test BinaryConverter (OBJ -> .amesh) for CI."""

from __future__ import annotations

import struct
import subprocess
import sys
import tempfile
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def read_amesh_header(path: Path) -> tuple[bytes, int, int]:
    data = path.read_bytes()
    if len(data) < 12:
        raise ValueError(f"{path}: file too small ({len(data)} bytes)")
    magic, version, vertex_count = struct.unpack_from("<4sII", data, 0)
    expected = 12 + vertex_count * 36
    if len(data) != expected:
        raise ValueError(
            f"{path}: size mismatch (got {len(data)}, expected {expected})"
        )
    return magic, version, vertex_count


def main() -> int:
    root = repo_root()
    tool_dir = root / "EditorAssets/Tools/BinaryConverter"
    converter = tool_dir / "obj_to_amesh.py"
    fixture = tool_dir / "fixtures/minimal_triangle.obj"

    for path in (converter, fixture):
        if not path.is_file():
            print(f"verify_binary_converter: missing {path}", file=sys.stderr)
            return 1

    with tempfile.TemporaryDirectory(prefix="amesh-verify-") as tmp:
        out_path = Path(tmp) / "minimal_triangle.amesh"
        proc = subprocess.run(
            [sys.executable, str(converter), str(fixture), str(out_path)],
            capture_output=True,
            text=True,
        )
        if proc.returncode != 0:
            print(proc.stdout, end="")
            print(proc.stderr, end="", file=sys.stderr)
            print("verify_binary_converter: obj_to_amesh failed", file=sys.stderr)
            return 1

        try:
            magic, version, vertex_count = read_amesh_header(out_path)
        except ValueError as exc:
            print(str(exc), file=sys.stderr)
            return 1

        if magic != b"AMSH":
            print(f"verify_binary_converter: bad magic {magic!r}", file=sys.stderr)
            return 1
        if version != 1:
            print(f"verify_binary_converter: bad version {version}", file=sys.stderr)
            return 1
        if vertex_count != 3:
            print(
                f"verify_binary_converter: expected 3 vertices, got {vertex_count}",
                file=sys.stderr,
            )
            return 1

    batch = tool_dir / "batch_obj_to_amesh.py"
    if not batch.is_file():
        print(f"verify_binary_converter: missing {batch}", file=sys.stderr)
        return 1

    with tempfile.TemporaryDirectory(prefix="amesh-batch-") as tmp:
        batch_out = Path(tmp) / "out"
        batch_proc = subprocess.run(
            [
                sys.executable,
                str(batch),
                str(fixture.parent),
                "--output-root",
                str(batch_out),
            ],
            cwd=str(tool_dir),
            capture_output=True,
            text=True,
        )
        if batch_proc.returncode != 0:
            print(batch_proc.stdout, end="")
            print(batch_proc.stderr, end="", file=sys.stderr)
            print("verify_binary_converter: batch failed", file=sys.stderr)
            return 1

        batch_file = batch_out / "minimal_triangle.amesh"
        if not batch_file.is_file():
            print(
                "verify_binary_converter: batch did not produce minimal_triangle.amesh",
                file=sys.stderr,
            )
            return 1

    print("verify_binary_converter: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())