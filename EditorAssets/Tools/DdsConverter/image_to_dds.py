#!/usr/bin/env python3
"""Convert common image formats to legacy 32-bit RGBA DDS (DirectXTex-compatible)."""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print(
        "Pillow is required: pip install -r requirements.txt",
        file=sys.stderr,
    )
    raise SystemExit(1)

DDS_MAGIC = 0x20534444  # "DDS "
DDSD_CAPS = 0x1
DDSD_HEIGHT = 0x2
DDSD_WIDTH = 0x4
DDSD_PIXELFORMAT = 0x1000
DDSD_MIPMAPCOUNT = 0x20000
DDSD_PITCH = 0x8

DDSCAPS_TEXTURE = 0x1000
DDSCAPS_COMPLEX = 0x8
DDSCAPS_MIPMAP = 0x400000

DDPF_ALPHAPIXELS = 0x1
DDPF_RGB = 0x40


def build_mip_chain(image: Image.Image, max_levels: int | None) -> list[Image.Image]:
    rgba = image.convert("RGBA")
    levels: list[Image.Image] = [rgba]
    while True:
        current = levels[-1]
        if current.width == 1 and current.height == 1:
            break
        if max_levels is not None and len(levels) >= max_levels:
            break
        next_w = max(1, current.width // 2)
        next_h = max(1, current.height // 2)
        levels.append(current.resize((next_w, next_h), Image.Resampling.LANCZOS))
    return levels


def write_dds_rgba8(levels: list[Image.Image], out_path: Path) -> None:
    width = levels[0].width
    height = levels[0].height
    mip_count = len(levels)

    header_size = 124
    pixel_format_size = 32

    caps = DDSCAPS_TEXTURE
    if mip_count > 1:
        caps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP

    flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
    if mip_count > 1:
        flags |= DDSD_MIPMAPCOUNT
    flags |= DDSD_PITCH

    pitch = width * 4
    reserved1 = (0,) * 11
    pixel_format = (
        pixel_format_size,
        DDPF_ALPHAPIXELS | DDPF_RGB,
        0,
        32,
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000,
    )

    header = struct.pack(
        "<7I11I8I5I",
        header_size,
        flags,
        height,
        width,
        pitch,
        1,
        mip_count,
        *reserved1,
        *pixel_format,
        caps,
        0,
        0,
        0,
        0,
    )
    assert len(header) == 124

    body = bytearray()
    for level in levels:
        body.extend(level.transpose(Image.FLIP_TOP_BOTTOM).tobytes("raw", "RGBA"))

    out_path.write_bytes(struct.pack("<I", DDS_MAGIC) + header + bytes(body))


def main() -> int:
    parser = argparse.ArgumentParser(description="Image -> RGBA8 DDS")
    parser.add_argument("input_image", type=Path)
    parser.add_argument("output_dds", type=Path)
    parser.add_argument(
        "--mipmaps",
        type=int,
        default=None,
        metavar="N",
        help="Max mip levels (default: full chain; use 1 for single mip)",
    )
    args = parser.parse_args()

    if not args.input_image.is_file():
        print(f"Input not found: {args.input_image}", file=sys.stderr)
        return 1

    max_levels = args.mipmaps
    if max_levels == 1:
        max_levels = 1
    elif max_levels is not None and max_levels < 1:
        print("--mipmaps must be >= 1", file=sys.stderr)
        return 1

    try:
        with Image.open(args.input_image) as img:
            levels = build_mip_chain(img, max_levels)
            write_dds_rgba8(levels, args.output_dds)
    except OSError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    print(
        f"Wrote {args.output_dds} "
        f"({levels[0].width}x{levels[0].height}, {len(levels)} mip(s))"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
