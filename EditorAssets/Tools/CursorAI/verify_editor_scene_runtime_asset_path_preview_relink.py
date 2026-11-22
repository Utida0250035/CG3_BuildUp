#!/usr/bin/env python3
"""AssetPath プレビューが構築後・Initialize 後に再リンクされるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
RELINK = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntimeEditorAssetPathPreviewRelink.cpp"
)
SCENE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
NOTIFY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorAuthoringEditorAssetPathEditNotify.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not RELINK.is_file():
        errors.append("missing EditorSceneRuntimeEditorAssetPathPreviewRelink.cpp")
    else:
        text = RELINK.read_text(encoding="utf-8", errors="replace")
        if "EditorSceneRuntimeEditorAssetPathPreviewLink::Apply" not in text:
            errors.append("relink must delegate to preview link")

    if RUNTIME.is_file():
        rt = RUNTIME.read_text(encoding="utf-8", errors="replace")
        if "RelinkEditorAssetPathPreviewsInScene" not in rt:
            errors.append("BuildEntitiesFromSceneDocument must relink after build")
        if rt.find("InitializeSceneDocumentEntities") > rt.find(
            "RelinkEditorAssetPathPreviewsInScene"
        ):
            errors.append("relink must run after InitializeSceneDocumentEntities")
    else:
        errors.append("missing EditorSceneRuntime.cpp")

    if SCENE.is_file():
        if "RelinkPreviewEditorAssetPathsAfterRebuild" not in SCENE.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("SceneEntityEditor must relink after preview rebuild")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if NOTIFY.is_file():
        if "NotifyPreviewOnly" not in NOTIFY.read_text(encoding="utf-8", errors="replace"):
            errors.append("asset path edit notify must queue preview only")
    else:
        errors.append("missing EntityEditorAuthoringEditorAssetPathEditNotify.cpp")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "NotifyEditorAssetPathPreviewRefresh" not in ui:
            errors.append("inspector must use asset path preview refresh notify")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print(
            "verify_editor_scene_runtime_asset_path_preview_relink: FAILED",
            file=sys.stderr,
        )
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_asset_path_preview_relink: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())