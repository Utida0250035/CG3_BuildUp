#!/usr/bin/env python3
"""遷移演出中も BuildUp が F1 復帰入力を受け付けることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    game_order = ROOT / "Project" / "Codes/Include/Scene/GameOrder.h"
    scene_h = ROOT / "Project" / "Codes/Include/Scene/Scene.h"
    buildup_h = ROOT / "Project" / "Codes/Include/Scene/SceneBuildUp.h"
    f1_cpp = ROOT / "Project" / "Codes/Source/Scene/SceneBuildUpF1ReturnRequest.cpp"

    if game_order.is_file():
        text = game_order.read_text(encoding="utf-8", errors="replace")
        if "TryHandleInputWhileSceneTransitionBlocked" not in text:
            errors.append("GameOrder must call TryHandleInputWhileSceneTransitionBlocked")
        if "AllowsCurrentSceneUpdate" not in text:
            errors.append("GameOrder must gate on AllowsCurrentSceneUpdate")
    else:
        errors.append("missing GameOrder.h")

    if scene_h.is_file():
        if "TryHandleInputWhileSceneTransitionBlocked" not in scene_h.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("Scene needs TryHandleInputWhileSceneTransitionBlocked")
    else:
        errors.append("missing Scene.h")

    if buildup_h.is_file():
        if "TryHandleInputWhileSceneTransitionBlocked" not in buildup_h.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("SceneBuildUp must override transition-block input")
    else:
        errors.append("missing SceneBuildUp.h")

    if not f1_cpp.is_file():
        errors.append("missing SceneBuildUpF1ReturnRequest.cpp")

    if errors:
        print("verify_scene_build_up_f1_during_fade: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_f1_during_fade: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())