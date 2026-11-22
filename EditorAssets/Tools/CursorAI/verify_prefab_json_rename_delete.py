#!/usr/bin/env python3
"""プレハブ JSON の削除・改名 API と UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HEADER = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetFileOperations.h"
)
OPS = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetFileOperations.cpp"
)
PANEL = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetProjectPanelList.cpp"
)
SCRUB = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetUiPathScrub.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not HEADER.is_file():
        errors.append("missing PrefabAssetFileOperations.h")
    else:
        text = HEADER.read_text(encoding="utf-8", errors="replace")
        if "TryDeletePrefabJsonFile" not in text:
            errors.append("header must declare delete API")
        if "TryRenamePrefabJsonFile" not in text:
            errors.append("header must declare rename API")

    if OPS.is_file():
        ops = OPS.read_text(encoding="utf-8", errors="replace")
        if "fs::remove" not in ops or "fs::rename" not in ops:
            errors.append("file ops must remove and rename")
        if "kEntitiesPrefabDirectory" not in ops:
            errors.append("ops must scope to Entities prefab root")
    else:
        errors.append("missing PrefabAssetFileOperations.cpp")

    if PANEL.is_file():
        panel = PANEL.read_text(encoding="utf-8", errors="replace")
        if 'ImGui::Button("削除")' not in panel:
            errors.append("panel must offer delete")
        if 'ImGui::Button("名前変更")' not in panel:
            errors.append("panel must offer rename")
    else:
        errors.append("missing PrefabAssetProjectPanelList.cpp")

    if not SCRUB.is_file():
        errors.append("missing PrefabAssetUiPathScrub.cpp")

    if UI.is_file():
        if "PrefabAssetProjectPanelList" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("EntityEditorUi must use prefab project panel list")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_prefab_json_rename_delete: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_json_rename_delete: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())