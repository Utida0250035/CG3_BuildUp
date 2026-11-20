#!/usr/bin/env python3
"""エディタ内実行が Runtime タブ切替のみであり BuildUp と分離されていることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    tick_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorInEditorPlayTick.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
    queue_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorInEditorPlayQueue.cpp"

    for path in (tick_cpp, queue_cpp):
        if not path.is_file():
            errors.append(f"missing {path.name}")

    if tick_cpp.is_file():
        tick_text = tick_cpp.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUp" in tick_text or "QueueFadeToBuildUp" in tick_text:
            errors.append("InEditorPlayTick must not launch BuildUp")
        if "ApplyInEditorPlayRuntimeTabFocusOnly" not in tick_text:
            errors.append("InEditorPlayTick must focus Runtime tab only")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityEditorInEditorPlayTick::Process" not in text:
            errors.append("SceneEntityEditor must use InEditorPlayTick::Process")
        idx = text.find("requestPlayRuntimeScene")
        if idx >= 0:
            chunk = text[idx : idx + 200]
            if "QueueFadeToBuildUp" in chunk or "requestLaunchSceneBuildUp" in chunk:
                errors.append("requestPlayRuntimeScene path must not touch BuildUp")

    ui_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
    if ui_cpp.is_file():
        ui_text = ui_cpp.read_text(encoding="utf-8", errors="replace")
        preview = ui_text.split('ImGui::Button("エディタ内プレビュー")', 1)
        if len(preview) > 1:
            chunk = preview[1][:180]
            if "QueueInEditorPlayRuntimeTabOnly" not in chunk:
                errors.append("エディタ内プレビュー must use QueueInEditorPlayRuntimeTabOnly")
            if "requestLaunchSceneBuildUp" in chunk:
                errors.append("エディタ内プレビュー must not set BuildUp request")

    if errors:
        print("verify_scene_entity_editor_runtime_tab_only_play: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_runtime_tab_only_play: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())