#!/usr/bin/env python3
"""scaffold が sceneName 以外も差し替えることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    py_mod = (
        ROOT
        / "EditorAssets"
        / "Tools"
        / "DebugPlayAreaGenerator"
        / "personalize_play_area_scene.py"
    )
    scaffold_py = (
        ROOT / "EditorAssets" / "Tools" / "DebugPlayAreaGenerator" / "scaffold_play_area.py"
    )
    personalize_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "DebugPlayAreaScaffoldPersonalize.cpp"
    )
    editor_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "EditorDebugPlayAreaScaffold.cpp"
    )

    if not py_mod.is_file():
        errors.append("missing personalize_play_area_scene.py")
    else:
        text = py_mod.read_text(encoding="utf-8", errors="replace")
        if "instanceName" not in text or "tagCatalog" not in text:
            errors.append("python personalize must touch instanceName and tagCatalog")

    if scaffold_py.is_file():
        text = scaffold_py.read_text(encoding="utf-8", errors="replace")
        if "personalize_play_area_scene" not in text:
            errors.append("scaffold_play_area.py must call personalize_play_area_scene")
        if 'data["sceneName"] = args.name' in text:
            errors.append("scaffold must not only assign sceneName inline")

    if personalize_cpp.is_file():
        text = personalize_cpp.read_text(encoding="utf-8", errors="replace")
        if "DebugProp" not in text or "DebugFloorMarker" not in text:
            errors.append("C++ personalize must rename template instances")

    if editor_cpp.is_file():
        if "PersonalizeDebugPlayAreaSceneJson" not in editor_cpp.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("EditorDebugPlayAreaScaffold must call Personalize")

    if errors:
        print("verify_scaffold_play_area_personalize: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scaffold_play_area_personalize: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())