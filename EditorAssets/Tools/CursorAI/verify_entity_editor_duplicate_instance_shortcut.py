#!/usr/bin/env python3
"""エンティティ複製 Ctrl+D ショートカットの配線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SHORTCUT = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorDuplicateInstanceKeyboardShortcut.cpp"
)
WORKBENCH = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"


def main() -> int:
    errors: list[str] = []

    if not SHORTCUT.is_file():
        errors.append("missing EntityEditorDuplicateInstanceKeyboardShortcut.cpp")
    else:
        text = SHORTCUT.read_text(encoding="utf-8", errors="replace")
        if "ImGuiMod_Ctrl | ImGuiKey_D" not in text:
            errors.append("shortcut must use Ctrl+D")
        if "DuplicateSelectedInstance" not in text:
            errors.append("shortcut must call DuplicateSelectedInstance")

    if WORKBENCH.is_file():
        wb = WORKBENCH.read_text(encoding="utf-8", errors="replace")
        if "PollEntityEditorDuplicateInstanceKeyboardShortcut" not in wb:
            errors.append("workbench must poll duplicate shortcut")
    else:
        errors.append("missing EditorEntityWorkbench.cpp")

    if errors:
        print("verify_entity_editor_duplicate_instance_shortcut: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_duplicate_instance_shortcut: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())