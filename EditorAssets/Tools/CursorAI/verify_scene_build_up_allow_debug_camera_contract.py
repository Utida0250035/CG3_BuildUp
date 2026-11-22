#!/usr/bin/env python3
"""SceneBuildUp が AllowDebugCameraControl を override せず Scene 既定に従うことを検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP_H = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneBuildUp.h"
BUILDUP_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
SCENE_H = ROOT / "Project" / "Codes" / "Include" / "Scene" / "Scene.h"
CONTRACT_H = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "Scene"
    / "SceneBuildUpAllowDebugCameraContract.h"
)
POLICY_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneDebugCameraInputPolicy.cpp"

OVERRIDE_RE = re.compile(
    r"AllowDebugCameraControl\s*\(\)\s*const\s*(?:override\s*)?\{"
)


def main() -> int:
    errors: list[str] = []

    if not CONTRACT_H.is_file():
        errors.append("missing SceneBuildUpAllowDebugCameraContract.h")
    elif "kInheritsSceneAllowDebugCameraControl" not in CONTRACT_H.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("contract header must document Scene inheritance")

    if BUILDUP_H.is_file():
        text = BUILDUP_H.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUpAllowDebugCameraContract.h" not in text:
            errors.append("SceneBuildUp.h must include allow-debug contract header")
        if OVERRIDE_RE.search(text):
            errors.append("SceneBuildUp.h must not override AllowDebugCameraControl")
    else:
        errors.append("missing SceneBuildUp.h")

    if BUILDUP_CPP.is_file() and OVERRIDE_RE.search(
        BUILDUP_CPP.read_text(encoding="utf-8", errors="replace")
    ):
        errors.append("SceneBuildUp.cpp must not define AllowDebugCameraControl")

    if SCENE_H.is_file():
        scene = SCENE_H.read_text(encoding="utf-8", errors="replace")
        if not re.search(
            r"AllowDebugCameraControl\s*\(\)\s*const\s*\{\s*return\s+false",
            scene,
        ):
            errors.append("Scene base AllowDebugCameraControl must default to false")
    else:
        errors.append("missing Scene.h")

    if POLICY_CPP.is_file():
        if "AllowDebugCameraControl" not in POLICY_CPP.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("SceneDebugCameraInputPolicy must consult AllowDebugCameraControl")
    else:
        errors.append("missing SceneDebugCameraInputPolicy.cpp")

    if errors:
        print("verify_scene_build_up_allow_debug_camera_contract: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_allow_debug_camera_contract: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())