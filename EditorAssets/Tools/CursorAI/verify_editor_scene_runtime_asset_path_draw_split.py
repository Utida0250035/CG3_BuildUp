#!/usr/bin/env python3
"""CmpEditorAssetPath が Draw を暗黙追加しない分割プレビュー経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
LINK = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntimeEditorAssetPathPreviewLink.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not LINK.is_file():
        errors.append("missing EditorSceneRuntimeEditorAssetPathPreviewLink.cpp")
    else:
        link = LINK.read_text(encoding="utf-8", errors="replace")
        if "AddDrawCmp" in link:
            errors.append("AssetPath link must not AddDrawCmp")
        if "自動追加しません" not in link:
            errors.append("expected warning when Draw slot missing")

    if RUNTIME.is_file():
        rt = RUNTIME.read_text(encoding="utf-8", errors="replace")
        if "TryLoadEditorAssetPathPreview" in rt:
            errors.append("Remove legacy TryLoadEditorAssetPathPreview from runtime")
        if "EditorSceneRuntimeEditorAssetPathPreviewLink::Apply" not in rt:
            errors.append("BuildEntityFromDefinition must call AssetPath preview link")
    else:
        errors.append("missing EditorSceneRuntime.cpp")

    if errors:
        print("verify_editor_scene_runtime_asset_path_draw_split: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_asset_path_draw_split: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())