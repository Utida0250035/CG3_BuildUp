#!/usr/bin/env python3
"""SceneBuildUp が cameraRig 自動スポーン実体を Update することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
RIG = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneCameraRig.cpp"


def main() -> int:
    errors: list[str] = []

    if RIG.is_file():
        if "TickSpawnedEntities" not in RIG.read_text(encoding="utf-8", errors="replace"):
            errors.append("SceneCameraRig must implement TickSpawnedEntities")
    else:
        errors.append("missing SceneCameraRig.cpp")

    if BUILDUP.is_file():
        update = BUILDUP.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::Update()", 1
        )
        if len(update) > 1:
            body = update[1].split("void SceneBuildUp::", 1)[0]
            if "TickCameraRigEntities" not in body:
                errors.append("SceneBuildUp::Update must TickCameraRigEntities")
        else:
            errors.append("missing SceneBuildUp::Update")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_camera_rig_update: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_camera_rig_update: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())