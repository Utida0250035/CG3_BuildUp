#!/usr/bin/env python3
"""LoadFromJsonPath 後にインスタンス名で選択を復元する経路を検査。"""

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
    / "EntityEditorRestoreSelectionAfterSceneLoad.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorRestoreSelectionAfterSceneLoad.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "SelectionSnapshot",
            "selectedInstanceNames",
            "Capture",
            "Apply",
        ):
            if needle not in text:
                errors.append(f"restore module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorRestoreSelectionAfterSceneLoad::Apply" not in ed:
            errors.append("ApplySceneDocumentLoadedFromDisk must restore selection")
        if "ResetSelectionAfterSceneDocumentReload()" in ed.split("ApplySceneDocumentLoadedFromDisk", 1)[-1][:800]:
            errors.append("load path must not hard-reset selection to 0 only")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_load_json_keep_selection: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_load_json_keep_selection: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())