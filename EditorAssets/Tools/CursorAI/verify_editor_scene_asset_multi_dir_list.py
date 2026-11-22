#!/usr/bin/env python3
"""ListEditableSceneJsonFiles が複数ルートを走査することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
ROOTS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorEditableSceneJsonRoots.h"
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"
UI_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
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
ENTITY_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not ROOTS_H.is_file():
        errors.append("missing EditorEditableSceneJsonRoots.h")
    else:
        text = ROOTS_H.read_text(encoding="utf-8", errors="replace")
        for marker in (
            "kScenesDirectory",
            "kDebugPlayAreaDirectory",
            "EditorAssets::kScenesDirectory",
        ):
            if marker not in text:
                errors.append(f"roots must include {marker}")

    if not ASSET_CPP.is_file():
        errors.append("missing EditorSceneAsset.cpp")
    else:
        text = ASSET_CPP.read_text(encoding="utf-8", errors="replace")
        if "EditorEditableSceneJsonRoots::kAll" not in text:
            errors.append("ListEditableSceneJsonFiles must iterate kAll roots")
        editable = text.split("ListEditableSceneJsonFiles", 1)
        if len(editable) > 1:
            body = editable[1].split("}", 1)[0]
            if body.count("ListSceneJsonFiles(GameAssets::kScenesDirectory)") > 0:
                errors.append(
                    "ListEditableSceneJsonFiles must not hardcode single dir only"
                )

    if not LIST_CPP.is_file():
        errors.append("missing EntityEditorUiEditableSceneList.cpp")
    else:
        list_text = LIST_CPP.read_text(encoding="utf-8", errors="replace")
        if "ListEditableSceneJsonFiles" not in list_text:
            errors.append("editable scene list UI must use ListEditableSceneJsonFiles")
        if "ListSceneJsonFiles(" in list_text:
            errors.append("editable scene list UI must not call ListSceneJsonFiles directly")

    if UI_CPP.is_file():
        ui = UI_CPP.read_text(encoding="utf-8", errors="replace")
        if "DrawEntityEditorEditableSceneList" not in ui:
            errors.append("EntityEditorUi must wire DrawEntityEditorEditableSceneList")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if ENTITY_CPP.is_file():
        entity = ENTITY_CPP.read_text(encoding="utf-8", errors="replace")
        if "ListEditableSceneJsonFiles" not in entity:
            errors.append("EntityEditor must use ListEditableSceneJsonFiles")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_editor_scene_asset_multi_dir_list: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_asset_multi_dir_list: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())