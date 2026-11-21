#!/usr/bin/env python3
"""EntityHasComponentForSlot が分割ガイドラインの Mesh／Billboard を認識するか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SLOT_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntimeEntityHasComponentForSlot.cpp"
)
RUNTIME = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"


def main() -> int:
    errors: list[str] = []

    if not SLOT_CPP.is_file():
        errors.append("missing EditorSceneRuntimeEntityHasComponentForSlot.cpp")
    else:
        text = SLOT_CPP.read_text(encoding="utf-8", errors="replace")
        for needle in (
            'slot.typeName == "CmpMesh"',
            "HasDrawCmp<CmpMesh>",
            'slot.typeName == "CmpEditorTextureBillboard"',
            "HasDrawCmp<CmpEditorTextureBillboard>",
        ):
            if needle not in text:
                errors.append(f"slot probe missing: {needle}")

    if RUNTIME.is_file():
        rt = RUNTIME.read_text(encoding="utf-8", errors="replace")
        if "EditorSceneRuntimeEntityHasComponentForSlot.h" not in rt:
            errors.append("EditorSceneRuntime must include EntityHasComponentForSlot module")
        if (
            "bool EntityHasComponentForSlot(const Entity &entity, const ComponentSlot &slot)"
            in rt
        ):
            errors.append("duplicate EntityHasComponentForSlot definition in EditorSceneRuntime.cpp")
        if "EntityHasComponentForSlot(*entity, slot)" not in rt:
            errors.append("BuildEntityFromDefinition must call EntityHasComponentForSlot")
    else:
        errors.append("missing EditorSceneRuntime.cpp")

    if errors:
        print(
            "verify_editor_scene_runtime_entity_has_component_for_slot: FAILED",
            file=sys.stderr,
        )
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_entity_has_component_for_slot: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())