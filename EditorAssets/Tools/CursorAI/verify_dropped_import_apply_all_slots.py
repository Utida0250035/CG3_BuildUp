#!/usr/bin/env python3
"""直近ドロップを単一 Cmp だけでなく全パススロットへ適用できるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
APPLY = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorImportedAssetApplyToEntityDefinition.cpp"
UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorInspectorImportedDropBatchApplyUi.cpp"
)
MAIN = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not APPLY.is_file():
        errors.append("missing EditorImportedAssetApplyToEntityDefinition.cpp")
    else:
        text = APPLY.read_text(encoding="utf-8", errors="replace")
        if "ApplyImportedPathToAllMatchingSlots" not in text:
            errors.append("must define ApplyImportedPathToAllMatchingSlots")
        if "for (EntityEditor::ComponentSlot &slot : definition.components)" not in text:
            errors.append("must iterate all component slots")

    if not UI.is_file():
        errors.append("missing EntityEditorInspectorImportedDropBatchApplyUi.cpp")
    else:
        ui_text = UI.read_text(encoding="utf-8", errors="replace")
        if "全パススロットへ適用" not in ui_text:
            errors.append("inspector batch apply button required")

    if MAIN.is_file():
        if "EntityEditorInspectorImportedDropBatchApplyUi" not in MAIN.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("EntityEditorUi must wire inspector batch apply UI")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_dropped_import_apply_all_slots: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_dropped_import_apply_all_slots: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())