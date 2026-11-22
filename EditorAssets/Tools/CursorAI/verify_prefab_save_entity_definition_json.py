#!/usr/bin/env python3
"""プレハブ保存が EntityDefinition 単体 JSON になるか検査。"""

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
BODY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorPrefabSaveEntityDefinitionBody.cpp"
)
LOAD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorPrefabSpawnFirstInstanceRoot.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not BODY.is_file():
        errors.append("missing EntityEditorPrefabSaveEntityDefinitionBody.cpp")
    else:
        if "BuildEntityDefinitionForPrefabFile" not in BODY.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("prefab body builder required")

    if SAVE.is_file():
        text = SAVE.read_text(encoding="utf-8", errors="replace")
        if "BuildEntityDefinitionForPrefabFile" not in text:
            errors.append("save must build definition body")
        if "SaveToFile(prefabPath, prefabInstance)" in text:
            errors.append("must not save SceneEntityInstance wrapper")
        if "SaveToFile(prefabPath, definition)" not in text:
            errors.append("must SaveToFile EntityDefinition")
    else:
        errors.append("missing EntityEditorSaveSelectedInstanceToPrefab.cpp")

    if LOAD.is_file():
        if "json.get<EntityDefinition>()" not in LOAD.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("spawn must load definition-only prefabs")
    else:
        errors.append("missing prefab spawn root")

    if errors:
        print("verify_prefab_save_entity_definition_json: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_prefab_save_entity_definition_json: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())