#!/usr/bin/env python3
"""シーンを実行が SceneBuildUp 経路を使うことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    route_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneEntityEditorScenePlayBuildUpRoute.h"
    ui_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"

    if not route_h.is_file():
        errors.append("missing SceneEntityEditorScenePlayBuildUpRoute.h")

    if ui_cpp.is_file():
        text = ui_cpp.read_text(encoding="utf-8", errors="replace")
        if "QueueEditorScenePlayViaBuildUp" not in text:
            errors.append("EntityEditorUi: expected QueueEditorScenePlayViaBuildUp")
        # 「シーンを実行」ボタン直後が BuildUp 経路であること
        marker = 'if (ImGui::Button("シーンを実行"))'
        idx = text.find(marker)
        if idx < 0:
            errors.append('EntityEditorUi: missing "シーンを実行" button')
        else:
            chunk = text[idx : idx + 200]
            if "requestPlayRuntimeScene" in chunk:
                errors.append(
                    "EntityEditorUi: シーンを実行 must not use requestPlayRuntimeScene"
                )
            if "QueueEditorScenePlayViaBuildUp" not in chunk:
                errors.append(
                    "EntityEditorUi: シーンを実行 must call QueueEditorScenePlayViaBuildUp"
                )

    if errors:
        print("verify_scene_entity_editor_buildup_play: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_buildup_play: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())