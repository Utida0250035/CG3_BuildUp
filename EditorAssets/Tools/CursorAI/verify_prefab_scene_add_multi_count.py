#!/usr/bin/env python3
"""プレハブ配置が件数指定で複数体追加できるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SPAWN = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorPrefabSpawnFirstInstanceRoot.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
BATCH_UI = (
    ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Prefab" / "PrefabAssetBatchSpawnUi.cpp"
)
EDITOR_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"


def main() -> int:
    errors: list[str] = []

    if SPAWN.is_file():
        if "AppendFromPrefabFileBatch" not in SPAWN.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("single append must delegate to batch API")
    else:
        errors.append("missing prefab spawn root")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "prefabSpawnBatchCount" not in ui:
            errors.append("scene add must use prefabSpawnBatchCount")
        if "AppendFromPrefabFileBatch" not in ui:
            errors.append("scene add button must call batch append")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if BATCH_UI.is_file():
        if "prefabSpawnBatchCount" not in BATCH_UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("batch UI must bind spawn count to UiState")
    else:
        errors.append("missing PrefabAssetBatchSpawnUi.cpp")

    if EDITOR_H.is_file():
        if "prefabSpawnBatchCount" not in EDITOR_H.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("UiState must store prefabSpawnBatchCount")
    else:
        errors.append("missing EntityEditor.h")

    if errors:
        print("verify_prefab_scene_add_multi_count: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_scene_add_multi_count: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())