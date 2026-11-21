#!/usr/bin/env python3
"""プレイヤー向けコンポーネントが Editor と Runtime に登録されているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

PLAYABLE = (
    "AbstractInput",
    "CmpMoveByInput2D",
    "CmpDrawPlayerParryField",
    "CmpDirection",
    "CmpLhRigidBody",
    "CmpHealthHit",
    "CmpSwing",
    "CmpChildEntitys",
    "CmpParry",
)


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def main() -> int:
    root = repo_root()
    registry = (
        root / "Project/Codes/Source/System/Editor/Entity/EntityEditorKnownComponentRegistry.cpp"
    ).read_text(encoding="utf-8")
    runtime = (
        root / "Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp"
    ).read_text(encoding="utf-8")
    errors: list[str] = []

    for name in PLAYABLE:
        if f'"{name}"' not in registry:
            errors.append(f"registry missing: {name}")
        if f'slot.typeName == "{name}"' not in runtime:
            errors.append(f"EditorSceneRuntime missing slot handler: {name}")

    if "ApplyPlayerComponentPostBuild" not in runtime:
        errors.append("BuildEntities must call ApplyPlayerComponentPostBuild")

    template = root / "EditorAssets/Entities/player_parry.template.json"
    if not template.is_file():
        errors.append("missing player_parry.template.json")

    if errors:
        print("verify_editor_playable_component_registration: FAILED")
        for err in errors:
            print(f"  - {err}")
        return 1

    print("verify_editor_playable_component_registration: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())