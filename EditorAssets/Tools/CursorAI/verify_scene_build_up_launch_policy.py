#!/usr/bin/env python3
"""BuildUp: Launch は生成のみを呼び手が守っているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"

LAUNCH_CALL = "SceneBuildUpRegistry::Instance().Launch("


def main() -> int:
    errors: list[str] = []
    policy_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneBuildUpLaunchPolicy.h"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if not policy_h.is_file():
        errors.append("missing SceneBuildUpLaunchPolicy.h")

    policy_text = policy_h.read_text(encoding="utf-8", errors="replace")
    if "CreateOwnedBuildUp" not in policy_text:
        errors.append("SceneBuildUpLaunchPolicy.h: expected CreateOwnedBuildUp")

    dedupe_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpRegistryLaunchDedupe.cpp"
    build_up_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
    build_up_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneBuildUp.h"

    if not dedupe_cpp.is_file():
        errors.append("missing SceneBuildUpRegistryLaunchDedupe.cpp")
    if build_up_cpp.is_file() and "SceneBuildUpRegistryLaunchDedupe::AcquireOrCreateOwned" not in build_up_cpp.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("SceneBuildUp.cpp: Launch must use AcquireOrCreateOwned")
    if build_up_h.is_file() and "SetReturnScene" not in build_up_h.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("SceneBuildUp.h: expected SetReturnScene for relaunch")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "QueueFadeToBuildUp" not in text:
            errors.append("SceneEntityEditor: expected QueueFadeToBuildUp")
        if LAUNCH_CALL in text:
            errors.append("SceneEntityEditor: raw SceneBuildUp Launch prohibited")

    allowed_launch_files = {
        (CODES / "Source" / "Scene" / "SceneBuildUp.cpp").resolve(),
        (CODES / "Source" / "Scene" / "SceneBuildUpLaunchPolicy.cpp").resolve(),
    }

    for path in CODES.rglob("*"):
        if path.suffix not in {".cpp", ".h", ".hpp"}:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if LAUNCH_CALL not in text:
            continue
        if path.resolve() not in allowed_launch_files:
            errors.append(
                f"{path.relative_to(ROOT)}: direct SceneBuildUpRegistry::Launch not allowed"
            )

    if errors:
        print("verify_scene_build_up_launch_policy: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_launch_policy: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())