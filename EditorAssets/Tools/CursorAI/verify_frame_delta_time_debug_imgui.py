#!/usr/bin/env python3
"""FrameDeltaTime の ImGui オーバーレイ／ヒストグラムが配線されていることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    debug_cpp = ROOT / "Project" / "Codes" / "Source" / "ForDebug" / "FrameDeltaTimeDebugImGui.cpp"
    gameorder_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "GameOrder.h"
    frame_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorFrame.cpp"

    if not debug_cpp.is_file():
        errors.append("missing FrameDeltaTimeDebugImGui.cpp")
    else:
        text = debug_cpp.read_text(encoding="utf-8", errors="replace")
        if "EngineFrameDeltaTime" not in text:
            errors.append("debug ui must read EngineFrameDeltaTime")
        if "PlotHistogram" not in text:
            errors.append("debug ui must include histogram")
        if "FrameDeltaOverlay" not in text:
            errors.append("debug ui must include overlay")

    if gameorder_h.is_file():
        if "DrawFrameDeltaTimeDebug" not in gameorder_h.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("GameOrder must call DrawFrameDeltaTimeDebug")

    if frame_cpp.is_file():
        if "EditorFrameFrameDeltaDiagnostics" not in frame_cpp.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("EditorFrame must wire frame delta diagnostics menu")

    if errors:
        print("verify_frame_delta_time_debug_imgui: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_frame_delta_time_debug_imgui: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())