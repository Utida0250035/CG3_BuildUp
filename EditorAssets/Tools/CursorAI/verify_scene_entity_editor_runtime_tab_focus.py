#!/usr/bin/env python3
"""エディタ内プレイ後の Runtime タブフォーカスのみを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
    focus_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneEntityEditorRuntimeTabFocus.h"

    if not focus_h.is_file():
        errors.append("missing SceneEntityEditorRuntimeTabFocus.h")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "ApplyInEditorPlayRuntimeTabFocusOnly" not in text:
            errors.append("SceneEntityEditor: expected ApplyInEditorPlayRuntimeTabFocusOnly")
        block_start = text.find("requestPlayRuntimeScene")
        if block_start >= 0:
            block = text[block_start : block_start + 450]
            if "previewSimulationEnabled = true" in block:
                errors.append(
                    "requestPlayRuntimeScene must not auto-enable previewSimulation"
                )
            if "runtimeFunctionalViewDraw = true" in block:
                errors.append(
                    "requestPlayRuntimeScene must not auto-enable functional draw"
                )
            if "ApplyFramingFromSceneDocument" in block:
                errors.append(
                    "requestPlayRuntimeScene must not apply stage camera framing"
                )

    if errors:
        print("verify_scene_entity_editor_runtime_tab_focus: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_runtime_tab_focus: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())