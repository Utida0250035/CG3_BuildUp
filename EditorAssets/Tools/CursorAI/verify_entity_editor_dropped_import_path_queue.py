#!/usr/bin/env python3
"""複数ドロップ時に Imported パスがキューへ残る経路を検査。"""

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
    / "EntityEditorDroppedImportPathQueue.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorDroppedImportPathQueue.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        if "recentImportedAssetPaths" not in text:
            errors.append("queue module must set recentImportedAssetPaths")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "RecordBatch" not in ed:
            errors.append("ProcessDroppedFileImports must record import queue")
        loop = ed.split("for (const std::string &sourcePath : dropped)", 1)
        if len(loop) > 1 and "lastImportedAssetPath = dest" in loop[1].split("RecordBatch", 1)[0]:
            errors.append("must not assign lastImported inside import loop")
    else:
        errors.append("missing EntityEditor.cpp")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "recentImportedAssetPaths" not in ui:
            errors.append("UI must list recentImportedAssetPaths")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_entity_editor_dropped_import_path_queue: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_dropped_import_path_queue: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())