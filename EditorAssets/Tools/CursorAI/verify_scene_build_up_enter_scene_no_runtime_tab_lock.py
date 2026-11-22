#!/usr/bin/env python3
"""BuildUp::EnterScene が EditorFrame の Runtime タブを固定しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
POLICY = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpEnterSceneEditorUiPolicy.cpp"


def main() -> int:
    errors: list[str] = []

    if BUILDUP.is_file():
        enter = BUILDUP.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::EnterScene()", 1
        )
        if len(enter) < 2:
            errors.append("missing EnterScene")
        else:
            body = enter[1].split("void SceneBuildUp::", 1)[0]
            if "SetSceneViewTab" in body:
                errors.append("EnterScene must not call SetSceneViewTab")
            if "SceneViewTab::Runtime" in body:
                errors.append("EnterScene must not reference SceneViewTab::Runtime")
            if "ApplyOnEnterScene" not in body:
                errors.append("EnterScene must call ApplyOnEnterScene")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if POLICY.is_file():
        text = POLICY.read_text(encoding="utf-8", errors="replace")
        if "SetSceneViewTab" in text:
            errors.append("enter UI policy must not SetSceneViewTab")
        if "ClearStaleRuntimeSceneViewFocusRequest" not in text:
            errors.append("enter UI policy must clear stale runtime focus only")
    else:
        errors.append("missing SceneBuildUpEnterSceneEditorUiPolicy.cpp")

    if errors:
        print("verify_scene_build_up_enter_scene_no_runtime_tab_lock: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_enter_scene_no_runtime_tab_lock: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())