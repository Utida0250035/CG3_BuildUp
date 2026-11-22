#!/usr/bin/env python3
"""メニュー「プレイ」が保存＋実行キューを呼ぶか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PLAY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorEntityWorkbenchPlayMenu.cpp"
)
WORKBENCH = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"


def main() -> int:
    errors: list[str] = []

    if not PLAY.is_file():
        errors.append("missing EditorEntityWorkbenchPlayMenu.cpp")
    else:
        text = PLAY.read_text(encoding="utf-8", errors="replace")
        if 'BeginMenu("プレイ")' not in text:
            errors.append('must define "プレイ" menu')
        if "QueueInEditorPlayRuntimeTabOnly" not in text:
            errors.append("play menu must queue in-editor runtime after save")
        if "QueueEditorScenePlayViaBuildUp" not in text:
            errors.append("play menu must queue BuildUp play after save")

    if WORKBENCH.is_file():
        if "EditorEntityWorkbenchPlayMenu::DrawMenuItems" not in WORKBENCH.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("workbench must call DrawMenuItems for play menu")
    else:
        errors.append("missing EditorEntityWorkbench.cpp")

    if errors:
        print("verify_editor_entity_workbench_play_menu: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_entity_workbench_play_menu: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())