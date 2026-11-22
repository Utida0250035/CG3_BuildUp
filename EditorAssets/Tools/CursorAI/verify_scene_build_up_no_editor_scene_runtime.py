#!/usr/bin/env python3
"""SceneBuildUp 本体が EditorSceneRuntime.h を直接 include しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes" / "Source" / "Scene"
LOAD_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpSceneDocumentLoad.cpp"

DIRECT_TUS = (
    "SceneBuildUp.cpp",
    "SceneBuildUpStageCameraFraming.cpp",
)


def main() -> int:
    errors: list[str] = []

    for name in DIRECT_TUS:
        path = CODES / name
        if not path.is_file():
            errors.append(f"missing {name}")
            continue
        if "EditorSceneRuntime.h" in path.read_text(encoding="utf-8", errors="replace"):
            errors.append(f"{name} must not include EditorSceneRuntime.h")

    if not LOAD_CPP.is_file():
        errors.append("missing SceneBuildUpSceneDocumentLoad.cpp")
    elif "EditorSceneRuntime.h" not in LOAD_CPP.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("SceneBuildUpSceneDocumentLoad must include EditorSceneRuntime")

    buildup = CODES / "SceneBuildUp.cpp"
    if buildup.is_file():
        text = buildup.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUpSceneDocumentLoad::" not in text:
            errors.append("SceneBuildUp must load via SceneBuildUpSceneDocumentLoad")

    if errors:
        print("verify_scene_build_up_no_editor_scene_runtime: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_no_editor_scene_runtime: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())