#!/usr/bin/env python3
"""Release スモーク用 CLI --smoke-frames が main に配線されているか検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MAIN = ROOT / "Project" / "Codes" / "Source" / "Application" / "main.cpp"
PARSE_CPP = ROOT / "Project" / "Codes" / "Source" / "Application" / "ReleaseStartupSmokeCliParse.cpp"
RESOLVE_CPP = (
    ROOT / "Project" / "Codes" / "Source" / "Application" / "ReleaseStartupSmokeFrameBudgetResolve.cpp"
)
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
PS1 = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "runReleaseStartupSmoke.ps1"


def main() -> int:
    errors: list[str] = []

    for path in (PARSE_CPP, RESOLVE_CPP):
        if not path.is_file():
            errors.append(f"missing {path.name}")

    if not MAIN.is_file():
        errors.append("missing main.cpp")
    else:
        text = MAIN.read_text(encoding="utf-8", errors="replace")
        if "lpCmdLine" not in text:
            errors.append("main.cpp must use lpCmdLine")
        if "ResolveReleaseStartupSmokeFrameBudget" not in text:
            errors.append("main.cpp must call ResolveReleaseStartupSmokeFrameBudget")

    if PARSE_CPP.is_file():
        parse = PARSE_CPP.read_text(encoding="utf-8", errors="replace")
        if "--smoke-frames" not in parse:
            errors.append("cli parse must support --smoke-frames")

    if VCPROJ.is_file():
        vcx = VCPROJ.read_text(encoding="utf-8", errors="replace")
        for name in ("ReleaseStartupSmokeCliParse.cpp", "ReleaseStartupSmokeFrameBudgetResolve.cpp"):
            if name not in vcx:
                errors.append(f"vcxproj missing {name}")

    if PS1.is_file():
        ps = PS1.read_text(encoding="utf-8", errors="replace")
        if "--smoke-frames" not in ps and "smoke-frames" not in ps:
            errors.append("runReleaseStartupSmoke.ps1 should pass --smoke-frames to exe")
    else:
        errors.append("missing runReleaseStartupSmoke.ps1")

    if errors:
        print("verify_release_startup_smoke_cli: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_release_startup_smoke_cli: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())