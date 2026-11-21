#!/usr/bin/env python3
"""Imported パスの選択への一括適用 UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BATCH = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetBatchApplyToSelection.cpp"
)
APPLY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetApplyToEntityDefinition.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not APPLY.is_file():
        errors.append("missing EditorImportedAssetApplyToEntityDefinition.cpp")
    else:
        text = APPLY.read_text(encoding="utf-8", errors="replace")
        if "CmpEditorAssetPath" not in text:
            errors.append("apply must upsert CmpEditorAssetPath")

    if not BATCH.is_file():
        errors.append("missing EditorImportedAssetBatchApplyToSelection.cpp")
    else:
        text = BATCH.read_text(encoding="utf-8", errors="replace")
        if "ApplySinglePathToSelectedInstances" not in text:
            errors.append("batch apply API required")
        if "DrawProjectPanelBatchApplyUi" not in text:
            errors.append("project panel UI required")
        if "直近パスを選択へ一括適用" not in text:
            errors.append("must offer last-path batch apply button")

    if UI.is_file():
        if "DrawProjectPanelBatchApplyUi" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("EntityEditorUi project panel must call batch apply UI")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_editor_imported_asset_batch_apply_ui: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_imported_asset_batch_apply_ui: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())