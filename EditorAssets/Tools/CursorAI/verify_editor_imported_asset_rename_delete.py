#!/usr/bin/env python3
"""Imported ファイルの削除・改名 API と UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HEADER = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorImportedAsset.h"
OPS = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetFileOperations.cpp"
)
PANEL = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetProjectPanelList.cpp"
)
SCRUB = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetUiPathScrub.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not HEADER.is_file():
        errors.append("missing EditorImportedAsset.h")
    else:
        text = HEADER.read_text(encoding="utf-8", errors="replace")
        if "TryDeleteImportedAssetFile" not in text:
            errors.append("header must declare delete API")
        if "TryRenameImportedAssetFile" not in text:
            errors.append("header must declare rename API")

    if OPS.is_file():
        ops = OPS.read_text(encoding="utf-8", errors="replace")
        if "fs::remove" not in ops or "fs::rename" not in ops:
            errors.append("file ops must remove and rename")
        if "kImportedAssetsDirectory" not in ops:
            errors.append("ops must scope to Imported root")
    else:
        errors.append("missing EditorImportedAssetFileOperations.cpp")

    if PANEL.is_file():
        panel = PANEL.read_text(encoding="utf-8", errors="replace")
        if 'ImGui::Button("削除")' not in panel:
            errors.append("panel must offer delete")
        if 'ImGui::Button("名前変更")' not in panel:
            errors.append("panel must offer rename")
    else:
        errors.append("missing project panel list")

    if not SCRUB.is_file():
        errors.append("missing EditorImportedAssetUiPathScrub.cpp")

    if errors:
        print("verify_editor_imported_asset_rename_delete: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_imported_asset_rename_delete: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())