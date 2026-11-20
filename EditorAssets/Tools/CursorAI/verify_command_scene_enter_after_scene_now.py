#!/usr/bin/env python3
"""sceneNow 確定後の Enter が BootEnterApply 経由であることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"


def main() -> int:
    errors: list[str] = []
    command_cpp = CODES / "Source" / "Scene" / "CommandScene.cpp"
    enter_cpp = CODES / "Source" / "Scene" / "CommandSceneBootEnterApply.cpp"

    if command_cpp.is_file():
        text = command_cpp.read_text(encoding="utf-8", errors="replace")
        ctor = text.split("CommandScene::CommandScene()", 1)
        if len(ctor) > 1:
            chunk = ctor[1].split("}", 1)[0]
            if "PrepareInitialSceneBeforeEnter" not in chunk:
                errors.append("constructor must PrepareInitialSceneBeforeEnter")
            if "EnterInitialSceneNow" not in chunk:
                errors.append("constructor must EnterInitialSceneNow")
            if "sceneNow_->EnterScene()" in chunk.replace(" ", ""):
                errors.append("constructor must not call EnterScene directly")
    else:
        errors.append("missing CommandScene.cpp")

    if enter_cpp.is_file():
        text = enter_cpp.read_text(encoding="utf-8", errors="replace")
        if "IsValidBootSceneJsonPath" not in text:
            errors.append("BootEnterApply must validate JSON before Enter")
        if "SceneTitle::Instance()" not in text:
            errors.append("BootEnterApply must fallback to Title")
    else:
        errors.append("missing CommandSceneBootEnterApply.cpp")

    if errors:
        print("verify_command_scene_enter_after_scene_now: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_command_scene_enter_after_scene_now: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())