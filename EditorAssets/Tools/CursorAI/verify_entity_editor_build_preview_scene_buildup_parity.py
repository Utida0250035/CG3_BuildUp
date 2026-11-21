#!/usr/bin/env python3
"""BuildPreviewScene が BuildUp 同一経路と PlayRequest 同期を使うか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PARITY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorBuildPreviewSceneBuildUpParity.cpp"
)
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not PARITY.is_file():
        errors.append("missing EntityEditorBuildPreviewSceneBuildUpParity.cpp")
    else:
        text = PARITY.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "SceneBuildUpSceneDocumentTagCatalog::ApplyAfterJsonLoad",
            "SceneBuildUpSceneDocumentLoad::BuildEntities",
            "StageEditorPlayScenePath",
            "InEditorRuntimePreview",
            "sceneBuildUpLaunchPath",
        ):
            if needle not in text:
                errors.append(f"parity build missing: {needle}")

    if EDITOR.is_file():
        ed = EDITOR.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorBuildPreviewSceneBuildUpParity::Build" not in ed:
            errors.append("BuildPreviewScene must delegate to BuildUp parity module")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_entity_editor_build_preview_scene_buildup_parity: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_build_preview_scene_buildup_parity: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())