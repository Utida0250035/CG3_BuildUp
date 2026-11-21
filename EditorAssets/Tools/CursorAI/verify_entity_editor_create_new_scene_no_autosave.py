#!/usr/bin/env python3
"""新規シーン作成が requestSaveToDisk を立てない（メモリ草案）経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MOD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorCreateNewSceneFile.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorCreateNewSceneFile.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        if "NotifyPreviewOnly" not in text:
            errors.append("new scene must use NotifyPreviewOnly")
        if "Notify(editor)" in text.replace("NotifyPreviewOnly", ""):
            errors.append("new scene must not call full Notify (auto-save)")
        if "requestSaveToDisk = true" in text:
            errors.append("new scene must not set requestSaveToDisk")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorCreateNewSceneFile::Create" not in ed:
            errors.append("CreateNewSceneFile must delegate to create-new module")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_create_new_scene_no_autosave: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_create_new_scene_no_autosave: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())