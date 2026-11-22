#!/usr/bin/env python3
"""タグをすべて解除が複数選択に対応するか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CLEAR = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorClearAllEntityTags.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not CLEAR.is_file():
        errors.append("missing EntityEditorClearAllEntityTags.cpp")
    else:
        text = CLEAR.read_text(encoding="utf-8", errors="replace")
        if "ClearAllTagsOnInstanceIndices" not in text:
            errors.append("batch clear helper required")
        if "tagHashes.clear" not in text:
            errors.append("must clear tagHashes")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "タグをすべて解除（選択全体）" not in ui:
            errors.append("multi-select panel must offer clear all tags")
        if "EntityEditorClearAllEntityTags" not in ui:
            errors.append("UI must call clear all tags helper")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_clear_all_tags_multi_select: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_clear_all_tags_multi_select: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())