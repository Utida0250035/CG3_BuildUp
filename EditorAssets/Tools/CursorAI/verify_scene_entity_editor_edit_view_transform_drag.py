#!/usr/bin/env python3
"""エディットビューの配置ドラッグ／軸ギズモ導線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
DRAG_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorEditViewTransformDrag.cpp"
GIZMO_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneEntityEditorEditViewTransformGizmoDraw.cpp"
)
RAY_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEditViewScreenRay.cpp"
FRAME_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorFrame.cpp"


def main() -> int:
    errors: list[str] = []

    for path, label in (
        (DRAG_CPP, "drag"),
        (GIZMO_CPP, "gizmo"),
        (RAY_CPP, "screen ray"),
    ):
        if not path.is_file():
            errors.append(f"missing {path.name}")
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if label == "drag":
            for needle in (
                "TryPickHorizontalPlaneY",
                "MarkSceneDocumentDirty",
                "TryMapMouseToEditViewLocal",
            ):
                if needle not in text:
                    errors.append(f"drag: expected {needle}")
        if label == "gizmo" and "DrawAxisLine" not in text:
            errors.append("gizmo: expected axis draw")
        if label == "screen ray" and "TryPickHorizontalPlaneY" not in text:
            errors.append("ray: expected plane pick")

    if SCENE_CPP.is_file():
        scene = SCENE_CPP.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityEditorEditViewTransformDrag::Tick" not in scene:
            errors.append("SceneEntityEditor must tick edit view drag")
        if "SceneEntityEditorEditViewTransformGizmoDraw::Draw" not in scene:
            errors.append("SceneEntityEditor must draw transform gizmo")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if FRAME_CPP.is_file():
        frame = FRAME_CPP.read_text(encoding="utf-8", errors="replace")
        if "TryMapMouseToEditViewLocal" not in frame:
            errors.append("EditorFrame must map edit view mouse")
    else:
        errors.append("missing EditorFrame.cpp")

    if errors:
        print("verify_scene_entity_editor_edit_view_transform_drag: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_edit_view_transform_drag: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())