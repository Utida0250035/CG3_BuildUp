#!/usr/bin/env python3
"""BuildUp ctor が JSON sceneName を即反映することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
RESOLVE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpSceneDisplayNameResolve.cpp"


def main() -> int:
    errors: list[str] = []

    if not RESOLVE.is_file():
        errors.append("missing SceneBuildUpSceneDisplayNameResolve.cpp")
    else:
        text = RESOLVE.read_text(encoding="utf-8", errors="replace")
        if '"sceneName"' not in text:
            errors.append("resolver must read sceneName from JSON")
        if "SceneDisplayNameFromPath" not in text:
            errors.append("resolver must fall back to path stem")

    if BUILDUP.is_file():
        ctor = BUILDUP.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::SceneBuildUp", 1
        )
        if len(ctor) < 2:
            errors.append("missing SceneBuildUp ctor")
        elif "ResolveInitialSceneDisplayName" not in ctor[1].split(
            "SceneBuildUp::~SceneBuildUp", 1
        )[0]:
            errors.append("ctor must use ResolveInitialSceneDisplayName")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_display_name_resolve: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_display_name_resolve: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())