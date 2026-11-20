#!/usr/bin/env python3
"""SceneBuildUp::ExitScene がシーンビューを Edit に戻すことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    buildup = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
    restore_cpp = (
        ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpExitSceneEditorUiRestore.cpp"
    )
    teardown = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Scene"
        / "SceneEntityEditorExitSceneTeardown.cpp"
    )

    if not buildup.is_file():
        errors.append("missing SceneBuildUp.cpp")
    else:
        exit_block = buildup.read_text(encoding="utf-8", errors="replace").split(
            "SceneBuildUp::ExitScene()", 1
        )
        if len(exit_block) < 2:
            errors.append("missing ExitScene")
        elif "ApplyOnExitScene" not in exit_block[1].split("void ", 2)[0]:
            errors.append("ExitScene must call ApplyOnExitScene")

    if not restore_cpp.is_file():
        errors.append("missing SceneBuildUpExitSceneEditorUiRestore.cpp")
    elif "RestoreUiAfterSceneBuildUpExit" not in restore_cpp.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("restore module must call RestoreUiAfterSceneBuildUpExit")

    if teardown.is_file():
        text = teardown.read_text(encoding="utf-8", errors="replace")
        if "RestoreSceneViewTabAfterBuildUpExit" not in text:
            errors.append("teardown must define RestoreSceneViewTabAfterBuildUpExit")
        if "SetSceneViewTab(Editor::SceneViewTab::Edit)" not in text:
            errors.append("teardown must set SceneViewTab::Edit")
    else:
        errors.append("missing SceneEntityEditorExitSceneTeardown.cpp")

    if errors:
        print("verify_scene_build_up_exit_scene_view_tab: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_exit_scene_view_tab: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())