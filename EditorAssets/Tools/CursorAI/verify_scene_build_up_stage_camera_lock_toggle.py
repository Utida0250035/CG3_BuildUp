#!/usr/bin/env python3
"""SceneBuildUp の StageView ロックが定数 true でないことを検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HEADER = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneBuildUp.h"
CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
LOCK_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpStageCameraLock.cpp"


def main() -> int:
    errors: list[str] = []

    if HEADER.is_file():
        text = HEADER.read_text(encoding="utf-8", errors="replace")
        if re.search(
            r"IsStageViewCameraLocked\s*\(\)\s*const\s*override\s*\{\s*return\s+true",
            text,
        ):
            errors.append("SceneBuildUp.h must not hardcode IsStageViewCameraLocked true")
        if "stageViewCameraLocked_" not in text:
            errors.append("SceneBuildUp.h needs stageViewCameraLocked_ member")
    else:
        errors.append("missing SceneBuildUp.h")

    if CPP.is_file():
        text = CPP.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUp::IsStageViewCameraLocked" not in text:
            errors.append("SceneBuildUp.cpp must implement IsStageViewCameraLocked")
        if "TryToggleLockHotkey" not in text:
            errors.append("SceneBuildUp::Update should wire F5 toggle (Development)")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if not LOCK_CPP.is_file():
        errors.append("missing SceneBuildUpStageCameraLock.cpp")

    if errors:
        print("verify_scene_build_up_stage_camera_lock_toggle: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_stage_camera_lock_toggle: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())