#!/usr/bin/env python3
"""Edit ホバー時のカメラ更新が CmpCamera 経由であることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    draw_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorEditViewDebugCameraDraw.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if not draw_cpp.is_file():
        errors.append("missing SceneEntityEditorEditViewDebugCameraDraw.cpp")
    else:
        text = draw_cpp.read_text(encoding="utf-8", errors="replace")
        if "EditViewHovered" not in text:
            errors.append("edit hover draw must check EditViewHovered")
        if "PrepareBeforeDraw(true)" not in text:
            errors.append("edit hover must use PrepareBeforeDraw(true)")
        if "camera->Update()" in text:
            errors.append("edit hover module must not call camera->Update() directly")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        draw_fn = text.split("void SceneEntityEditor::Draw()", 1)
        if len(draw_fn) > 1:
            body = draw_fn[1].split("void SceneEntityEditor::", 1)[0]
            if "ApplyHoveredEditViewCameraInput" not in body:
                errors.append("Draw must call ApplyHoveredEditViewCameraInput")
            if "camera->Update()" in body:
                errors.append("Draw must not call camera->Update() directly")

    if errors:
        print("verify_scene_entity_editor_edit_hover_camera: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_edit_hover_camera: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())