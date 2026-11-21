#!/usr/bin/env python3
"""シーンビュー描画でライト・ポストが CB に反映される経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
APPLY_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneEntityEditorSceneViewVisualEffectsApply.cpp"
)
SCENE_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
PREVIEW_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneEntityEditorAuthoringLightingPreview.cpp"
)
FUNCTIONAL_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorSceneRuntimeFunctionalViewDraw.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not APPLY_CPP.is_file():
        errors.append("missing SceneEntityEditorSceneViewVisualEffectsApply.cpp")
    else:
        apply_text = APPLY_CPP.read_text(encoding="utf-8", errors="replace")
        if "FlushDirectionalLightConstantBuffer" not in apply_text:
            errors.append("ApplyBeforeDraw must flush directional light CB")

    if PREVIEW_CPP.is_file():
        if "ApplySceneGameLightingFromProfile" not in PREVIEW_CPP.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("enter preview must load game_scene_lighting profile")
    else:
        errors.append("missing SceneEntityEditorAuthoringLightingPreview.cpp")

    if SCENE_CPP.is_file():
        scene = SCENE_CPP.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityEditorSceneViewVisualEffectsApply::ApplyBeforeDraw" not in scene:
            errors.append("DrawSceneViewContent must apply visual effects before draw")
        if "ApplyOnEnterEntityEditor" not in scene:
            errors.append("EnterScene must apply lighting profile on enter")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if FUNCTIONAL_CPP.is_file():
        functional = FUNCTIONAL_CPP.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityEditorSceneViewVisualEffectsApply::ApplyBeforeDraw" not in functional:
            errors.append("functional runtime view must flush lighting CB after SetCamera")
    else:
        errors.append("missing EditorSceneRuntimeFunctionalViewDraw.cpp")

    if errors:
        print("verify_scene_entity_editor_preview_visual_effects: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_preview_visual_effects: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())