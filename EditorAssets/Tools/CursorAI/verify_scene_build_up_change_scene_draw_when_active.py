#!/usr/bin/env python3
"""BuildUp が遷移中のみ ChangeScene を描画することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
POLICY = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpChangeSceneDrawPolicy.cpp"


def main() -> int:
    errors: list[str] = []

    if not POLICY.is_file():
        errors.append("missing SceneBuildUpChangeSceneDrawPolicy.cpp")
    else:
        text = POLICY.read_text(encoding="utf-8", errors="replace")
        if "GetIsExecute" not in text:
            errors.append("policy must gate on GetIsExecute")
        if "Draw()" not in text:
            errors.append("policy must call Draw when active")

    if BUILDUP.is_file():
        draw = BUILDUP.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::Draw()", 1
        )
        if len(draw) < 2:
            errors.append("missing Draw")
        else:
            body = draw[1].split("void SceneBuildUp::", 1)[0]
            if "DrawTransitionWhenActive" not in body:
                errors.append("Draw must call DrawTransitionWhenActive")
            if "CommandChangeScene::Instance()->Draw()" in body:
                errors.append("Draw must not always call CommandChangeScene::Draw")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_change_scene_draw_when_active: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_change_scene_draw_when_active: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())