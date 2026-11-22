#!/usr/bin/env python3
"""LoopTick13Demo が Codes から機能検証シーン正本として参照されているか検査。"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE_JSON = ROOT / "EditorAssets" / "DebugPlayArea" / "LoopTick13Demo.scene.json"
PATHS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorAssetsPaths.h"
CANONICAL_H = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "System"
    / "Editor"
    / "EditorDebugPlayAreaFeatureVerificationCanonical.h"
)
HOTKEYS_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorDebugHotkeys.cpp"
WORKBENCH_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"


def main() -> int:
    errors: list[str] = []

    if not SCENE_JSON.is_file():
        errors.append("missing LoopTick13Demo.scene.json")
    else:
        try:
            data = json.loads(SCENE_JSON.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            errors.append("LoopTick13Demo.scene.json: invalid JSON")
        else:
            if data.get("sceneName") != "LoopTick13Demo":
                errors.append("LoopTick13Demo.scene.json: sceneName must be LoopTick13Demo")

    if not PATHS_H.is_file():
        errors.append("missing EditorAssetsPaths.h")
    else:
        paths_text = PATHS_H.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaLoopTick13DemoScene" not in paths_text:
            errors.append("EditorAssetsPaths.h: missing kDebugPlayAreaLoopTick13DemoScene")
        if "LoopTick13Demo.scene.json" not in paths_text:
            errors.append("EditorAssetsPaths.h: must point at LoopTick13Demo.scene.json")

    if not CANONICAL_H.is_file():
        errors.append("missing EditorDebugPlayAreaFeatureVerificationCanonical.h")
    else:
        canon = CANONICAL_H.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaLoopTick13DemoScene" not in canon:
            errors.append("canonical header must use kDebugPlayAreaLoopTick13DemoScene")

    for path, label in ((HOTKEYS_CPP, "F2"), (WORKBENCH_CPP, "debug menu")):
        if not path.is_file():
            errors.append(f"missing {path.name}")
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if "DebugPlayAreaFeatureVerificationCanonical::SceneJsonPath" not in text:
            errors.append(f"{label}: must launch canonical SceneJsonPath()")
        if "kDebugPlayAreaTemplateScene" in text and label == "F2":
            errors.append("F2 must not launch kDebugPlayAreaTemplateScene")

    if errors:
        print("verify_loop_tick13_demo_feature_verification_canonical: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_loop_tick13_demo_feature_verification_canonical: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())