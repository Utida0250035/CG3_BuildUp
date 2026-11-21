#!/usr/bin/env python3
"""エディタメニューからライト・ポスト調整ウィンドウを開く導線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
WORKBENCH = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"
LIGHT_POST = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorEntityWorkbenchLightPostWindow.cpp"
)
DEBUG_IMG = ROOT / "Project" / "Codes" / "Source" / "ForDebug" / "LightingEffectsDebugImGui.cpp"


def main() -> int:
    errors: list[str] = []

    if not LIGHT_POST.is_file():
        errors.append("missing EditorEntityWorkbenchLightPostWindow.cpp")
    else:
        text = LIGHT_POST.read_text(encoding="utf-8", errors="replace")
        if "SetLightingEffectsDebugVisible" not in text:
            errors.append("OpenFromMenu must show lighting window")
        if "SetWindowFocus" not in text:
            errors.append("OpenFromMenu must focus window")

    if WORKBENCH.is_file():
        wb = WORKBENCH.read_text(encoding="utf-8", errors="replace")
        if "ライト・ポスト調整" not in wb:
            errors.append('menu must include "ライト・ポスト調整"')
        if "EditorEntityWorkbenchLightPostWindow::OpenFromMenu" not in wb:
            errors.append("menu must call OpenFromMenu")
        if "EditorEntityWorkbenchLightPostWindow::WindowTitle" not in wb:
            errors.append("dock layout must include light/post window")
    else:
        errors.append("missing EditorEntityWorkbench.cpp")

    if DEBUG_IMG.is_file():
        if "ライト・ポスト調整" not in DEBUG_IMG.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("debug window title must be ライト・ポスト調整")
    else:
        errors.append("missing LightingEffectsDebugImGui.cpp")

    if errors:
        print("verify_editor_entity_workbench_light_post_menu: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_entity_workbench_light_post_menu: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())