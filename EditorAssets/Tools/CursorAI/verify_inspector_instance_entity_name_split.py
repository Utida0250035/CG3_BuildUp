#!/usr/bin/env python3
"""instanceName と entityName がインスペクタで独立編集できるか検査。"""

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
    / "EntityEditorInspectorInstanceIdentityUi.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
RUNTIME = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorSceneRuntimeInstanceNamePolicy.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not PANEL.is_file():
        errors.append("missing EntityEditorInspectorInstanceIdentityUi.cpp")
    else:
        text = PANEL.read_text(encoding="utf-8", errors="replace")
        if "instanceName" not in text or "entityName" not in text:
            errors.append("identity panel must edit both names")
        if 'InputString("ヒエラルキー表示名' not in text:
            errors.append("must label instanceName field")
        if 'InputString("エンティティ名 (entityName)"' not in text:
            errors.append("must label entityName field")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorInspectorInstanceIdentityUi" not in ui:
            errors.append("inspector must use identity UI module")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if RUNTIME.is_file():
        if "ApplySceneInstanceName" not in RUNTIME.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("runtime must apply instanceName separately")
    else:
        errors.append("missing EditorSceneRuntimeInstanceNamePolicy.cpp")

    if errors:
        print("verify_inspector_instance_entity_name_split: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_inspector_instance_entity_name_split: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())