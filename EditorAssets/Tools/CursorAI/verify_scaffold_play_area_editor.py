#!/usr/bin/env python3
"""エディタから DebugPlayArea scaffold が呼べることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    scaffold_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorDebugPlayAreaScaffold.cpp"
    ui_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
    asset_cpp = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneAsset.cpp"

    if not scaffold_cpp.is_file():
        errors.append("missing EditorDebugPlayAreaScaffold.cpp")
    else:
        text = scaffold_cpp.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaTemplateScene" not in text:
            errors.append("scaffold must use kDebugPlayAreaTemplateScene")
        if "PersonalizeDebugPlayAreaSceneJson" not in text:
            errors.append("scaffold must personalize sceneName")

    if ui_cpp.is_file():
        if "EditorDebugPlayAreaScaffold" not in ui_cpp.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("EntityEditorUi must wire EditorDebugPlayAreaScaffold")

    roots_h = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorEditableSceneJsonRoots.h"
    if roots_h.is_file():
        if "kDebugPlayAreaDirectory" not in roots_h.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("editable scene roots must include DebugPlayArea")
    else:
        errors.append("missing EditorEditableSceneJsonRoots.h")

    if errors:
        print("verify_scaffold_play_area_editor: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scaffold_play_area_editor: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())