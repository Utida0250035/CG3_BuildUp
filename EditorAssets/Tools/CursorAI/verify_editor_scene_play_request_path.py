#!/usr/bin/env python3
"""EditorScenePlayRequest::Path が起動・BuildUp で消費されることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"


def main() -> int:
    errors: list[str] = []
    asset_h = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Scene" / "EditorSceneAsset.h"

    if asset_h.is_file():
        text = asset_h.read_text(encoding="utf-8", errors="replace")
        if "TakePendingEditorPlayScenePath" not in text:
            errors.append("EditorSceneAsset.h: expected TakePendingEditorPlayScenePath")

    boot_cpp = CODES / "Source" / "Scene" / "CommandSceneDevelopmentBoot.cpp"
    entity_cpp = CODES / "Source" / "Scene" / "SceneEntityEditor.cpp"
    ui_cpp = CODES / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"

    for path, needle in (
        (boot_cpp, "TakePendingEditorPlayScenePath"),
        (entity_cpp, "TakePendingEditorPlayScenePath"),
        (ui_cpp, "StageEditorPlayScenePath"),
    ):
        if not path.is_file():
            errors.append(f"missing {path.relative_to(ROOT)}")
            continue
        if needle not in path.read_text(encoding="utf-8", errors="replace"):
            errors.append(f"{path.relative_to(ROOT)}: expected {needle}")

    if errors:
        print("verify_editor_scene_play_request_path: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_play_request_path: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())