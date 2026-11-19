#!/usr/bin/env python3
"""Batch-convert Wavefront OBJ files under a directory to .amesh."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from obj_to_amesh import parse_obj, write_amesh


def default_output_path(input_obj: Path, output_root: Path | None) -> Path:
    if output_root is None:
        return input_obj.with_suffix(".amesh")
    relative = input_obj.with_suffix(".amesh").name
    return output_root / relative


def collect_obj_files(input_root: Path, recursive: bool) -> list[Path]:
    pattern = "**/*.obj" if recursive else "*.obj"
    return sorted(p for p in input_root.glob(pattern) if p.is_file())


def convert_one(input_obj: Path, output_amesh: Path, skip_up_to_date: bool) -> bool:
    if skip_up_to_date and output_amesh.is_file():
        if output_amesh.stat().st_mtime >= input_obj.stat().st_mtime:
            print(f"skip (up to date): {input_obj}")
            return True

    output_amesh.parent.mkdir(parents=True, exist_ok=True)
    try:
        vertices = parse_obj(input_obj)
        write_amesh(vertices, output_amesh)
    except (ValueError, OSError) as exc:
        print(f"Error converting {input_obj}: {exc}", file=sys.stderr)
        return False

    print(f"Wrote {len(vertices)} vertices -> {output_amesh}")
    return True


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Batch convert OBJ files to .amesh (see README)."
    )
    parser.add_argument(
        "input_root",
        type=Path,
        help="Directory that contains .obj files",
    )
    parser.add_argument(
        "--output-root",
        type=Path,
        default=None,
        help="Optional output directory (default: each .obj beside its .amesh)",
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Include subdirectories",
    )
    parser.add_argument(
        "--skip-up-to-date",
        action="store_true",
        help="Skip when output .amesh is newer than or same age as input",
    )
    args = parser.parse_args()

    if not args.input_root.is_dir():
        print(f"Input root not found: {args.input_root}", file=sys.stderr)
        return 1

    obj_files = collect_obj_files(args.input_root, args.recursive)
    if not obj_files:
        print(f"No .obj files under {args.input_root}", file=sys.stderr)
        return 1

    failures = 0
    for input_obj in obj_files:
        output_amesh = default_output_path(input_obj, args.output_root)
        if not convert_one(input_obj, output_amesh, args.skip_up_to_date):
            failures += 1

    if failures:
        print(f"{failures} conversion(s) failed", file=sys.stderr)
        return 1

    print(f"batch_obj_to_amesh: OK ({len(obj_files)} file(s))")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())