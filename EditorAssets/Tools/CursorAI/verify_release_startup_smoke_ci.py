#!/usr/bin/env python3
"""Release 起動スモークが CI とコードに配線されているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    yml = ROOT / ".github" / "workflows" / "ReleaseBuild.yml"
    ps1 = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "runReleaseStartupSmoke.ps1"
    smoke_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Application"
        / "ReleaseStartupSmokeFrames.cpp"
    )
    main_cpp = ROOT / "Project" / "Codes" / "Source" / "Application" / "main.cpp"
    memo = (
        ROOT
        / "EditorAssets"
        / "Tools"
        / "CursorAI"
        / "docs"
        / "feature-memos"
        / "release_startup_smoke.md"
    )

    if not ps1.is_file():
        errors.append("missing runReleaseStartupSmoke.ps1")

    if not yml.is_file():
        errors.append("missing ReleaseBuild.yml")
    elif "runReleaseStartupSmoke.ps1" not in yml.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("ReleaseBuild.yml must run release startup smoke")

    if not smoke_cpp.is_file():
        errors.append("missing ReleaseStartupSmokeFrames.cpp")
    if main_cpp.is_file():
        text = main_cpp.read_text(encoding="utf-8", errors="replace")
        if "ReadReleaseStartupSmokeFrameBudget" not in text:
            errors.append("main.cpp must honor smoke frame budget")
    else:
        errors.append("missing main.cpp")

    if not memo.is_file():
        errors.append("missing release_startup_smoke.md")

    if errors:
        print("verify_release_startup_smoke_ci: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_release_startup_smoke_ci: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())