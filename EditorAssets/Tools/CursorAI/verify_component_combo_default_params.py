#!/usr/bin/env python3
"""コンポーネント型コンボ変更で params を DefaultParams に差し替えるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
APPLY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorInspectorComponentTypeComboApply.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not APPLY.is_file():
        errors.append("missing EntityEditorInspectorComponentTypeComboApply.cpp")
    else:
        text = APPLY.read_text(encoding="utf-8", errors="replace")
        if "DefaultParamsForType" not in text:
            errors.append("combo apply must use DefaultParamsForType")
        if "MergeParamsForRegisteredType" in text:
            errors.append("combo apply must not merge old params")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "ApplyKnownComponentTypeFromCombo" not in ui:
            errors.append("EntityEditorUi must call combo apply helper")
        combo = ui.split('ImGui::Combo("コンポーネント型"', 1)
        if len(combo) >= 2 and "MergeParamsForRegisteredType" in combo[1][:600]:
            errors.append("combo handler must not call MergeParamsForRegisteredType")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_component_combo_default_params: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_component_combo_default_params: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())