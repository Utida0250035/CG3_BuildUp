#!/usr/bin/env python3
"""重複時の stem_N 自動リネーム経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
REPORT = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetAutoStemRenameReport.cpp"
)
POLICY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetCollisionPolicy.cpp"
)
ENTITY = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"
DROPPED = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not REPORT.is_file():
        errors.append("missing EditorImportedAssetAutoStemRenameReport.cpp")
    else:
        text = REPORT.read_text(encoding="utf-8", errors="replace")
        if "UsedAutoStemIncrementRename" not in text:
            errors.append("auto stem rename detector required")

    if POLICY.is_file():
        pol = POLICY.read_text(encoding="utf-8", errors="replace")
        if "MakeIncrementedStemName" not in pol or "AutoIncrementStem" not in pol:
            errors.append("default policy must use stem increment")
    else:
        errors.append("missing collision policy")

    if ENTITY.is_file():
        if "AutoIncrementStem" not in ENTITY.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("UiState default must be AutoIncrementStem")
    else:
        errors.append("missing EntityEditor.h")

    if DROPPED.is_file():
        if "UsedAutoStemIncrementRename" not in DROPPED.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("drop import must report auto stem rename")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_editor_imported_asset_auto_stem_rename: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_imported_asset_auto_stem_rename: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())