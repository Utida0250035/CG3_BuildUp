#!/usr/bin/env python3
"""プレハブ新規出力が stem_N 以外の衝突方針も選べるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RESOLVE = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetCollisionPolicy.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
POLICY_UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Prefab"
    / "PrefabAssetOutputCollisionPolicyUi.cpp"
)
EDITOR_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"


def main() -> int:
    errors: list[str] = []

    if not RESOLVE.is_file():
        errors.append("missing PrefabAssetCollisionPolicy.cpp")
    else:
        text = RESOLVE.read_text(encoding="utf-8", errors="replace")
        if "OverwriteExisting" not in text or "TimestampSuffix" not in text:
            errors.append("prefab collision must include overwrite and timestamp")
        if "BatchPrefixThenIncrement" not in text:
            errors.append("prefab collision must include batch prefix mode")
        if "stem + \"_\" + std::to_string(suffix)" not in text:
            errors.append("auto increment must use stem_N pattern")

    if POLICY_UI.is_file():
        if "連番 (stem_N)" not in POLICY_UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("prefab policy UI must document stem_N")
    else:
        errors.append("missing PrefabAssetOutputCollisionPolicyUi.cpp")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "ResolvePrefabJsonOutputPath" not in ui:
            errors.append("save button must use ResolvePrefabJsonOutputPath")
        if "prefabOutputCollisionPolicy" not in ui:
            errors.append("save must read prefabOutputCollisionPolicy")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if EDITOR_H.is_file():
        if "PrefabAssetNameCollisionPolicy" not in EDITOR_H.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("UiState must define PrefabAssetNameCollisionPolicy")
    else:
        errors.append("missing EntityEditor.h")

    if errors:
        print("verify_prefab_collision_policy: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_collision_policy: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())