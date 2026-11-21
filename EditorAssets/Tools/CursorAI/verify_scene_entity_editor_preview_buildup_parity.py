#!/usr/bin/env python3
"""SceneEntityEditor プレビューが SceneBuildUp と同一 BuildEntities 経路か検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PARITY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorBuildPreviewSceneBuildUpParity.cpp"
)
SCENE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
REBUILD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneEntityEditorPreviewBuildUpParityRebuild.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not PARITY.is_file():
        errors.append("missing EntityEditorBuildPreviewSceneBuildUpParity.cpp")
    else:
        text = PARITY.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUpSceneDocumentLoad::BuildEntities" not in text:
            errors.append("preview parity must call SceneBuildUpSceneDocumentLoad::BuildEntities")

    if not REBUILD.is_file():
        errors.append("missing SceneEntityEditorPreviewBuildUpParityRebuild.cpp")
    else:
        if "BuildPreviewScene" not in REBUILD.read_text(encoding="utf-8", errors="replace"):
            errors.append("rebuild helper must delegate to BuildPreviewScene")

    if SCENE.is_file():
        sc = SCENE.read_text(encoding="utf-8", errors="replace")
        if "RebuildPreviewEntitiesViaBuildUpParity" not in sc:
            errors.append("SceneEntityEditor must rebuild via BuildUp parity helper")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if errors:
        print("verify_scene_entity_editor_preview_buildup_parity: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_preview_buildup_parity: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())