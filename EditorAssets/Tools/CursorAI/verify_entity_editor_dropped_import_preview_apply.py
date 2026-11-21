#!/usr/bin/env python3
"""ドロップインポート後に requestApplyPreview が立つ経路を検査。"""

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
    / "EntityEditorDroppedImportRequestPreview.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorDroppedImportRequestPreview.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        if "requestApplyPreview" not in text:
            errors.append("preview module must set requestApplyPreview")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "DroppedImportRequestPreview::AfterProcessing" not in ed:
            errors.append("ProcessDroppedFileImports must request preview refresh")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_dropped_import_preview_apply: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_dropped_import_preview_apply: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())