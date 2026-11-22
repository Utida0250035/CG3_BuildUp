#!/usr/bin/env python3
"""Runtime タブ描画が SetupStageViewCamera に差し替えないことを検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    draw_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorRuntimeViewCameraDraw.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if not draw_cpp.is_file():
        errors.append("missing SceneEntityEditorRuntimeViewCameraDraw.cpp")
    else:
        draw_text = draw_cpp.read_text(encoding="utf-8", errors="replace")
        if "SetupStageViewCamera" in draw_text:
            errors.append("runtime draw module must not call SetupStageViewCamera")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        fn = text.split("void SceneEntityEditor::DrawSceneViewContent", 1)
        if len(fn) < 2:
            errors.append("DrawSceneViewContent not found")
        else:
            body = fn[1]
            end = body.find("void SceneEntityEditor::")
            if end < 0:
                end = len(body)
            body = body[:end]
            if "PrepareSceneViewCameraForDraw" not in body:
                errors.append("DrawSceneViewContent must call PrepareSceneViewCameraForDraw")
            runtime_blocks = re.findall(
                r"SceneViewTab::Runtime[\s\S]{0,400}",
                body,
            )
            for block in runtime_blocks:
                if "SetupStageViewCamera" in block:
                    errors.append(
                        "Runtime tab path must not call SetupStageViewCamera"
                    )
                    break

    if errors:
        print("verify_scene_entity_editor_runtime_stage_camera: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_runtime_stage_camera: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())