#!/usr/bin/env python3
"""登録外 typeName 向け DrawComponentParams JSON フォールバックを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
GENERIC = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorUiDrawComponentParamsGenericJson.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not GENERIC.is_file():
        errors.append("missing generic component params drawer")
    else:
        text = GENERIC.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorKnownComponentRegistry::Contains" not in text:
            errors.append("generic drawer must gate on registry Contains")
        if "component_split_guidelines" not in text:
            errors.append("must reference component_split_guidelines")
        if "DrawJsonObjectFields" not in text:
            errors.append("must edit params as JSON tree")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "TryDrawUnknownComponentParams" not in ui:
            errors.append("DrawComponentParams must call unknown JSON drawer")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_draw_component_params_unknown_json: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_draw_component_params_unknown_json: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())