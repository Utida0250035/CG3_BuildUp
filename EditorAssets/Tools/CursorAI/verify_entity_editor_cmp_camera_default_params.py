#!/usr/bin/env python3
"""CmpCamera 追加時の params 既定値を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CAMERA = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorDefaultParamsCmpCamera.cpp"
)
NORM = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorComponentSlotParamsNormalize.cpp"
)
DEF = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorDefinition.cpp"
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not CAMERA.is_file():
        errors.append("missing EntityEditorDefaultParamsCmpCamera.cpp")
    else:
        text = CAMERA.read_text(encoding="utf-8", errors="replace")
        for needle in ("perspectiveFovY", "nearClip", "farClip"):
            if needle not in text:
                errors.append(f"CmpCamera defaults missing: {needle}")

    if not NORM.is_file():
        errors.append("missing EntityEditorComponentSlotParamsNormalize.cpp")
    else:
        if "NormalizeKnownComponentSlotParams" not in NORM.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("normalize must fill empty params")

    if DEF.is_file():
        def_text = DEF.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorDefaultParamsCmpCamera::Json" not in def_text:
            errors.append("DefaultParamsForType must use CmpCamera module")
        fj = def_text.split("void from_json(const nlohmann::json &j, ComponentSlot &slot)", 1)
        if len(fj) < 2 or "NormalizeKnownComponentSlotParams" not in fj[1].split("void from_json", 1)[0]:
            errors.append("ComponentSlot from_json must normalize params")
    else:
        errors.append("missing EntityEditorDefinition.cpp")

    if UI.is_file():
        if "NormalizeKnownComponentSlotParams" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("AppendKnownComponentSlot must normalize params")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_entity_editor_cmp_camera_default_params: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_cmp_camera_default_params: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())