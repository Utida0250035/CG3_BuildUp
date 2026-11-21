#!/usr/bin/env python3
"""本編 PlayerParry と JSON 経路が同じ組み立て処理を共有しているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def main() -> int:
    root = repo_root()
    errors: list[str] = []

    factory = (
        root / "Project/Codes/Source/System/EntityFactory.cpp"
    ).read_text(encoding="utf-8")
    post_build = (
        root
        / "Project/Codes/Source/System/Editor/Scene/EditorSceneRuntimePlayerComponentPostBuild.cpp"
    ).read_text(encoding="utf-8")
    binder = (
        root / "Project/Codes/Source/System/Scripting/LuaEntityApiBinder.cpp"
    ).read_text(encoding="utf-8")
    vcx = (root / "Project/CG2_BuildUp.vcxproj").read_text(encoding="utf-8")
    template = root / "EditorAssets/Entities/player_parry.template.json"

    if "AssemblePlayerParryRuntimeExtras" not in factory:
        errors.append("EntityFactory::PlayerParry must call AssemblePlayerParryRuntimeExtras")
    if "AssemblePlayerParryRuntimeExtras" not in post_build:
        errors.append("PostBuild must call AssemblePlayerParryRuntimeExtras")
    if "spawn_player_parry_from_json" not in binder:
        errors.append("Lua API spawn_player_parry_from_json missing")
    if "PlayerParryRuntimeAssembly.cpp" not in vcx:
        errors.append("vcxproj missing PlayerParryRuntimeAssembly.cpp")
    if not template.is_file():
        errors.append("missing player_parry.template.json")
    else:
        text = template.read_text(encoding="utf-8")
        if "useDefaultPlayerParryAssembly" not in text:
            errors.append("template should enable useDefaultPlayerParryAssembly")

    if not (
        root / "Project/Codes/Include/System/Playable/PlayerParryRuntimeAssembly.h"
    ).is_file():
        errors.append("missing PlayerParryRuntimeAssembly.h")

    if errors:
        print("verify_player_parry_runtime_assembly: FAILED")
        for err in errors:
            print(f"  - {err}")
        return 1

    print("verify_player_parry_runtime_assembly: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())