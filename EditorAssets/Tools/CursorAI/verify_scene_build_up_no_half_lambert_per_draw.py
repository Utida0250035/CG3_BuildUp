#!/usr/bin/env python3
"""BuildUp が描画ごとに HalfLambert 等のライトを上書きしないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
FUNC_DRAW = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorSceneRuntimeFunctionalViewDraw.cpp"
)

FORBIDDEN_IN_DRAW = (
    "EngineSetDirectionalLightData",
    "LightModel::HalfLambert",
    "ApplySceneBuildUpRuntimeVisualFromProfile",
)


def draw_body(cpp_text: str, fn_marker: str) -> str | None:
    chunk = cpp_text.split(fn_marker, 1)
    if len(chunk) < 2:
        return None
    return chunk[1].split("void ", 1)[0]


def main() -> int:
    errors: list[str] = []

    if BUILDUP.is_file():
        text = BUILDUP.read_text(encoding="utf-8", errors="replace")
        if any(token in text for token in FORBIDDEN_IN_DRAW):
            errors.append("SceneBuildUp.cpp must not hard-set directional light")
        for fn in ("DrawScene", "DrawRuntimeViewContent"):
            body = draw_body(text, f"SceneBuildUp::{fn}")
            if body is None:
                continue
            for token in FORBIDDEN_IN_DRAW:
                if token in body:
                    errors.append(f"{fn} must not call {token}")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if FUNC_DRAW.is_file():
        text = FUNC_DRAW.read_text(encoding="utf-8", errors="replace")
        if "ApplySceneBuildUpRuntimeVisualFromProfile" in text:
            errors.append(
                "functional runtime view must not apply lighting profile each draw"
            )
    else:
        errors.append("missing EditorSceneRuntimeFunctionalViewDraw.cpp")

    policy = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneBuildUpRuntimeLightingPolicy.h"
    if policy.is_file():
        if "HalfLambert" not in policy.read_text(encoding="utf-8", errors="replace"):
            errors.append("lighting policy must document HalfLambert non-override")
    else:
        errors.append("missing SceneBuildUpRuntimeLightingPolicy.h")

    if errors:
        print("verify_scene_build_up_no_half_lambert_per_draw: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_no_half_lambert_per_draw: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())