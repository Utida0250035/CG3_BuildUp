#!/usr/bin/env python3
"""オブジェクト追加が空以外のテンプレートを選べるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PRESET = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorSceneObjectAddPreset.cpp"
)
UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorHierarchyAddObjectUi.cpp"
)
MAIN_UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not PRESET.is_file():
        errors.append("missing EntityEditorSceneObjectAddPreset.cpp")
    else:
        text = PRESET.read_text(encoding="utf-8", errors="replace")
        for sym in (
            "SceneObjectAddPreset::Empty",
            "SceneObjectAddPreset::MainCamera",
            "MakeDefaultEntityDefinition",
            "MainCamera.json",
        ):
            if sym not in text:
                errors.append(f"preset module missing: {sym}")

    if not UI.is_file():
        errors.append("missing EntityEditorHierarchyAddObjectUi.cpp")
    else:
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "追加テンプレート" not in ui:
            errors.append("hierarchy UI must offer preset combo")
        if "AppendInstance" not in ui:
            errors.append("hierarchy UI must call AppendInstance with preset")

    if MAIN_UI.is_file():
        main_ui = MAIN_UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorHierarchyAddObjectUi" not in main_ui:
            errors.append("EntityEditorUi must use hierarchy add-object UI")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_add_scene_object_presets: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_add_scene_object_presets: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())