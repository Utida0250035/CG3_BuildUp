#!/usr/bin/env python3
"""本番相当プロファイル計測の構成／手順がリポジトリに定義されているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MEMO = (
    ROOT
    / "EditorAssets"
    / "Tools"
    / "CursorAI"
    / "docs"
    / "feature-memos"
    / "development_opt_profile.md"
)
BUILD_CMD = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "buildDevelopmentProfile.cmd"
FRAME_MEMO = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "docs" / "feature-memos" / "frame_profiler.md"
SHIP_MEMO = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "docs" / "feature-memos" / "ship_build_configurations.md"


def main() -> int:
    errors: list[str] = []

    if not MEMO.is_file():
        errors.append("missing development_opt_profile.md")
    else:
        text = MEMO.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "DBG-Profiler-001",
            "buildDevelopmentProfile.cmd",
            "buildDevelopment.cmd",
            "Release",
            "MinSpace",
        ):
            if needle not in text:
                errors.append(f"development_opt_profile.md must mention: {needle}")

    if not BUILD_CMD.is_file():
        errors.append("missing buildDevelopmentProfile.cmd")
    else:
        cmd = BUILD_CMD.read_text(encoding="utf-8", errors="replace")
        if "ClCompile_Optimization=MinSpace" not in cmd:
            errors.append("buildDevelopmentProfile.cmd must set ClCompile_Optimization=MinSpace")
        if "Configuration=Development" not in cmd:
            errors.append("buildDevelopmentProfile.cmd must use Development configuration")

    if FRAME_MEMO.is_file():
        if "development_opt_profile.md" not in FRAME_MEMO.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("frame_profiler.md should link development_opt_profile.md")
    else:
        errors.append("missing frame_profiler.md")

    if not SHIP_MEMO.is_file():
        errors.append("missing ship_build_configurations.md")

    if errors:
        print("verify_development_opt_profile: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_development_opt_profile: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())