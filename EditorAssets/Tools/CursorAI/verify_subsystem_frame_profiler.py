#!/usr/bin/env python3
"""サブシステム別フレームプロファイラの配線を静的検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    profiler_h = ROOT / "Project" / "Codes" / "Include" / "ForDebug" / "SubsystemFrameProfiler.h"
    game_order = ROOT / "Project" / "Codes" / "Include" / "Scene" / "GameOrder.h"
    main_cpp = ROOT / "Project" / "Codes" / "Source" / "Application" / "main.cpp"
    engine_cpp = ROOT / "Project" / "Codes" / "Source" / "Engine" / "AtrumEngine.cpp"
    scene_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "Scene.cpp"
    memo = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "docs" / "feature-memos" / "frame_profiler.md"

    if not profiler_h.is_file():
        errors.append("missing SubsystemFrameProfiler.h")

    for path, token in (
        (game_order, "SubsystemFrameProfilerBeginFrame"),
        (main_cpp, "SubsystemFrameId::ProcessPump"),
        (engine_cpp, "SubsystemFrameId::GfxPreDraw"),
        (scene_cpp, "SubsystemFrameId::GfxSceneDraw"),
    ):
        if not path.is_file():
            errors.append(f"missing {path.name}")
        elif token not in path.read_text(encoding="utf-8", errors="replace"):
            errors.append(f"{path.name}: expected {token}")

    if memo.is_file():
        if "GfxPostDraw" not in memo.read_text(encoding="utf-8", errors="replace"):
            errors.append("frame_profiler.md incomplete")
    else:
        errors.append("missing frame_profiler.md")

    if errors:
        print("verify_subsystem_frame_profiler: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_subsystem_frame_profiler: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())