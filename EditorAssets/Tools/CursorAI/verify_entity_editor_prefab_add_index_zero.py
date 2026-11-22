#!/usr/bin/env python3
"""プレハブ「シーンへ追加」が entities/instances の先頭 1 件のみであることを検査（roadmap GAP-021 突合）。"""

from __future__ import annotations

import re
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


def main() -> int:
    errors: list[str] = []

    if not SPAWN.is_file():
        errors.append("missing EntityEditorPrefabSpawnFirstInstanceRoot.cpp")
        return report(errors)

    text = SPAWN.read_text(encoding="utf-8", errors="replace")
    if "instances.front()" not in text:
        errors.append("scene-format prefab must use instances.front() (index 0)")

    if re.search(r"for\s*\([^)]*prefabScene\.instances", text):
        errors.append("must not iterate all prefabScene.instances")

    if re.search(r"prefabScene\.instances\.push_back", text):
        errors.append("must not bulk-append prefabScene.instances")

    return report(errors)


def report(errors: list[str]) -> int:
    if errors:
        print("verify_entity_editor_prefab_add_index_zero: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1
    print("verify_entity_editor_prefab_add_index_zero: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())