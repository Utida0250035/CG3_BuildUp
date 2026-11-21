#!/usr/bin/env python3
"""インスペクタでシーン JSON パスを編集・検証できるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PANEL = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorInspectorSceneJsonPathUi.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not PANEL.is_file():
        errors.append("missing EntityEditorInspectorSceneJsonPathUi.cpp")
    else:
        text = PANEL.read_text(encoding="utf-8", errors="replace")
        if 'InputString("シーン JSON パス"' not in text:
            errors.append("inspector must edit jsonFilePath with InputString")
        if "selectedScenePath" not in text:
            errors.append("must sync selectedScenePath on edit")
        if "このパスで開き直す" not in text:
            errors.append("must offer reopen by path")
        if "IsEditableSceneJsonPath" not in text:
            errors.append("must validate editable scene path")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorInspectorSceneJsonPathUi" not in ui:
            errors.append("DrawInspectorPanel must use scene json path UI")
        draw_fn = ui.split("void EntityEditor::DrawInspectorPanel()")[1].split(
            "void EntityEditor::", 1
        )[0]
        empty_pos = draw_fn.find("instances.empty()")
        panel_pos = draw_fn.find("EntityEditorInspectorSceneJsonPathUi")
        if panel_pos < 0 or empty_pos < 0 or panel_pos > empty_pos:
            errors.append("scene path editor must appear before empty-scene early return")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_inspector_scene_json_path_edit: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_inspector_scene_json_path_edit: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())