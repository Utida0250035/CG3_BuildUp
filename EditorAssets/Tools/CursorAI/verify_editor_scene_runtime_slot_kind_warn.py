#!/usr/bin/env python3
"""EditorSceneRuntime がスロット種別不一致を警告する導線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntime.cpp"
)
WARN_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntimeBuildWarnings.cpp"
)
EDITOR_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not WARN_CPP.is_file():
        errors.append("missing EditorSceneRuntimeBuildWarnings.cpp")
    else:
        text = WARN_CPP.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "RecordSkippedComponentSlot",
            "DefaultKindForTypeName",
            "スロット種別の不一致",
        ):
            if needle not in text:
                errors.append(f"warnings module must include {needle!r}")

    if not RUNTIME_CPP.is_file():
        errors.append("missing EditorSceneRuntime.cpp")
    else:
        rt = RUNTIME_CPP.read_text(encoding="utf-8", errors="replace")
        if "RecordSkippedComponentSlot" not in rt:
            errors.append("BuildEntityFromDefinition must record skipped slots")
        if "TryAddEmptyComponent(*entity, slot)) {" not in rt:
            errors.append("expected TryAddEmptyComponent failure path")

    if not EDITOR_CPP.is_file():
        errors.append("missing EntityEditor.cpp")
    else:
        ed = EDITOR_CPP.read_text(encoding="utf-8", errors="replace")
        if "importStatusMessage" not in ed.split("BuildPreviewScene", 1)[-1][:600]:
            errors.append("BuildPreviewScene must surface warnings to importStatusMessage")

    if errors:
        print("verify_editor_scene_runtime_slot_kind_warn: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_slot_kind_warn: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())