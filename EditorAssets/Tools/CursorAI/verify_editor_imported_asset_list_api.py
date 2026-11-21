#!/usr/bin/env python3
"""Imported 配下ファイル一覧 API の有無を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HEADER = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorImportedAsset.h"
LIST = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorImportedAssetList.cpp"
PANEL = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetProjectPanelList.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not HEADER.is_file():
        errors.append("missing EditorImportedAsset.h")
    elif "ListImportedAssetRelativePaths" not in HEADER.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("header must declare ListImportedAssetRelativePaths")

    if not LIST.is_file():
        errors.append("missing EditorImportedAssetList.cpp")
    else:
        text = LIST.read_text(encoding="utf-8", errors="replace")
        if "directory_iterator" not in text or "kImportedAssetsDirectory" not in text:
            errors.append("list must scan kImportedAssetsDirectory")

    if not PANEL.is_file():
        errors.append("missing EditorImportedAssetProjectPanelList.cpp")
    else:
        if "ListImportedAssetRelativePaths" not in PANEL.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("project panel must call list API")

    if UI.is_file():
        if "DrawImportedAssetsFolderList" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("project panel must draw imported folder list")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_editor_imported_asset_list_api: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_imported_asset_list_api: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())