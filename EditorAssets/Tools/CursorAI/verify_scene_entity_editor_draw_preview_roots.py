#!/usr/bin/env python3
"""プレビュー描画が DrawSceneDocumentEntities（ルート＋孤立子）経由であることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    runtime_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if runtime_cpp.is_file():
        text = runtime_cpp.read_text(encoding="utf-8", errors="replace")
        if "DrawSceneDocumentEntities" not in text:
            errors.append("EditorSceneRuntime missing DrawSceneDocumentEntities")
        if "InitializeSceneDocumentEntities" not in text:
            errors.append("EditorSceneRuntime missing InitializeSceneDocumentEntities")
        draw_fn = text.split("void DrawSceneDocumentEntities", 1)
        if len(draw_fn) > 1:
            body = draw_fn[1][:600]
            if "GetParent()" not in body:
                errors.append("DrawSceneDocumentEntities must handle parent hierarchy")
    else:
        errors.append("missing EditorSceneRuntime.cpp")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        prev = text.split("void SceneEntityEditor::DrawPreviewEntities", 1)
        if len(prev) > 1:
            body = prev[1].split("}", 1)[0]
            if "DrawSceneDocumentEntities" not in body:
                errors.append("DrawPreviewEntities must delegate to DrawSceneDocumentEntities")
            if "entity->Draw()" in body:
                errors.append("DrawPreviewEntities must not call entity->Draw inline")

    if errors:
        print("verify_scene_entity_editor_draw_preview_roots: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_draw_preview_roots: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())