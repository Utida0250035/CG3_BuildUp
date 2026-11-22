#!/usr/bin/env python3
"""EditorFrame にストリーミング診断メニューが配線されていることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    frame_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorFrame.cpp"
    stream_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "EditorFrameStreamingDiagnostics.cpp"
    )

    if not stream_cpp.is_file():
        errors.append("missing EditorFrameStreamingDiagnostics.cpp")

    if frame_cpp.is_file():
        text = frame_cpp.read_text(encoding="utf-8", errors="replace")
        if "EditorFrameStreamingDiagnostics::DrawMenuItems" not in text:
            errors.append("EditorFrame: expected streaming menu in diagnostics")
        if 'BeginMenu("デバッグ")' not in text:
            errors.append("EditorFrame: expected デバッグ menu in diagnostics")

    if errors:
        print("verify_editor_frame_streaming_menu: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_frame_streaming_menu: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())