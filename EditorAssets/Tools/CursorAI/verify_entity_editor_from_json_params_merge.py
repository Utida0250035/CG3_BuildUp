#!/usr/bin/env python3
"""ComponentSlot from_json が params を既定値とマージするか検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DEF = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorDefinition.cpp"
MERGE = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorComponentSlotParamsNormalize.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not MERGE.is_file():
        errors.append("missing EntityEditorComponentSlotParamsNormalize.cpp")
    else:
        text = MERGE.read_text(encoding="utf-8", errors="replace")
        if "MergeParamsForRegisteredType" not in text:
            errors.append("MergeParamsForRegisteredType must exist")
        if "DefaultParamsForType" not in text or "merged[key]" not in text:
            errors.append("merge must overlay incoming onto defaults")

    if DEF.is_file():
        block = DEF.read_text(encoding="utf-8", errors="replace")
        slot_fn = block.split("void from_json(const nlohmann::json &j, ComponentSlot &slot)", 1)
        if len(slot_fn) < 2:
            errors.append("missing ComponentSlot from_json")
        else:
            body = slot_fn[1].split("namespace {", 1)[0]
            if re.search(r"slot\.params\s*=\s*j\.value\s*\(\s*[\"']params[\"']", body):
                errors.append("from_json must not wholesale-assign params via j.value")
            if "MergeParamsForRegisteredType" not in body:
                errors.append("from_json must call MergeParamsForRegisteredType")
    else:
        errors.append("missing EntityEditorDefinition.cpp")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        combo = ui.split('ImGui::Combo("コンポーネント型"', 1)
        if len(combo) >= 2:
            tail = combo[1][:800]
            if "MergeParamsForRegisteredType" in tail:
                errors.append(
                    "component type combo must replace with DefaultParams, not merge"
                )
            if "ApplyKnownComponentTypeFromCombo" not in ui:
                errors.append(
                    "component type combo must call ApplyKnownComponentTypeFromCombo"
                )
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_entity_editor_from_json_params_merge: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_from_json_params_merge: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())