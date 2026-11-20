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

SCENE_SUBCLASS_RE = re.compile(
    r"class\s+Scene\w+\s*(?:final\s*)?:\s*public\s+Scene\b"
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


def discover_scene_implementation_headers() -> list[str]:
    found: list[str] = []
    for path in sorted(INCLUDE.glob("Scene*.h")):
        text = path.read_text(encoding="utf-8", errors="replace")
        if SCENE_SUBCLASS_RE.search(text):
            found.append(path.name)
    return found


def scene_base_defaults_locked_true() -> bool:
    path = INCLUDE / "Scene.h"
    if not path.is_file():
        return False
    text = path.read_text(encoding="utf-8", errors="replace")
    defaults_path = INCLUDE / "SceneStageCameraLockDefaults.h"
    defaults_ok = defaults_path.is_file() and bool(
        re.search(
            r"kDefaultIsStageViewCameraLocked\s*=\s*true",
            defaults_path.read_text(encoding="utf-8", errors="replace"),
        )
    )
    return (
        "kDefaultIsStageViewCameraLocked" in text
        and defaults_ok
        and bool(
            re.search(
                r"IsStageViewCameraLocked\s*\(\)\s*const\s*\{[\s\S]*?return\s+"
                r"SceneStageCameraLockDefaults::kDefaultIsStageViewCameraLocked",
                text,
            )
        )
    )


def particle_editor_declares_debug_override(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    return "AllowDebugCameraControl() const override" in text


def particle_editor_cpp_scoped_debug_allow(path: Path) -> bool:
    text = path.read_text(encoding="utf-8", errors="replace")
    if "AllowDebugCameraControl" not in text:
        return False
    return bool(
        re.search(
            r"bool\s+SceneParticleEditor::AllowDebugCameraControl\s*\(\)"
            r"[\s\S]*?AllowDebugCameraControlInParticleEditorScene\s*\(",
            text,
        )
    )


def particle_editor_policy_limits_to_dev_imgui(path: Path) -> bool:
    if not path.is_file():
        return False
    text = path.read_text(encoding="utf-8", errors="replace")
    return bool(
        re.search(
            r"#if\s+defined\(DEVELOPMENT\)\s+&&\s+defined\(USE_IMGUI\)"
            r"[\s\S]*?return\s+true"
            r"[\s\S]*?#else"
            r"[\s\S]*?return\s+false",
            text,
        )
    )


def buildup_lock_is_runtime_toggleable() -> bool:
    header = header_path("SceneBuildUp.h")
    cpp = SOURCE / "SceneBuildUp.cpp"
    if not header.is_file() or not cpp.is_file():
        return False
    if has_locked_true(header):
        return False
    text = cpp.read_text(encoding="utf-8", errors="replace")
    return (
        "SceneBuildUp::IsStageViewCameraLocked" in text
        and "stageViewCameraLocked_" in header.read_text(encoding="utf-8", errors="replace")
    )


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
        if name == "SceneBuildUp.h":
            if not buildup_lock_is_runtime_toggleable():
                errors.append(
                    "SceneBuildUp: expected runtime toggleable StageView lock "
                    "(default locked via stageViewCameraLocked_)"
                )
            text = path.read_text(encoding="utf-8", errors="replace")
            if "AllowDebugCameraControl() const override" in text:
                errors.append(
                    "SceneBuildUp: must not override AllowDebugCameraControl "
                    "(inherit Scene default false)"
                )
            if gameplay_header_allows_debug_in_header(path):
                errors.append(
                    "SceneBuildUp: gameplay must not AllowDebugCameraControl() -> true"
                )
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

    particle_header = header_path("SceneParticleEditor.h")
    if particle_header.is_file() and not particle_editor_declares_debug_override(
        particle_header
    ):
        errors.append(
            "SceneParticleEditor.h: expected AllowDebugCameraControl() override"
        )

    particle_cpp = SOURCE / "SceneParticleEditor.cpp"
    if not particle_cpp.is_file():
        errors.append("missing SceneParticleEditor.cpp")
    elif not particle_editor_cpp_scoped_debug_allow(particle_cpp):
        errors.append(
            "SceneParticleEditor.cpp: expected AllowDebugCameraControl via "
            "AllowDebugCameraControlInParticleEditorScene"
        )

    particle_policy = SOURCE / "SceneParticleEditorAllowDebugPolicy.cpp"
    if not particle_policy.is_file():
        errors.append("missing SceneParticleEditorAllowDebugPolicy.cpp")
    elif not particle_editor_policy_limits_to_dev_imgui(particle_policy):
        errors.append(
            "SceneParticleEditorAllowDebugPolicy.cpp: expected "
            "DEVELOPMENT+USE_IMGUI -> true, else -> false"
        )

    if not scene_base_defaults_locked_true():
        errors.append(
            "Scene.h: expected IsStageViewCameraLocked() default via "
            "SceneStageCameraLockDefaults::kDefaultIsStageViewCameraLocked (true)"
        )

    known_scenes = set(LOCKED_REQUIRED) | set(UNLOCKED_EDITOR)
    for name in discover_scene_implementation_headers():
        if name not in known_scenes:
            errors.append(
                f"{name}: new Scene subclass; register in verify_camera_policy "
                "LOCKED_REQUIRED or UNLOCKED_EDITOR"
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