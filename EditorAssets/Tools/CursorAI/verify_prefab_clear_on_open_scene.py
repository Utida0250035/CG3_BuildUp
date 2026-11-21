#!/usr/bin/env python3
"""シーンオープン時に selectedPrefabPath がクリアされるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"
SCRUB = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorClearPrefabSelectionOnSceneOpen.cpp"
)
HEADER = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorClearPrefabSelectionOnSceneOpen.h"
)


def main() -> int:
    errors: list[str] = []

    if not HEADER.is_file():
        errors.append("missing clear prefab selection header")

    if SCRUB.is_file():
        if "selectedPrefabPath.clear" not in SCRUB.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("scrub must clear selectedPrefabPath")
    else:
        errors.append("missing EntityEditorClearPrefabSelectionOnSceneOpen.cpp")

    if EDITOR.is_file():
        body = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorClearPrefabSelectionOnSceneOpen" not in body:
            errors.append("EntityEditor must invoke clear on scene open")
        if "OpenSceneFromPath" in body:
            # OpenSceneFromPath 関数内で OnSceneDocumentOpened が呼ばれること
            start = body.find("bool EntityEditor::OpenSceneFromPath")
            if start < 0:
                errors.append("OpenSceneFromPath not found")
            else:
                end = body.find("\nbool EntityEditor::", start + 1)
                block = body[start:end] if end > start else body[start:]
                if "OnSceneDocumentOpened" not in block:
                    errors.append("OpenSceneFromPath must call OnSceneDocumentOpened")
        else:
            errors.append("missing OpenSceneFromPath")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_prefab_clear_on_open_scene: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_clear_on_open_scene: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())