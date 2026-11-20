#!/usr/bin/env python3
"""buildDevelopment が CI 同等の NuGet 復元を呼ぶか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILD_CMD = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "buildDevelopment.cmd"
RESTORE_CMD = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "restoreDevelopmentNuGetPackages.cmd"
DEV_YML = ROOT / ".github" / "workflows" / "DevelopmentBuild.yml"
PACKAGES_CONFIG = ROOT / "packages.config"
SDL_TARGETS_REL = (
    "Project/packages/sdl2.redist.2.0.5/build/native/sdl2.redist.targets"
)


def main() -> int:
    errors: list[str] = []

    if not PACKAGES_CONFIG.is_file():
        errors.append("missing repo root packages.config")

    if not RESTORE_CMD.is_file():
        errors.append("missing restoreDevelopmentNuGetPackages.cmd")
    else:
        restore = RESTORE_CMD.read_text(encoding="utf-8", errors="replace")
        if "packages.config" not in restore:
            errors.append("restore cmd must use packages.config")
        if "Project\\packages" not in restore and "Project/packages" not in restore:
            errors.append("restore cmd must use Project/packages")
        if "sdl2.redist.2.0.5" not in restore:
            errors.append("restore cmd must verify SDL2 targets")

    if not BUILD_CMD.is_file():
        errors.append("missing buildDevelopment.cmd")
    else:
        cmd = BUILD_CMD.read_text(encoding="utf-8", errors="replace")
        if "restoreDevelopmentNuGetPackages.cmd" not in cmd:
            errors.append("buildDevelopment.cmd must call restoreDevelopmentNuGetPackages.cmd")

    if DEV_YML.is_file():
        yml = DEV_YML.read_text(encoding="utf-8", errors="replace")
        if "nuget restore" not in yml.lower():
            errors.append("DevelopmentBuild.yml: expected nuget restore step")
        if "Project/packages" not in yml:
            errors.append("DevelopmentBuild.yml: expected Project/packages")
    else:
        errors.append("missing DevelopmentBuild.yml")

    if SDL_TARGETS_REL.replace("/", "\\") not in str(ROOT):
        pass  # path check is in restore script text only

    if errors:
        print("verify_build_development_nuget_restore: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_build_development_nuget_restore: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())