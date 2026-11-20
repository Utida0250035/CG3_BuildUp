#!/usr/bin/env python3
"""CommandScene 起動経路が Launch 前に TryLoad しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"


def main() -> int:
    errors: list[str] = []
    boot_cpp = CODES / "Source" / "Scene" / "CommandSceneDevelopmentBoot.cpp"
    release_cpp = CODES / "Source" / "Scene" / "CommandSceneReleaseBootApply.cpp"
    gate_cpp = CODES / "Source" / "Scene" / "CommandSceneBootLaunchPathGate.cpp"

    if boot_cpp.is_file():
        text = boot_cpp.read_text(encoding="utf-8", errors="replace")
        resolver = text.split("ResolveDevelopmentBootTarget()", 1)
        if len(resolver) > 1:
            body = resolver[1].split("IsValidBootSceneJsonPath", 1)[0]
            if "TryLoadSceneDocument" in body:
                errors.append(
                    "ResolveDevelopmentBootTarget must not TryLoad before Launch"
                )
            if "CanLaunchBootSceneAtPath" not in body:
                errors.append(
                    "ResolveDevelopmentBootTarget must use CanLaunchBootSceneAtPath"
                )
    else:
        errors.append("missing CommandSceneDevelopmentBoot.cpp")

    if release_cpp.is_file():
        text = release_cpp.read_text(encoding="utf-8", errors="replace")
        if "IsValidBootSceneJsonPath" in text or "TryLoadSceneDocument" in text:
            errors.append("ReleaseBootApply must not TryLoad before Launch")
        if "CanLaunchBootSceneAtPath" not in text:
            errors.append("ReleaseBootApply must use CanLaunchBootSceneAtPath")
    else:
        errors.append("missing CommandSceneReleaseBootApply.cpp")

    if not gate_cpp.is_file():
        errors.append("missing CommandSceneBootLaunchPathGate.cpp")
    elif "TryLoad" in gate_cpp.read_text(encoding="utf-8", errors="replace"):
        errors.append("BootLaunchPathGate must not call TryLoad")

    if errors:
        print("verify_command_scene_launch_no_tryload: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_command_scene_launch_no_tryload: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())