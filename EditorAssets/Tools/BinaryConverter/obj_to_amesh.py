#!/usr/bin/env python3
"""Wavefront OBJ (triangle faces) -> Atrum .amesh (see atr_mesh_binary_format.md)."""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

MAGIC = b"AMSH"
VERSION = 1
VERTEX_STRUCT = struct.Struct("<4f2f3f")  # position xyzw, uv, normal xyz


def parse_obj(path: Path) -> list[tuple]:
    positions: list[tuple[float, float, float]] = []
    tex_coords: list[tuple[float, float]] = []
    normals: list[tuple[float, float, float]] = []
    vertices: list[tuple] = []

    with path.open(encoding="utf-8", errors="replace") as handle:
        for raw in handle:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            tag = parts[0]

            if tag == "v" and len(parts) >= 4:
                positions.append(
                    (float(parts[1]), float(parts[2]), float(parts[3]))
                )
            elif tag == "vt" and len(parts) >= 3:
                tex_coords.append((float(parts[1]), float(parts[2])))
            elif tag == "vn" and len(parts) >= 4:
                normals.append(
                    (float(parts[1]), float(parts[2]), float(parts[3]))
                )
            elif tag == "f" and len(parts) >= 4:
                if len(parts) != 4:
                    raise ValueError(
                        f"Only triangle faces supported: {line!r}"
                    )
                triangle = []
                for corner in parts[1:4]:
                    indices = corner.split("/")
                    while len(indices) < 3:
                        indices.append("")
                    vi = int(indices[0]) if indices[0] else 1
                    ti = int(indices[1]) if len(indices) > 1 and indices[1] else 0
                    ni = int(indices[2]) if len(indices) > 2 and indices[2] else 0

                    px, py, pz = positions[vi - 1]
                    pz = -pz

                    u, v = 0.0, 0.0
                    if ti > 0 and tex_coords:
                        u, v = tex_coords[ti - 1]
                        v = 1.0 - v

                    nx, ny, nz = (0.0, 1.0, 0.0)
                    if ni > 0 and normals:
                        nx, ny, nz = normals[ni - 1]
                        nz = -nz

                    triangle.append((px, py, pz, 1.0, u, v, nx, ny, nz))

                vertices.extend(reversed(triangle))

    if not vertices:
        raise ValueError(f"No triangle vertices produced from {path}")

    return vertices


def write_amesh(vertices: list[tuple], out_path: Path) -> None:
    header = struct.pack("<4sII", MAGIC, VERSION, len(vertices))
    body = b"".join(VERTEX_STRUCT.pack(*v) for v in vertices)
    out_path.write_bytes(header + body)


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert OBJ to .amesh")
    parser.add_argument("input_obj", type=Path)
    parser.add_argument("output_amesh", type=Path)
    args = parser.parse_args()

    if not args.input_obj.is_file():
        print(f"Input not found: {args.input_obj}", file=sys.stderr)
        return 1

    try:
        vertices = parse_obj(args.input_obj)
        write_amesh(vertices, args.output_amesh)
    except (ValueError, OSError, struct.error) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    print(f"Wrote {len(vertices)} vertices -> {args.output_amesh}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
