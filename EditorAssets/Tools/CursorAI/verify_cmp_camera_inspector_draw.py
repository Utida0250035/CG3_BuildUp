#!/usr/bin/env python3
"""CmpCamera 専用インスペクタが未対応表示を経由しないことを検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CMP_UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorUiDrawComponentParamsCmpCamera.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not CMP_UI.is_file():
        errors.append("missing EntityEditorUiDrawComponentParamsCmpCamera.cpp")
    else:
        text = CMP_UI.read_text(encoding="utf-8", errors="replace")
        if 'slot.typeName == "CmpCamera"' not in text:
            errors.append("CmpCamera drawer must branch on typeName")
        for key in ("perspectiveFovY", "nearClip", "farClip"):
            if key not in text:
                errors.append(f"CmpCamera UI must edit {key}")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "TryDrawCmpCameraComponentParams" not in ui:
            errors.append("DrawComponentParams must delegate to CmpCamera drawer")
        if re.search(
            r'if \(slot\.typeName == "CmpCamera"\)', ui
        ):
            errors.append("inline CmpCamera block should live in dedicated drawer")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_cmp_camera_inspector_draw: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_cmp_camera_inspector_draw: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())