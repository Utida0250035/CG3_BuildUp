#!/usr/bin/env python3
"""Static check: gameplay scenes lock stage view camera (policy doc)."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
INCLUDE = ROOT / "Project" / "Codes" / "Include" / "Scene"
SOURCE = ROOT / "Project" / "Codes" / "Source" / "Scene"

LOCKED_REQUIRED = (
    "SceneGame.h",
    "SceneBuildUp.h",
    "SceneTitle.h",
    "SceneResult.h",
    "SceneMovie.h",
)

UNLOCKED_EDITOR = (
    "SceneEntityEditor.h",
    "SceneParticleEditor.h",
)


def header_path(name: str) -> Path:
    return INCLUDE / name


def has_locked_true(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    return bool(
        re.search(
            r"IsStageViewCameraLocked\s*\(\)\s*const\s*override\s*\{\s*return\s*true",
            text,
        )
    )


def has_locked_false(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    return bool(
        re.search(
            r"IsStageViewCameraLocked\s*\(\)\s*const\s*override\s*\{\s*return\s*false",
            text,
        )
    )


def gameplay_header_allows_debug_in_header(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    return bool(
        re.search(
            r"AllowDebugCameraControl\s*\(\)\s*const\s*override\s*\{\s*return\s*true",
            text,
        )
    )


def entity_editor_declares_debug_override(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    return "AllowDebugCameraControl() const override" in text


def entity_editor_cpp_defers_debug_input(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    if "AllowDebugCameraControl" not in text:
        return False
    return bool(
        re.search(
            r"#if\s+defined\(DEVELOPMENT\)\s+&&\s+defined\(USE_IMGUI\)"
            r"[\s\S]*?return\s+false",
            text,
        )
    )


def main() -> int:
    errors: list[str] = []
    for name in LOCKED_REQUIRED:
        path = header_path(name)
        if not path.is_file():
            errors.append(f"missing header: {name}")
            continue
        if not has_locked_true(path):
            errors.append(f"{name}: expected IsStageViewCameraLocked() -> true")
        if gameplay_header_allows_debug_in_header(path):
            errors.append(
                f"{name}: gameplay must not AllowDebugCameraControl() -> true in header"
            )

    for name in UNLOCKED_EDITOR:
        path = header_path(name)
        if not path.is_file():
            errors.append(f"missing header: {name}")
            continue
        if not has_locked_false(path):
            errors.append(f"{name}: expected IsStageViewCameraLocked() -> false")

    entity_header = header_path("SceneEntityEditor.h")
    if entity_header.is_file() and not entity_editor_declares_debug_override(
        entity_header
    ):
        errors.append(
            "SceneEntityEditor.h: expected AllowDebugCameraControl() override"
        )

    entity_cpp = SOURCE / "SceneEntityEditor.cpp"
    if not entity_cpp.is_file():
        errors.append("missing SceneEntityEditor.cpp")
    elif not entity_editor_cpp_defers_debug_input(entity_cpp):
        errors.append(
            "SceneEntityEditor.cpp: expected DEVELOPMENT+USE_IMGUI AllowDebugCameraControl -> false"
        )

    if errors:
        print("verify_camera_policy: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_camera_policy: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())