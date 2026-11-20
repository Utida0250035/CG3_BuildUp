#!/usr/bin/env python3
"""launch_config JSON が schemaVersion と機能検証既定パスを含むことを検査。"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
REPO_LAUNCH = ROOT / "EditorAssets" / "launch_config.json"
DOC_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorLaunchConfigJsonDocument.cpp"
SCHEMA_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorLaunchConfigSchema.h"
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"


def main() -> int:
    errors: list[str] = []

    for key in ("schemaVersion", "defaultFeatureVerificationScenePath"):
        if not SCHEMA_H.is_file() or key not in SCHEMA_H.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append(f"EditorLaunchConfigSchema must define {key}")

    if not DOC_CPP.is_file():
        errors.append("missing EditorLaunchConfigJsonDocument.cpp")
    else:
        doc = DOC_CPP.read_text(encoding="utf-8", errors="replace")
        if "BuildDocument" not in doc:
            errors.append("BuildDocument required")
        if "kDefaultFeatureVerificationScenePathKey" not in doc:
            errors.append("save must write defaultFeatureVerificationScenePath")

    if ASSET_CPP.is_file():
        if "LaunchConfigJsonDocument::BuildDocument" not in ASSET_CPP.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("TrySaveLaunchConfig must use BuildDocument")
        if 'json["startupScenePath"]' in ASSET_CPP.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("TrySaveLaunchConfig must not assign startupScenePath only")
    else:
        errors.append("missing EditorSceneAsset.cpp")

    if not REPO_LAUNCH.is_file():
        errors.append("missing EditorAssets/launch_config.json")
    else:
        try:
            data = json.loads(REPO_LAUNCH.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            errors.append("EditorAssets/launch_config.json invalid")
        else:
            if data.get("schemaVersion") != 1:
                errors.append("repo launch_config schemaVersion must be 1")
            if "defaultFeatureVerificationScenePath" not in data:
                errors.append("repo launch_config missing defaultFeatureVerificationScenePath")

    if errors:
        print("verify_launch_config_schema_version: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_launch_config_schema_version: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())