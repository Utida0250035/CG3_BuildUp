#!/usr/bin/env python3
"""EntityEditor からライト・ポスト効果を Authoring 調整できる導線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
PREVIEW_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneEntityEditorAuthoringLightingPreview.cpp"
)
WORKBENCH = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"
GAME_ORDER = ROOT / "Project" / "Codes" / "Include" / "Scene" / "GameOrder.h"


def main() -> int:
    errors: list[str] = []

    if not PREVIEW_CPP.is_file():
        errors.append("missing SceneEntityEditorAuthoringLightingPreview.cpp")
    else:
        text = PREVIEW_CPP.read_text(encoding="utf-8", errors="replace")
        if "ApplySceneGameLightingFromProfile" not in text:
            errors.append("preview must apply game_scene_lighting profile")

    if SCENE_CPP.is_file():
        scene = SCENE_CPP.read_text(encoding="utf-8", errors="replace")
        if "ApplyOnEnterEntityEditor" not in scene:
            errors.append("SceneEntityEditor::EnterScene must apply lighting profile")
        if "FlushDirectionalLightConstantBuffer" not in scene:
            errors.append("SetupEditorLighting must flush directional CB for post FX")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if WORKBENCH.is_file():
        wb = WORKBENCH.read_text(encoding="utf-8", errors="replace")
        if "SetLightingEffectsDebugVisible" not in wb:
            errors.append("EntityEditor エディタ menu must open lighting panel")
    else:
        errors.append("missing EditorEntityWorkbench.cpp")

    if GAME_ORDER.is_file():
        go = GAME_ORDER.read_text(encoding="utf-8", errors="replace")
        if "DrawLightingEffectsDebugWindow" not in go:
            errors.append("GameOrder must draw lighting effects debug window")
    else:
        errors.append("missing GameOrder.h")

    if errors:
        print("verify_editor_atrum_engine_visual_authoring: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_atrum_engine_visual_authoring: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())