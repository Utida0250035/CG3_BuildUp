#!/usr/bin/env python3
"""CmpMesh / CmpEditorTextureBillboard がエディタ追加経路に載っているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
DEFINITION = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorDefinition.cpp"
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []
    for label, path in (
        ("EditorSceneRuntime", RUNTIME),
        ("EntityEditorDefinition", DEFINITION),
        ("EntityEditorUi", UI),
    ):
        if not path.is_file():
            errors.append(f"missing {path.name}")
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        for cmp in ("CmpMesh", "CmpEditorTextureBillboard"):
            if cmp not in text:
                errors.append(f"{label}: expected {cmp}")

    if RUNTIME.is_file():
        rt = RUNTIME.read_text(encoding="utf-8", errors="replace")
        if 'slot.typeName == "CmpMesh"' not in rt or "AddDrawCmp<CmpMesh>" not in rt:
            errors.append("TryAddEmptyComponent must add CmpMesh draw slot")
        if (
            'slot.typeName == "CmpEditorTextureBillboard"' not in rt
            or "AddDrawCmp<CmpEditorTextureBillboard>" not in rt
        ):
            errors.append("TryAddEmptyComponent must add CmpEditorTextureBillboard")

    if errors:
        print("verify_editor_scene_runtime_cmp_mesh_billboard_try_add: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_cmp_mesh_billboard_try_add: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())