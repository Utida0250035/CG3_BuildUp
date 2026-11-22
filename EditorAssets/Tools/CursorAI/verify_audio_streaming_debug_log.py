#!/usr/bin/env python3
"""ストリーミング診断が LogFile へ書けることを静的検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    log_h = ROOT / "Project" / "Codes" / "Include" / "ForDebug" / "AudioStreamingDebugLog.h"
    imgui_cpp = ROOT / "Project" / "Codes" / "Source" / "ForDebug" / "AudioStreamingDebugImGui.cpp"

    if not log_h.is_file():
        errors.append("missing AudioStreamingDebugLog.h")

    if imgui_cpp.is_file():
        text = imgui_cpp.read_text(encoding="utf-8", errors="replace")
        if "AppendStreamingMemoryDebugSnapshotToLog" not in text:
            errors.append("AudioStreamingDebugImGui: expected log append call")
        if "スナップショットをログへ" not in text:
            errors.append("AudioStreamingDebugImGui: expected log export button")

    if errors:
        print("verify_audio_streaming_debug_log: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_audio_streaming_debug_log: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())