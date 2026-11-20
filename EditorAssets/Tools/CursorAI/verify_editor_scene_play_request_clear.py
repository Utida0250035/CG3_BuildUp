#!/usr/bin/env python3
"""EditorScenePlayRequest::Clear が消費・破棄経路で呼ばれることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    asset_h = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Scene" / "EditorSceneAsset.h"
    asset_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"

    if asset_h.is_file():
        text = asset_h.read_text(encoding="utf-8", errors="replace")
        if "ClearStagedEditorPlayScenePath" not in text:
            errors.append("EditorSceneAsset.h: expected ClearStagedEditorPlayScenePath")

    if asset_cpp.is_file():
        text = asset_cpp.read_text(encoding="utf-8", errors="replace")
        if "TakePendingEditorPlayScenePath" not in text:
            errors.append("TakePending must consume via Clear")
        elif "EditorScenePlayRequest::Instance().Clear()" not in text:
            errors.append("EditorSceneAsset.cpp: expected Clear() in consume/discard")

    for rel in (
        "Project/Codes/Source/Scene/SceneEntityEditor.cpp",
        "Project/Codes/Source/Scene/SceneBuildUp.cpp",
    ):
        path = ROOT / rel
        if path.is_file() and "ClearStagedEditorPlayScenePath" not in path.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append(f"{rel}: expected ClearStagedEditorPlayScenePath on exit")

    if errors:
        print("verify_editor_scene_play_request_clear: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_play_request_clear: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())