#!/usr/bin/env python3
"""PlayRequest::Set 後に BuildUp を予約できることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    helper = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "Entity"
        / "EditorScenePlayRequestBuildUpQueue.cpp"
    )
    ui_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"

    if not helper.is_file():
        errors.append("missing EditorScenePlayRequestBuildUpQueue.cpp")

    if helper.is_file():
        text = helper.read_text(encoding="utf-8", errors="replace")
        if "TrySetRepositoryStartupScene" not in text:
            errors.append("expected TrySetRepositoryStartupScene before BuildUp")
        if "requestLaunchSceneBuildUp" not in text:
            errors.append("expected requestLaunchSceneBuildUp")

    if ui_cpp.is_file():
        text = ui_cpp.read_text(encoding="utf-8", errors="replace")
        if "TrySetStartupSceneAndQueueBuildUp" not in text:
            errors.append("EntityEditorUi: expected set+buildup button wiring")

    if errors:
        print("verify_editor_scene_play_request_buildup: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_play_request_buildup: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())