#!/usr/bin/env python3
"""FeatureTest が Codes から機能検証シーン正本として参照されているか検査。"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE_JSON = ROOT / "EditorAssets" / "DebugPlayArea" / "FeatureTest.scene.json"
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
        errors.append("missing FeatureTest.scene.json")
    else:
        try:
            data = json.loads(SCENE_JSON.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            errors.append("FeatureTest.scene.json: invalid JSON")
        else:
            if data.get("sceneName") != "FeatureTest":
                errors.append("FeatureTest.scene.json: sceneName must be FeatureTest")

    if not PATHS_H.is_file():
        errors.append("missing EditorAssetsPaths.h")
    else:
        paths_text = PATHS_H.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaFeatureTestScene" not in paths_text:
            errors.append("EditorAssetsPaths.h: missing kDebugPlayAreaFeatureTestScene")
        if "FeatureTest.scene.json" not in paths_text:
            errors.append("EditorAssetsPaths.h: must point at FeatureTest.scene.json")

    if not CANONICAL_H.is_file():
        errors.append("missing EditorDebugPlayAreaFeatureVerificationCanonical.h")
    else:
        canon = CANONICAL_H.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaFeatureTestScene" not in canon:
            errors.append("canonical header must use kDebugPlayAreaFeatureTestScene")

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
        print("verify_feature_test_scene_canonical: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_feature_test_scene_canonical: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())