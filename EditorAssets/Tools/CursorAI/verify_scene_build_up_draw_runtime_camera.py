#!/usr/bin/env python3
"""DrawRuntimeViewContent が Prepare 後に SetupStageViewCamera しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
BIND_CPP = (
    ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpRuntimeSubviewDrawCamera.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not BIND_CPP.is_file():
        errors.append("missing SceneBuildUpRuntimeSubviewDrawCamera.cpp")
    else:
        text = BIND_CPP.read_text(encoding="utf-8", errors="replace")
        if "SetupStageViewCamera" in text:
            errors.append("subview bind must not call SetupStageViewCamera")
        if "PrepareCamerasForDraw" not in text:
            errors.append("subview bind must call PrepareCamerasForDraw")

    if BUILDUP.is_file():
        chunk = BUILDUP.read_text(encoding="utf-8", errors="replace")
        rt = chunk.split("DrawRuntimeViewContent", 1)
        if len(rt) < 2:
            errors.append("missing DrawRuntimeViewContent")
        else:
            body = rt[1].split("void SceneBuildUp::", 1)[0]
            if "MaintainLockedStageCamera" in body:
                errors.append("DrawRuntimeViewContent must not MaintainLockedStageCamera")
            if "SetupStageViewCamera" in body:
                errors.append("DrawRuntimeViewContent must not SetupStageViewCamera")
            if "BindAfterGameOrderPrepare" not in body:
                errors.append("DrawRuntimeViewContent must use BindAfterGameOrderPrepare")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_draw_runtime_camera: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_draw_runtime_camera: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())