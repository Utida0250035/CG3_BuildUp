#!/usr/bin/env python3
"""launch_config 読込失敗が原因別に診断できることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DIAG_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorLaunchConfigLoadDiagnostics.cpp"
DIAG_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorLaunchConfigLoadDiagnostics.h"
UI_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"


def main() -> int:
    errors: list[str] = []

    required_issues = (
        "ConfigFileMissing",
        "ConfigJsonInvalid",
        "StartupPathKeyMissing",
        "StartupPathEmpty",
        "StartupSceneFileMissing",
    )

    if not DIAG_H.is_file() or not DIAG_CPP.is_file():
        errors.append("missing EditorLaunchConfigLoadDiagnostics")
    else:
        h = DIAG_H.read_text(encoding="utf-8", errors="replace")
        cpp = DIAG_CPP.read_text(encoding="utf-8", errors="replace")
        for issue in required_issues:
            if issue not in h:
                errors.append(f"LaunchConfigLoadIssue missing {issue}")
        if "DescribeLaunchConfigLoadIssueJa" not in cpp:
            errors.append("missing DescribeLaunchConfigLoadIssueJa")
        if "ResolveStartupScenePathDetailed" not in cpp:
            errors.append("missing ResolveStartupScenePathDetailed")

    if UI_CPP.is_file():
        ui = UI_CPP.read_text(encoding="utf-8", errors="replace")
        if "ResolveStartupScenePathDetailed" not in ui:
            errors.append("EntityEditorUi must show detailed launch failure")
        if "launch_config 未設定" in ui:
            errors.append("UI must not use vague launch_config unset message only")
        if "DescribeLaunchConfigLoadIssueJa" not in ui:
            errors.append("UI must call DescribeLaunchConfigLoadIssueJa")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if ASSET_CPP.is_file():
        text = ASSET_CPP.read_text(encoding="utf-8", errors="replace")
        if "EditorLaunchConfigLoadDiagnostics" not in text:
            errors.append("EditorSceneAsset must use load diagnostics")
        if "ResolveStartupScenePathDetailed" not in text:
            errors.append("ResolveStartupScenePath must use detailed resolver")
    else:
        errors.append("missing EditorSceneAsset.cpp")

    if errors:
        print("verify_launch_config_load_diagnostics: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_launch_config_load_diagnostics: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())