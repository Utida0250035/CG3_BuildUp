#!/usr/bin/env python3
"""削除が複数選択 index に対応する経路を検査。"""

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
    / "EntityEditorRemoveSelectedInstances.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorRemoveSelectedInstances.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "selectedInstanceIndices",
            "oldToNew",
            "CollectRemovalIndices",
        ):
            if needle not in text:
                errors.append(f"remove module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "RemoveFromScene" not in ed:
            errors.append("RemoveSelectedInstance must delegate to remove module")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_remove_selected_instances: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_remove_selected_instances: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())