#!/usr/bin/env python3
"""Command / Audio / Storage / Registry の Lua バインド検査。"""

from __future__ import annotations

import sys
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def main() -> int:
    root = repo_root()
    errors: list[str] = []

    binder = (
        root / "Project/Codes/Source/System/Scripting/LuaSubsystemApiBinder.cpp"
    )
    if not binder.is_file():
        errors.append("missing LuaSubsystemApiBinder.cpp")
    else:
        text = binder.read_text(encoding="utf-8")
        for marker in (
            'atrum["Command"]',
            'atrum["Audio"]',
            'atrum["Storage"]',
            'atrum["Registry"]',
            "CommandScene::Instance",
            "CommandChangeScene::Instance",
            "EngineCommandContextDirect",
            "EngineAudioManager",
            "EngineTextureStorage",
            "EngineModelStorage",
            "EntityStorage::Instance",
            "EntityTagRegistry::Instance",
            "RegistryCmp::Instance",
            "SceneBuildUpRegistry::Instance",
            "SceneMovieRegistry::Instance",
        ):
            if marker not in text:
                errors.append(f"LuaSubsystemApiBinder missing: {marker}")

    host = (
        root
        / "Project/Codes/Source/Scene/SceneBuildUpLuaRuntimeHost.cpp"
    ).read_text(encoding="utf-8")
    if "LuaSubsystemApiBinder" not in host:
        errors.append("SceneBuildUpLuaRuntimeHost must register LuaSubsystemApiBinder")

    vcx = (root / "Project/CG2_BuildUp.vcxproj").read_text(encoding="utf-8")
    if r"LuaSubsystemApiBinder.cpp" not in vcx:
        errors.append("vcxproj missing LuaSubsystemApiBinder.cpp")

    if errors:
        print("verify_lua_subsystem_api_bind: FAILED")
        for err in errors:
            print(f"  - {err}")
        return 1

    print("verify_lua_subsystem_api_bind: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())