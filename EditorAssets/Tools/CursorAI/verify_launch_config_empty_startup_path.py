#!/usr/bin/env python3
"""launch_config の startupScenePath 空文字がキー欠落と区別されることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DIAG_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorLaunchConfigLoadDiagnostics.cpp"
DIAG_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorLaunchConfigLoadDiagnostics.h"
ASSET_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Scene" / "EditorSceneAsset.h"
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"


def main() -> int:
    errors: list[str] = []

    if not DIAG_H.is_file():
        errors.append("missing EditorLaunchConfigLoadDiagnostics.h")
    else:
        h = DIAG_H.read_text(encoding="utf-8", errors="replace")
        if "StartupPathEmpty" not in h:
            errors.append("LaunchConfigLoadIssue must include StartupPathEmpty")

    if not DIAG_CPP.is_file():
        errors.append("missing diagnostics cpp")
    else:
        cpp = DIAG_CPP.read_text(encoding="utf-8", errors="replace")
        if "StartupPathEmpty" not in cpp:
            errors.append("loader must detect empty startupScenePath string")
        if "StartupPathEmpty" not in cpp.split("DescribeLaunchConfigLoadIssueJa", 1)[-1]:
            errors.append("DescribeLaunchConfigLoadIssueJa must cover StartupPathEmpty")
        if (
            "StartupPathKeyMissing" in cpp
            and cpp.count("StartupPathKeyMissing") >= 2
            and "startupScenePath.empty()" not in cpp
        ):
            errors.append("empty check required separate from key missing")

    if ASSET_H.is_file():
        if "TryLoadLaunchConfigWithIssue" not in ASSET_H.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("TryLoadLaunchConfigWithIssue must be public API")
    else:
        errors.append("missing EditorSceneAsset.h")

    if ASSET_CPP.is_file():
        if "TryLoadLaunchConfigWithIssue" not in ASSET_CPP.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("TryLoadLaunchConfigWithIssue must be implemented")
    else:
        errors.append("missing EditorSceneAsset.cpp")

    if errors:
        print("verify_launch_config_empty_startup_path: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_launch_config_empty_startup_path: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())