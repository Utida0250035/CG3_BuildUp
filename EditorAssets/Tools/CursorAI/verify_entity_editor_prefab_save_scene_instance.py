#!/usr/bin/env python3
"""プレハブ保存が SceneEntityInstance 形式である経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SAVE = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorSaveSelectedInstanceToPrefab.cpp"
)
SPAWN = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorPrefabSpawnFirstInstanceRoot.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not SAVE.is_file():
        errors.append("missing EntityEditorSaveSelectedInstanceToPrefab.cpp")
    else:
        text = SAVE.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityInstance" not in text:
            errors.append("prefab save must persist SceneEntityInstance")
        if "SaveToFile(prefabPath, prefabInstance)" not in text:
            errors.append("prefab save must write instance object")

    if SPAWN.is_file():
        sp = SPAWN.read_text(encoding="utf-8", errors="replace")
        if 'json.contains("definition")' not in sp:
            errors.append("prefab spawn must load instance wrapper JSON")
    else:
        errors.append("missing EntityEditorPrefabSpawnFirstInstanceRoot.cpp")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorSaveSelectedInstanceToPrefab::Save" not in ed:
            errors.append("SaveSelectedInstanceToPrefabPath must delegate")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_prefab_save_scene_instance: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_prefab_save_scene_instance: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())