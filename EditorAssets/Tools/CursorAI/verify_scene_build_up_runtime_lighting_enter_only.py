#!/usr/bin/env python3
"""BuildUp が描画パスで毎フレーム SetupRuntimeLighting しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
POLICY = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpRuntimeLightingPolicy.cpp"


def main() -> int:
    errors: list[str] = []

    if BUILDUP.is_file():
        text = BUILDUP.read_text(encoding="utf-8", errors="replace")
        if "SetupRuntimeLighting" in text:
            errors.append("SceneBuildUp.cpp must not reference SetupRuntimeLighting")
        enter = text.split("SceneBuildUp::EnterScene()", 1)
        if len(enter) < 2:
            errors.append("missing EnterScene")
        elif "ApplyOnceOnEnterScene" not in enter[1].split("void SceneBuildUp::", 1)[0]:
            errors.append("EnterScene must call ApplyOnceOnEnterScene")

        for fn in ("DrawScene", "DrawRuntimeViewContent"):
            chunk = text.split(f"SceneBuildUp::{fn}", 1)
            if len(chunk) < 2:
                if fn == "DrawRuntimeViewContent" and "#if defined(DEVELOPMENT)" in text:
                    continue
                errors.append(f"missing {fn}")
                continue
            body = chunk[1].split("void SceneBuildUp::", 1)[0]
            if "ApplySceneBuildUpRuntimeVisualFromProfile" in body:
                errors.append(f"{fn} must not apply runtime visual per draw")
            if "ApplyOnceOnEnterScene" in body:
                errors.append(f"{fn} must not call ApplyOnceOnEnterScene")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if not POLICY.is_file():
        errors.append("missing SceneBuildUpRuntimeLightingPolicy.cpp")

    if errors:
        print("verify_scene_build_up_runtime_lighting_enter_only: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_runtime_lighting_enter_only: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())