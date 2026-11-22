#!/usr/bin/env python3
"""combat_bounds_sandbox が Codes の既定デバッグ導線から参照されているか検査。"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE_JSON = ROOT / "EditorAssets" / "DebugPlayArea" / "combat_bounds_sandbox.scene.json"
PATHS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorAssetsPaths.h"
LAUNCH_H = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "System"
    / "Editor"
    / "EditorDebugPlayAreaCombatBoundsSandbox.h"
)
HOTKEYS_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorDebugHotkeys.cpp"
WORKBENCH_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"


def main() -> int:
    errors: list[str] = []

    if not SCENE_JSON.is_file():
        errors.append("missing combat_bounds_sandbox.scene.json")
    else:
        try:
            data = json.loads(SCENE_JSON.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            errors.append("combat_bounds_sandbox.scene.json: invalid JSON")
        else:
            if data.get("sceneName") != "CombatBoundsSandbox":
                errors.append("sceneName must be CombatBoundsSandbox")
            if "stageViewFramingBounds" not in data:
                errors.append("stageViewFramingBounds required for bounds sandbox")

    if not PATHS_H.is_file():
        errors.append("missing EditorAssetsPaths.h")
    else:
        text = PATHS_H.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaCombatBoundsSandboxScene" not in text:
            errors.append("EditorAssetsPaths.h: missing kDebugPlayAreaCombatBoundsSandboxScene")
        if "combat_bounds_sandbox.scene.json" not in text:
            errors.append("EditorAssetsPaths.h: must reference combat_bounds_sandbox.scene.json")

    if not LAUNCH_H.is_file():
        errors.append("missing EditorDebugPlayAreaCombatBoundsSandbox.h")

    if HOTKEYS_CPP.is_file():
        hotkeys = HOTKEYS_CPP.read_text(encoding="utf-8", errors="replace")
        if "Key::F6" not in hotkeys:
            errors.append("F6 hotkey for combat_bounds_sandbox missing")
        if "DebugPlayAreaCombatBoundsSandbox::SceneJsonPath" not in hotkeys:
            errors.append("F6 must launch DebugPlayAreaCombatBoundsSandbox::SceneJsonPath()")
    else:
        errors.append("missing SceneEntityEditorDebugHotkeys.cpp")

    if WORKBENCH_CPP.is_file():
        workbench = WORKBENCH_CPP.read_text(encoding="utf-8", errors="replace")
        if "DebugPlayAreaCombatBoundsSandbox::SceneJsonPath" not in workbench:
            errors.append("debug menu must launch combat_bounds_sandbox")
    else:
        errors.append("missing EditorEntityWorkbench.cpp")

    # Codes 全体でファイル名が文字列として残っていること
    codes_root = ROOT / "Project" / "Codes"
    if not any(
        "combat_bounds_sandbox" in p.read_text(encoding="utf-8", errors="replace")
        for p in codes_root.rglob("*")
        if p.suffix in {".h", ".cpp", ".hpp"}
    ):
        errors.append("Project/Codes has no combat_bounds_sandbox string reference")

    if errors:
        print("verify_combat_bounds_sandbox_debug_play_area: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_combat_bounds_sandbox_debug_play_area: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())