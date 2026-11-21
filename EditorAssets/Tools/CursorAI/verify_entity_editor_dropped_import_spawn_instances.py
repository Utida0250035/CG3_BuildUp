#!/usr/bin/env python3
"""ドロップインポート後にシーンインスタンスを生成する経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SPAWN = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorDroppedImportSpawnSceneInstances.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not SPAWN.is_file():
        errors.append("missing EntityEditorDroppedImportSpawnSceneInstances.cpp")
    else:
        text = SPAWN.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "CmpEditorAssetPath",
            "CmpMesh",
            "CmpEditorTextureBillboard",
            "EntityEditorAuthoringDocumentEditNotify::Notify",
        ):
            if needle not in text:
                errors.append(f"spawn module missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "AppendFromImportedAssetPaths" not in ed:
            errors.append("ProcessDroppedFileImports must spawn scene instances")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_dropped_import_spawn_instances: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_dropped_import_spawn_instances: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())