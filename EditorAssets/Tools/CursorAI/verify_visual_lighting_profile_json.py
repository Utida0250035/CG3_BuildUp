#!/usr/bin/env python3
"""EditorAssets / Assets の game_scene_lighting.json 正本を検査。"""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EDITOR_JSON = ROOT / "EditorAssets" / "Visual" / "game_scene_lighting.json"
ASSETS_JSON = ROOT / "Assets" / "Visual" / "game_scene_lighting.json"
PATHS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorAssetsPaths.h"
PROFILE_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneGameLightingProfile.cpp"
FOG_MD = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "docs" / "feature-memos" / "volumetric_fog.md"

REQUIRED_ROOT_KEYS = (
    "version",
    "directionalLight",
    "volumetricFog",
    "godRays",
    "directionalShadow",
)


def load_object(path: Path) -> dict | None:
    if not path.is_file():
        return None
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    return data if isinstance(data, dict) else None


def main() -> int:
    errors: list[str] = []

    editor = load_object(EDITOR_JSON)
    assets = load_object(ASSETS_JSON)
    if editor is None:
        errors.append("EditorAssets/Visual/game_scene_lighting.json missing or invalid")
    if assets is None:
        errors.append("Assets/Visual/game_scene_lighting.json missing or invalid")

    for label, doc in (("EditorAssets", editor), ("Assets", assets)):
        if doc is None:
            continue
        if doc.get("version") != 1:
            errors.append(f"{label}: version must be 1")
        for key in REQUIRED_ROOT_KEYS:
            if key not in doc:
                errors.append(f"{label}: missing key {key}")

    if editor is not None and assets is not None and editor != assets:
        errors.append("EditorAssets と Assets の game_scene_lighting.json が一致しない")

    if PATHS_H.is_file():
        paths_text = PATHS_H.read_text(encoding="utf-8", errors="replace")
        if "Visual/game_scene_lighting.json" not in paths_text:
            errors.append("EditorAssetsPaths.h: kGameSceneLightingProfileJson path expected")
    else:
        errors.append("missing EditorAssetsPaths.h")

    if PROFILE_CPP.is_file():
        cpp = PROFILE_CPP.read_text(encoding="utf-8", errors="replace")
        if "kGameSceneLightingProfileJson" not in cpp:
            errors.append("SceneGameLightingProfile.cpp must resolve profile JSON paths")
    else:
        errors.append("missing SceneGameLightingProfile.cpp")

    if FOG_MD.is_file():
        md = FOG_MD.read_text(encoding="utf-8", errors="replace")
        if "game_scene_lighting.json" not in md:
            errors.append("volumetric_fog.md must document game_scene_lighting.json")
    else:
        errors.append("missing volumetric_fog.md")

    if errors:
        print("verify_visual_lighting_profile_json: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_visual_lighting_profile_json: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())