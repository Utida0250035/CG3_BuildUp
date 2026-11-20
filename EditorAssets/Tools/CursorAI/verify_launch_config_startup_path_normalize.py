#!/usr/bin/env python3
"""launch_config 保存時に startupScenePath が正規化されることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
NORM_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorLaunchConfigStartupPathNormalize.cpp"
ASSET_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"


def main() -> int:
    errors: list[str] = []

    if not NORM_CPP.is_file():
        errors.append("missing EditorLaunchConfigStartupPathNormalize.cpp")
    else:
        text = NORM_CPP.read_text(encoding="utf-8", errors="replace")
        if "lexically_normal" not in text:
            errors.append("normalize must use lexically_normal")

    if not ASSET_CPP.is_file():
        errors.append("missing EditorSceneAsset.cpp")
    else:
        text = ASSET_CPP.read_text(encoding="utf-8", errors="replace")
        if "NormalizeForLaunchConfig" not in text:
            errors.append("EditorSceneAsset must normalize startup paths")
        save = text.split("TrySaveLaunchConfig", 1)
        if len(save) > 1 and "NormalizeForLaunchConfig" not in save[1].split("TrySetRepositoryStartupScene", 1)[0]:
            errors.append("TrySaveLaunchConfig must normalize before save")
        set_repo = text.split("TrySetRepositoryStartupScene", 1)
        if len(set_repo) > 1:
            body = set_repo[1].split("ResolveStartupScenePath", 1)[0]
            if "config.startupScenePath = sceneJsonPath" in body.replace(" ", ""):
                errors.append("TrySetRepositoryStartupScene must not assign raw path")
            if "Set(sceneJsonPath)" in body and "Set(normalized)" not in body:
                errors.append("PlayRequest Set must use normalized path")

    if errors:
        print("verify_launch_config_startup_path_normalize: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_launch_config_startup_path_normalize: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())