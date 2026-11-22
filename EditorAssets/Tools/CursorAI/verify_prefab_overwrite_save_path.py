#!/usr/bin/env python3
"""プレハブが選択パスへ上書き保存できるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
OVERWRITE_UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetOverwriteSaveUi.cpp"
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
OPS_H = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetFileOperations.h"
)


def main() -> int:
    errors: list[str] = []

    if not OPS_H.is_file() or "IsManagedExistingPrefabJsonPath" not in OPS_H.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("file ops must expose managed existing prefab path check")

    if OVERWRITE_UI.is_file():
        text = OVERWRITE_UI.read_text(encoding="utf-8", errors="replace")
        if "selectedPrefabPath" not in text:
            errors.append("overwrite UI must use selectedPrefabPath")
        if "SaveSelectedInstanceToPrefabPath" not in text:
            errors.append("overwrite must call SaveSelectedInstanceToPrefabPath")
        if "上書き" not in text:
            errors.append("overwrite button label required")
    else:
        errors.append("missing PrefabAssetOverwriteSaveUi.cpp")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "PrefabAssetOverwriteSaveUi" not in ui:
            errors.append("EntityEditorUi must draw overwrite save UI")
        if "MakeUniquePrefabFilePath" not in ui:
            errors.append("new-file save via MakeUnique must remain")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if OPS.is_file():
        if "IsManagedExistingPrefabJsonPath" not in OPS.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("ops must implement path check")
    else:
        errors.append("missing PrefabAssetFileOperations.cpp")

    if errors:
        print("verify_prefab_overwrite_save_path: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_overwrite_save_path: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())