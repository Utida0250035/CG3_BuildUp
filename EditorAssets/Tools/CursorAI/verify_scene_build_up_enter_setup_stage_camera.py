#!/usr/bin/env python3
"""SceneBuildUp Enter でロード直後に SetupStageViewCamera することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
FRAMING = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpStageCameraFraming.cpp"


def main() -> int:
    errors: list[str] = []

    if FRAMING.is_file():
        text = FRAMING.read_text(encoding="utf-8", errors="replace")
        after_load = text.split("ApplyStageCameraAfterSceneLoad", 1)
        if len(after_load) < 2:
            errors.append("missing ApplyStageCameraAfterSceneLoad")
        elif "SetupStageViewCamera" not in after_load[1].split(
            "MaintainLockedStageCamera", 1
        )[0]:
            errors.append("after-load path must call SetupStageViewCamera")
        if "ApplyStageCameraAfterSceneLoad" in text and (
            "PrepareCamerasForDraw" in text.split("ApplyStageCameraAfterSceneLoad", 1)[1].split(
                "MaintainLockedStageCamera", 1
            )[0]
        ):
            errors.append("Enter load path must not call PrepareCamerasForDraw")
    else:
        errors.append("missing SceneBuildUpStageCameraFraming.cpp")

    if BUILDUP.is_file():
        enter = BUILDUP.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::EnterScene()", 1
        )
        if len(enter) > 1:
            body = enter[1].split("void SceneBuildUp::", 1)[0]
            if "ApplyStageCameraAfterSceneLoad" not in body:
                errors.append("EnterScene must ApplyStageCameraAfterSceneLoad")
            if "MaintainLockedStageCamera" in body:
                errors.append("EnterScene must not call MaintainLockedStageCamera")
        else:
            errors.append("missing EnterScene")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_enter_setup_stage_camera: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_enter_setup_stage_camera: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())