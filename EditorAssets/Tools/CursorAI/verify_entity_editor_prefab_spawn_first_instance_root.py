#!/usr/bin/env python3
"""プレハブ追加が先頭 1 体・親解除のみである経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SPAWN = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorPrefabSpawnFirstInstanceRoot.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not SPAWN.is_file():
        errors.append("missing EntityEditorPrefabSpawnFirstInstanceRoot.cpp")
    else:
        text = SPAWN.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "instances.front()",
            "parentIndex = -1",
            "MakeUniqueInstanceName",
            "TryBuildRootInstanceFromPrefabJson",
        ):
            if needle not in text:
                errors.append(f"prefab spawn module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "AppendFromPrefabFile" not in ed:
            errors.append("AddInstanceFromPrefabPath must delegate to prefab spawn module")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print(
            "verify_entity_editor_prefab_spawn_first_instance_root: FAILED",
            file=sys.stderr,
        )
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_prefab_spawn_first_instance_root: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())