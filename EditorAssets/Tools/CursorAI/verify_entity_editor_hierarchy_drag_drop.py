#!/usr/bin/env python3
"""ヒエラルキーのドラッグ複製・並べ替えを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DRAG = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorHierarchyDragDrop.cpp"
)
REORDER = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorHierarchyInstanceReorder.cpp"
)
DUP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorHierarchyDuplicateAsChild.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not DRAG.is_file():
        errors.append("missing EntityEditorHierarchyDragDrop.cpp")
    else:
        text = DRAG.read_text(encoding="utf-8", errors="replace")
        if "BeginDragDropSource" not in text or "BeginDragDropTarget" not in text:
            errors.append("hierarchy drag drop must use ImGui DnD API")
        if "KeyCtrl" not in text or "KeyShift" not in text:
            errors.append("drag drop must distinguish duplicate and reorder")

    if not REORDER.is_file():
        errors.append("missing hierarchy reorder module")
    elif "MoveAfterSibling" not in REORDER.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("reorder module must implement MoveAfterSibling")

    if not DUP.is_file():
        errors.append("missing hierarchy duplicate-as-child module")

    if UI.is_file():
        if "EntityEditorHierarchyDragDrop" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("DrawHierarchyNode must attach drag drop")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_entity_editor_hierarchy_drag_drop: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_hierarchy_drag_drop: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())