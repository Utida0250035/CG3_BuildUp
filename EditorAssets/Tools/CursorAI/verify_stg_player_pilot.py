#!/usr/bin/env python3
"""全方位 STG 自機（CmpStgPlayerPilot）の登録と Lua spawn を検査。"""

from __future__ import annotations

import sys
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def main() -> int:
    root = repo_root()
    errors: list[str] = []

    registry = (
        root / "Project/Codes/Source/System/Editor/Entity/EntityEditorKnownComponentRegistry.cpp"
    ).read_text(encoding="utf-8")
    runtime = (
        root / "Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp"
    ).read_text(encoding="utf-8")
    binder = (
        root / "Project/Codes/Source/System/Scripting/LuaEntityApiBinder.cpp"
    ).read_text(encoding="utf-8")
    pilot = (
        root / "Project/Codes/Source/System/Playable/CmpStgPlayerPilot.cpp"
    ).read_text(encoding="utf-8")

    if '"CmpStgPlayerPilot"' not in registry:
        errors.append("registry missing CmpStgPlayerPilot")
    if 'slot.typeName == "CmpStgPlayerPilot"' not in runtime:
        errors.append("EditorSceneRuntime missing CmpStgPlayerPilot handler")
    if "spawn_stg_player_from_json" not in binder:
        errors.append("Lua API spawn_stg_player_from_json missing")
    if "GetRightStick" not in (
        root / "Project/Codes/Source/Input/XInputController.cpp"
    ).read_text(encoding="utf-8"):
        errors.append("XInputController missing GetRightStick")
    for token in ("Key::W", "Key::SPACE", "GamepadButton::A", "GetLeftStick"):
        if token not in pilot:
            errors.append(f"CmpStgPlayerPilot missing input binding: {token}")
    if "shootCooldownSeconds_" not in pilot:
        errors.append("CmpStgPlayerPilot missing shoot cooldown")

    template = root / "EditorAssets/Entities/stg_player_pilot.template.json"
    if not template.is_file():
        errors.append("missing stg_player_pilot.template.json")

    if errors:
        print("verify_stg_player_pilot: FAILED")
        for err in errors:
            print(f"  - {err}")
        return 1

    print("verify_stg_player_pilot: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())