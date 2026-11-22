#!/usr/bin/env python3
"""Lighting::ImGui が GameOrder 経由で配線されているかの静的検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    game_order = ROOT / "Project" / "Codes" / "Include" / "Scene" / "GameOrder.h"
    if not game_order.is_file():
        errors.append("missing GameOrder.h")
    else:
        text = game_order.read_text(encoding="utf-8", errors="replace")
        if "DrawLightingEffectsDebugWindow" not in text:
            errors.append("GameOrder.h: expected DrawLightingEffectsDebugWindow()")

    panel_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Engine"
        / "Renderer"
        / "Lighting"
        / "LightingEffectsImGuiPanel.cpp"
    )
    if not panel_cpp.is_file():
        errors.append("missing LightingEffectsImGuiPanel.cpp")

    lighting_cpp = (
        ROOT / "Project" / "Codes" / "Source" / "Engine" / "Renderer" / "Lighting" / "Lighting.cpp"
    )
    if lighting_cpp.is_file():
        text = lighting_cpp.read_text(encoding="utf-8", errors="replace")
        if "DrawLightingEffectsImGuiPanel" not in text:
            errors.append("Lighting.cpp: expected DrawLightingEffectsImGuiPanel")

    if errors:
        print("verify_lighting_imgui_wire: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_lighting_imgui_wire: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())