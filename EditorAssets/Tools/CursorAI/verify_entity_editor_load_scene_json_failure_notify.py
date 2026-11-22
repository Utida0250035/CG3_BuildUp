#!/usr/bin/env python3
"""シーン JSON 読込失敗時に importStatusMessage へ通知する経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
LOAD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorSceneDocumentLoadFromPath.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not LOAD.is_file():
        errors.append("missing EntityEditorSceneDocumentLoadFromPath.cpp")
    else:
        text = LOAD.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "シーン JSON が見つかりません",
            "シーン JSON を開けません",
            "TryLoadSceneDocument",
            "ApplyEmptySceneForMissingJsonPath",
        ):
            if needle not in text:
                errors.append(f"load module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorSceneDocumentLoadFromPath::TryLoad" not in ed:
            errors.append("LoadFromJsonPath must use TryLoad helper")
        if "importStatusMessage" not in ed.split("LoadFromJsonPath", 1)[-1][:800]:
            errors.append("LoadFromJsonPath must set importStatusMessage on failure")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_load_scene_json_failure_notify: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_load_scene_json_failure_notify: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())