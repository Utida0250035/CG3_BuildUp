#!/usr/bin/env python3
"""実行ビューの BuildUp 相当描画パスが配線されているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    draw_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "EditorSceneRuntimeFunctionalViewDraw.cpp"
    )
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
    ui_h = (
        ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"
    )

    if not draw_cpp.is_file():
        errors.append("missing EditorSceneRuntimeFunctionalViewDraw.cpp")

    if not ui_h.is_file():
        errors.append("missing EntityEditor.h")
    elif "runtimeFunctionalViewDraw" not in ui_h.read_text(encoding="utf-8", errors="replace"):
        errors.append("EntityEditor UiState: expected runtimeFunctionalViewDraw")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "DrawEditorSceneRuntimeFunctionalView" not in text:
            errors.append("SceneEntityEditor: expected functional runtime view draw")

    if errors:
        print("verify_editor_scene_view_functional: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_view_functional: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())