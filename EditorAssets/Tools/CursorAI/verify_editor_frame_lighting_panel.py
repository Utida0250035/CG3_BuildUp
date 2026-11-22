#!/usr/bin/env python3
"""EditorFrame 経由で描画診断ライティングパネルが配線されているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    frame_h = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorFrame.h"
    diag_cpp = (
        ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorFrameLightingDiagnostics.cpp"
    )
    workbench = (
        ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"
    )
    if not frame_h.is_file():
        errors.append("missing EditorFrame.h")
    else:
        text = frame_h.read_text(encoding="utf-8", errors="replace")
        if "DrawRenderDiagnosticsMenuItems" not in text:
            errors.append("EditorFrame.h: expected DrawRenderDiagnosticsMenuItems")

    if not diag_cpp.is_file():
        errors.append("missing EditorFrameLightingDiagnostics.cpp")

    if workbench.is_file():
        text = workbench.read_text(encoding="utf-8", errors="replace")
        if "DrawRenderDiagnosticsMenuItems" not in text:
            errors.append("EditorEntityWorkbench: expected EditorFrame lighting menu wire")

    if errors:
        print("verify_editor_frame_lighting_panel: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_frame_lighting_panel: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())