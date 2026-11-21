#!/usr/bin/env python3
"""Lua ランタイム統合とエンティティ API バインドの登録状態を検査する。"""

from __future__ import annotations

import sys
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def main() -> int:
    root = repo_root()
    vcx = (root / "Project" / "CG2_BuildUp.vcxproj").read_text(encoding="utf-8")
    errors: list[str] = []

    required_compile = [
        r"Codes\Source\System\Scripting\LuaSystem.cpp",
        r"Codes\Source\System\Scripting\LuaEntityApiBinder.cpp",
        r"Codes\Source\System\Scripting\LuaEntitySpawnFromJson.cpp",
        r"Codes\Source\System\Scripting\LuaEntityComponentAttach.cpp",
        r"Codes\Source\System\Entity\Components\CmpLuaUpd.cpp",
        r"Codes\Source\System\Entity\Components\CmpLuaDraw.cpp",
        r"Codes\Source\Scene\SceneBuildUpLuaRuntimeHost.cpp",
    ]
    for item in required_compile:
        if item not in vcx:
            errors.append(f"vcxproj missing ClCompile: {item}")

    if "Externals\\LuaJIT\\include" not in vcx:
        errors.append("vcxproj must include LuaJIT include path")

    if "Externals\\LuaJIT\\lib" not in vcx:
        errors.append("vcxproj must include LuaJIT lib path")

    allowlist = root / "EditorAssets" / "Tools" / "CursorAI" / "vcxproj_coverage_allowlist.txt"
    if allowlist.is_file():
        for line in allowlist.read_text(encoding="utf-8").splitlines():
            stripped = line.split("#", 1)[0].strip()
            if stripped.endswith("LuaSystem.cpp") or stripped.endswith("LuaSystemPool.cpp"):
                errors.append(f"allowlist still exempts Lua: {stripped}")

    bootstrap = root / "Project" / "Assets" / "Scripts" / "Runtime" / "bootstrap.lua"
    if not bootstrap.is_file():
        errors.append("missing Assets/Scripts/Runtime/bootstrap.lua")

    scene_build_up = (
        root / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
    ).read_text(encoding="utf-8")
    if "luaRuntimeHost_.OnEnterScene" not in scene_build_up:
        errors.append("SceneBuildUp must start Lua runtime on enter")

    binder = (
        root / "Project" / "Codes" / "Source" / "System" / "Scripting" / "LuaEntityApiBinder.cpp"
    ).read_text(encoding="utf-8")
    for api in (
        "spawn_entity_from_json",
        "tag_for_role",
        "require_behavior",
        "attach_lua_upd",
        "attach_lua_draw",
    ):
        if api not in binder:
            errors.append(f"LuaEntityApiBinder missing API: {api}")

    host_cpp = (
        root
        / "Project"
        / "Codes"
        / "Source"
        / "Scene"
        / "SceneBuildUpLuaRuntimeHost.cpp"
    ).read_text(encoding="utf-8")
    if "LuaComponentRuntime::SetActive" not in host_cpp:
        errors.append("SceneBuildUpLuaRuntimeHost must activate LuaComponentRuntime")

    if errors:
        print("verify_lua_runtime_entity_bind: FAILED")
        for err in errors:
            print(f"  - {err}")
        return 1

    print("verify_lua_runtime_entity_bind: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())