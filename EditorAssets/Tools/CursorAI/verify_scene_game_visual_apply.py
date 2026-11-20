#!/usr/bin/env python3
"""SceneGame が DrawScene 毎に lighting profile を再適用していないことを検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE_GAME_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneGame.cpp"


def main() -> int:
    errors: list[str] = []
    if not SCENE_GAME_CPP.is_file():
        errors.append("missing SceneGame.cpp")
    else:
        text = SCENE_GAME_CPP.read_text(encoding="utf-8", errors="replace")
        draw_match = re.search(
            r"void\s+SceneGame::DrawScene\s*\(\)\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\}",
            text,
            re.DOTALL,
        )
        if draw_match and "ApplySceneGameLightingFromProfile" in draw_match.group(1):
            errors.append(
                "SceneGame::DrawScene must not call ApplySceneGameLightingFromProfile"
            )
        if "ApplySceneGameVisualPresetOnEnter" not in text:
            errors.append(
                "SceneGame::EnterScene expected ApplySceneGameVisualPresetOnEnter"
            )

    if errors:
        print("verify_scene_game_visual_apply: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_game_visual_apply: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())