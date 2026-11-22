#!/usr/bin/env python3
"""プレハブ複数一括配置 UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BATCH = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorPrefabBatchSpawnFromFile.cpp"
)
UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetBatchSpawnUi.cpp"
)
EDITOR_UI = (
    ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not BATCH.is_file():
        errors.append("missing EntityEditorPrefabBatchSpawnFromFile.cpp")
    else:
        text = BATCH.read_text(encoding="utf-8", errors="replace")
        if "AppendFromPrefabFileBatch" not in text:
            errors.append("batch append API required")
        if "TryBuildRootInstanceFromPrefabJson" not in text:
            errors.append("batch must reuse prefab json builder")

    if not UI.is_file():
        errors.append("missing PrefabAssetBatchSpawnUi.cpp")
    else:
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "一括配置" not in ui:
            errors.append("batch spawn button required")
        if "AppendFromPrefabFileBatch" not in ui:
            errors.append("UI must call batch append")

    if EDITOR_UI.is_file():
        if "DrawBatchSpawnControls" not in EDITOR_UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("project panel must draw batch spawn controls")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_prefab_asset_batch_spawn_ui: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_asset_batch_spawn_ui: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())