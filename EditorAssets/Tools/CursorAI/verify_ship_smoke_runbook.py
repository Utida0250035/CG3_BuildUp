#!/usr/bin/env python3
"""Release スモーク手順書が README から辿れるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
README = ROOT / "README.md"
RUNBOOK = (
    ROOT
    / "EditorAssets"
    / "Tools"
    / "CursorAI"
    / "docs"
    / "feature-memos"
    / "smoke_runbook.md"
)
PS1 = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "runReleaseStartupSmoke.ps1"


def main() -> int:
    errors: list[str] = []

    if not RUNBOOK.is_file():
        errors.append("missing smoke_runbook.md")
    else:
        text = RUNBOOK.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "Release|x64",
            "runReleaseStartupSmoke.ps1",
            "Generated/Outputs/Release",
            "ATRUM_RELEASE_SMOKE_FRAMES",
            "Title",
        ):
            if needle not in text:
                errors.append(f"smoke_runbook.md must mention: {needle}")

    if not README.is_file():
        errors.append("missing README.md")
    else:
        readme = README.read_text(encoding="utf-8", errors="replace")
        if "smoke_runbook.md" not in readme:
            errors.append("README must link to smoke_runbook.md")
        if "runReleaseStartupSmoke.ps1" not in readme:
            errors.append("README must mention runReleaseStartupSmoke.ps1")

    if not PS1.is_file():
        errors.append("missing runReleaseStartupSmoke.ps1")

    if errors:
        print("verify_ship_smoke_runbook: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_ship_smoke_runbook: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())