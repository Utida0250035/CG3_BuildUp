#!/usr/bin/env python3
"""Release から ForDebug 専用 TU が除外されているか vcxproj を検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
ALLOWLIST = (
    ROOT
    / "EditorAssets"
    / "Tools"
    / "CursorAI"
    / "for_debug_sources_release_link_allowlist.txt"
)
RELEASE_MARK = "Release|x64"
FOR_DEBUG_PREFIX = "Codes/Source/ForDebug/"


def normalize_path(path: str) -> str:
    return path.replace("\\", "/")


def load_allowlist() -> set[str]:
    paths: set[str] = set()
    for line in ALLOWLIST.read_text(encoding="utf-8", errors="replace").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        paths.add(line.replace("/", "\\"))
    return paths


def is_target(path: str, allowed: set[str]) -> bool:
    normalized = normalize_path(path)
    allowed_norm = {normalize_path(p) for p in allowed}
    if normalized in allowed_norm:
        return False
    return normalized.startswith(FOR_DEBUG_PREFIX)


def main() -> int:
    errors: list[str] = []
    if not VCPROJ.is_file():
        errors.append("missing CG2_BuildUp.vcxproj")
    if not ALLOWLIST.is_file():
        errors.append("missing for_debug_sources_release_link_allowlist.txt")

    if VCPROJ.is_file():
        allowed = load_allowlist() if ALLOWLIST.is_file() else set()
        text = VCPROJ.read_text(encoding="utf-8", errors="replace")
        pattern = re.compile(
            r'<ClCompile Include="([^"]+)"([^/]*)(?:/>|>(.*?)</ClCompile>)',
            re.DOTALL,
        )
        for match in pattern.finditer(text):
            path = match.group(1)
            if not is_target(path, allowed):
                continue
            block = match.group(0)
            if RELEASE_MARK not in block or "ExcludedFromBuild" not in block:
                errors.append(f"missing Release exclude: {path}")

    if errors:
        print("verify_for_debug_sources_release_exclude: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_for_debug_sources_release_exclude: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())