#!/usr/bin/env python3
"""BuildEntitiesFromSceneDocument が MainCamera 補完を呼ぶか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
SUPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntimeMainCameraSupplement.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not SUPP.is_file():
        errors.append("missing EditorSceneRuntimeMainCameraSupplement.cpp")
    else:
        text = SUPP.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "ResolvePrimaryCmpCamera",
            "TryMainCameraFromDefaultPrefab",
            "MainCameraEntity",
        ):
            if needle not in text:
                errors.append(f"supplement missing: {needle}")

    if RUNTIME.is_file():
        rt = RUNTIME.read_text(encoding="utf-8", errors="replace")
        if "EditorSceneRuntimeMainCameraSupplement::EnsureInEntityList" not in rt:
            errors.append("BuildEntitiesFromSceneDocument must call EnsureInEntityList")
    else:
        errors.append("missing EditorSceneRuntime.cpp")

    if errors:
        print("verify_editor_scene_runtime_main_camera_supplement: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_main_camera_supplement: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())