#!/usr/bin/env python3
"""シーンビュー RT が GameOrder 後に ActiveCameraSystem 経由で再バインドすることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    bind_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorSceneViewRenderCameraBind.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
    active_cpp = ROOT / "Project" / "Codes" / "Source" / "Camera" / "ActiveCameraSystem.cpp"

    if not bind_cpp.is_file():
        errors.append("missing SceneEntityEditorSceneViewRenderCameraBind.cpp")
    else:
        text = bind_cpp.read_text(encoding="utf-8", errors="replace")
        if "SyncEngineAfterSceneViewFit" not in text:
            errors.append("scene view bind must use SyncEngineAfterSceneViewFit")
        if "GetActiveCamera" not in text:
            errors.append("scene view bind must prefer GameOrder active camera")

    if active_cpp.is_file():
        text = active_cpp.read_text(encoding="utf-8", errors="replace")
        if "SyncEngineAfterSceneViewFit" not in text:
            errors.append("ActiveCameraSystem missing SyncEngineAfterSceneViewFit")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        fn = text.split("void SceneEntityEditor::DrawSceneViewContent", 1)
        if len(fn) > 1:
            body = fn[1].split("void SceneEntityEditor::", 1)[0]
            if "BindForSceneViewRender" not in body:
                errors.append("DrawSceneViewContent must call BindForSceneViewRender")
            if "engine->SetCamera" in body:
                errors.append("DrawSceneViewContent must not call engine->SetCamera directly")

    if errors:
        print("verify_scene_entity_editor_scene_view_rt_camera: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_scene_view_rt_camera: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())