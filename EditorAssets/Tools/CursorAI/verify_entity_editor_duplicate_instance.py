#!/usr/bin/env python3
"""EntityEditor の選択インスタンス複製 UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
UI_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
DUP_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorDuplicateSelectedInstance.cpp"
)
EDITOR_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"


def main() -> int:
    errors: list[str] = []

    if not DUP_CPP.is_file():
        errors.append("missing EntityEditorDuplicateSelectedInstance.cpp")
    else:
        text = DUP_CPP.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "DuplicateSelectedInstanceInScene",
            "MakeUniqueDuplicateInstanceName",
            "CollectDuplicateSourceIndices",
            "selectedInstanceIndices",
            "EntityEditorAuthoringDocumentEditNotify::Notify",
        ):
            if needle not in text:
                errors.append(f"duplicate module: expected {needle}")

    if not UI_CPP.is_file():
        errors.append("missing EntityEditorUi.cpp")
    else:
        ui = UI_CPP.read_text(encoding="utf-8", errors="replace")
        if 'ImGui::Button("複製")' not in ui:
            errors.append("hierarchy must expose Duplicate button")
        if "DuplicateSelectedInstance()" not in ui:
            errors.append("hierarchy must call DuplicateSelectedInstance")

    if not EDITOR_H.is_file():
        errors.append("missing EntityEditor.h")
    else:
        header = EDITOR_H.read_text(encoding="utf-8", errors="replace")
        if "DuplicateSelectedInstance" not in header:
            errors.append("EntityEditor.h must declare DuplicateSelectedInstance")

    if errors:
        print("verify_entity_editor_duplicate_instance: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_duplicate_instance: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())