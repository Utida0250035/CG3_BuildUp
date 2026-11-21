#!/usr/bin/env python3
"""EntityEditor UI が DebugPlayArea シーンを一覧表示する導線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
LIST_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorUiEditableSceneList.cpp"
)
UI_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorUi.cpp"
)
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"
ENTRY_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Scene" / "EditorSceneJsonFileEntry.h"


def main() -> int:
    errors: list[str] = []

    if not LIST_CPP.is_file():
        errors.append("missing EntityEditorUiEditableSceneList.cpp")
    else:
        text = LIST_CPP.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "ListEditableSceneJsonFiles",
            "テスト再生エリア (DebugPlayArea)",
            "kDebugPlayAreaDirectory",
        ):
            if needle not in text:
                errors.append(f"scene list UI must include {needle!r}")

    if not UI_CPP.is_file():
        errors.append("missing EntityEditorUi.cpp")
    else:
        ui = UI_CPP.read_text(encoding="utf-8", errors="replace")
        if "DrawEntityEditorEditableSceneList" not in ui:
            errors.append("EntityEditorUi must call DrawEntityEditorEditableSceneList")

    if not ASSET_CPP.is_file():
        errors.append("missing EditorSceneAsset.cpp")
    else:
        asset = ASSET_CPP.read_text(encoding="utf-8", errors="replace")
        if "IsEditableSceneJsonPath" not in asset:
            errors.append("ListSceneJsonFiles must use IsEditableSceneJsonPath")
        if "kDebugPlayAreaDirectory" not in asset.split("TryDeleteSceneJsonFile", 1)[-1][:800]:
            errors.append("TryDeleteSceneJsonFile must allow DebugPlayArea root")

    if not ENTRY_H.is_file():
        errors.append("missing EditorSceneJsonFileEntry.h")
    else:
        entry = ENTRY_H.read_text(encoding="utf-8", errors="replace")
        if ".scene.json" not in entry:
            errors.append("must recognize .scene.json suffix")

    if errors:
        print("verify_entity_editor_ui_debug_play_area_list: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_ui_debug_play_area_list: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())