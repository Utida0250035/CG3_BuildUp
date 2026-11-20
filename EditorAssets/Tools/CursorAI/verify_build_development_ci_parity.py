#!/usr/bin/env python3
"""buildDevelopment.cmd が CI DevelopmentBuild と同じ入口・前検証か検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILD_CMD = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "buildDevelopment.cmd"
DEV_YML = ROOT / ".github" / "workflows" / "DevelopmentBuild.yml"
MEMO = (
    ROOT
    / "EditorAssets"
    / "Tools"
    / "CursorAI"
    / "docs"
    / "feature-memos"
    / "cloud_build_vcxproj_check.md"
)


def main() -> int:
    errors: list[str] = []

    if not BUILD_CMD.is_file():
        errors.append("missing buildDevelopment.cmd")
    else:
        cmd = BUILD_CMD.read_text(encoding="utf-8", errors="replace")
        if "verify_vcxproj_coverage.py" not in cmd:
            errors.append("buildDevelopment.cmd must run verify_vcxproj_coverage.py")
        if "CG2_BuildUp.slnx" not in cmd:
            errors.append("buildDevelopment.cmd must build CG2_BuildUp.slnx")
        if "CG2_BuildUp.vcxproj" in cmd and "CG2_BuildUp.slnx" not in cmd:
            errors.append("buildDevelopment.cmd must not use vcxproj-only entry")

    if DEV_YML.is_file():
        yml = DEV_YML.read_text(encoding="utf-8", errors="replace")
        if "verify_vcxproj_coverage.py" not in yml:
            errors.append("DevelopmentBuild.yml: expected vcxproj verify job")
        if "CG2_BuildUp.slnx" not in yml:
            errors.append("DevelopmentBuild.yml: expected slnx build")
    else:
        errors.append("missing DevelopmentBuild.yml")

    if MEMO.is_file():
        memo = MEMO.read_text(encoding="utf-8", errors="replace")
        if "buildDevelopment.cmd" not in memo:
            errors.append("cloud_build_vcxproj_check.md must document buildDevelopment.cmd")
    else:
        errors.append("missing cloud_build_vcxproj_check.md")

    if errors:
        print("verify_build_development_ci_parity: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_build_development_ci_parity: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())