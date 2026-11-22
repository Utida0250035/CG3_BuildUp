#!/usr/bin/env python3
"""シーン JSON 削除後に一覧順の後続／前シーンへ開く経路を検査。"""

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
    / "EntityEditorDeleteSceneOpenSuccessor.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorDeleteSceneOpenSuccessor.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        for needle in ("ResolveAfterDelete", "deletedIndex", "remaining"):
            if needle not in text:
                errors.append(f"successor module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "ResolveAfterDelete" not in ed:
            errors.append("DeleteSceneFile must resolve successor scene path")
        if "remaining.front()" in ed:
            errors.append("DeleteSceneFile must not use remaining.front() only")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_delete_scene_successor: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_delete_scene_successor: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())