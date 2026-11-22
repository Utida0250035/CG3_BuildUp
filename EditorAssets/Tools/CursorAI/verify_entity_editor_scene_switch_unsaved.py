#!/usr/bin/env python3
"""EntityEditor のシーン切替未保存確認を静的に検査する。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    prompt_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "Entity"
        / "EntityEditorSceneSwitchUnsavedPrompt.cpp"
    )
    editor_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "Entity"
        / "EntityEditor.cpp"
    )
    ui_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "Entity"
        / "EntityEditorUi.cpp"
    )
    vcxproj = ROOT / "Project" / "CG2_BuildUp.vcxproj"

    if not prompt_cpp.is_file():
        errors.append("missing EntityEditorSceneSwitchUnsavedPrompt.cpp")

    if editor_cpp.is_file():
        text = editor_cpp.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "TryOpenSceneFromPath",
            "sceneDocumentDirty_",
            "ClearSceneDocumentDirty",
        ):
            if needle not in text:
                errors.append(f"EntityEditor.cpp: expected {needle}")

    if ui_cpp.is_file():
        text = ui_cpp.read_text(encoding="utf-8", errors="replace")
        if "DrawEntityEditorSceneSwitchUnsavedPrompt" not in text:
            errors.append("EntityEditorUi: expected unsaved scene switch prompt")
        if "TryOpenSceneFromPath(ui_.selectedScenePath)" not in text:
            errors.append("EntityEditorUi: expected TryOpenSceneFromPath on open")

    if vcxproj.is_file():
        vtext = vcxproj.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorSceneSwitchUnsavedPrompt.cpp" not in vtext:
            errors.append("vcxproj: EntityEditorSceneSwitchUnsavedPrompt.cpp not listed")

    if errors:
        print("verify_entity_editor_scene_switch_unsaved: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_scene_switch_unsaved: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())