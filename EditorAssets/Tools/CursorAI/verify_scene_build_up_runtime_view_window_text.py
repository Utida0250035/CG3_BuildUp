#!/usr/bin/env python3
"""BuildUp 専用ランタイムウィンドウが JSON パス付きステータスを出すことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
STATUS_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpRuntimeViewStatusText.cpp"
WINDOW_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpDedicatedRuntimeViewWindow.cpp"
BUILDUP_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
GENERIC_ONLY = "シミュレーション実行"


def main() -> int:
    errors: list[str] = []

    if STATUS_CPP.is_file():
        text = STATUS_CPP.read_text(encoding="utf-8", errors="replace")
        if "SceneJsonPath" not in text or "SceneDisplayName" not in text:
            errors.append("status text must include scene path and display name")
    else:
        errors.append("missing SceneBuildUpRuntimeViewStatusText.cpp")

    if WINDOW_CPP.is_file():
        text = WINDOW_CPP.read_text(encoding="utf-8", errors="replace")
        if "BuildSceneBuildUpRuntimeViewStatusLine" not in text:
            errors.append("dedicated window must use dynamic status line")
        if "LayoutDedicatedSceneViewWindow" not in text:
            errors.append("dedicated window must use LayoutDedicatedSceneViewWindow")
        if GENERIC_ONLY in text:
            errors.append("dedicated window must not use generic simulation-only text")
    else:
        errors.append("missing SceneBuildUpDedicatedRuntimeViewWindow.cpp")

    if BUILDUP_CPP.is_file():
        draw = BUILDUP_CPP.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::Draw()", 1
        )
        if len(draw) < 2 or "LayoutAndRenderViews" not in draw[1].split(
            "void SceneBuildUp::", 1
        )[0]:
            errors.append("Draw must call LayoutAndRenderViews")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_runtime_view_window_text: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_runtime_view_window_text: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())