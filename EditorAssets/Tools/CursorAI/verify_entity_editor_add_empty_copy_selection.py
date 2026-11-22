#!/usr/bin/env python3
"""「オブジェクトを追加」が選択中定義を複製する経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MOD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorAddEmptyInstanceFromSelection.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorAddEmptyInstanceFromSelection.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "SelectedDefinition()",
            "MakeDefaultEntityDefinition",
            "EntityEditorAuthoringDocumentEditNotify::Notify",
        ):
            if needle not in text:
                errors.append(f"add-empty module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "AppendInstance" not in ed:
            errors.append("AddEmptyInstance must delegate to add-empty module")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_add_empty_copy_selection: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_add_empty_copy_selection: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())