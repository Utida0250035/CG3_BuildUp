#!/usr/bin/env python3
"""launch_config の既定 startup が機能検証シーンを指すことを検査。"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
LAUNCH = ROOT / "EditorAssets" / "launch_config.json"
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"
BOOT_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "CommandSceneDevelopmentBoot.cpp"
DEFAULTS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorLaunchConfigFeatureVerificationDefaults.h"


def main() -> int:
    errors: list[str] = []

    if not LAUNCH.is_file():
        errors.append("missing EditorAssets/launch_config.json")
    else:
        try:
            data = json.loads(LAUNCH.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            errors.append("EditorAssets/launch_config.json: invalid JSON")
        else:
            path = data.get("startupScenePath", "")
            if "LoopTick13Demo" not in path:
                errors.append("startupScenePath must reference LoopTick13Demo")
            if "DebugPlayArea" not in path:
                errors.append("startupScenePath must be under DebugPlayArea")

    if not DEFAULTS_H.is_file():
        errors.append("missing EditorLaunchConfigFeatureVerificationDefaults.h")
    else:
        text = DEFAULTS_H.read_text(encoding="utf-8", errors="replace")
        if "DebugPlayAreaFeatureVerificationCanonical" not in text:
            errors.append("defaults must use feature verification canonical")

    diag_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorLaunchConfigLoadDiagnostics.cpp"
    if diag_cpp.is_file():
        text = diag_cpp.read_text(encoding="utf-8", errors="replace")
        if "kEditorLaunchConfigJson" not in text:
            errors.append("startup resolver must read EditorAssets launch_config")
    else:
        errors.append("missing EditorLaunchConfigLoadDiagnostics.cpp")
    if ASSET_CPP.is_file():
        text = ASSET_CPP.read_text(encoding="utf-8", errors="replace")
        if "EditorLaunchConfigLoadDiagnostics" not in text:
            errors.append("EditorSceneAsset must delegate to load diagnostics")
    else:
        errors.append("missing EditorSceneAsset.cpp")

    if BOOT_CPP.is_file():
        text = BOOT_CPP.read_text(encoding="utf-8", errors="replace")
        if "DebugPlayAreaFeatureVerificationCanonical::SceneJsonPath" not in text:
            errors.append("Development boot fallback must use feature verification scene")
        if "kDebugPlayAreaTemplateScene" in text:
            errors.append("Development boot must not fall back to scaffold template")
    else:
        errors.append("missing CommandSceneDevelopmentBoot.cpp")

    if errors:
        print("verify_launch_config_feature_startup: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_launch_config_feature_startup: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())