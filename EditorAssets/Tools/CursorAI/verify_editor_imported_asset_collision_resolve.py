#!/usr/bin/env python3
"""Imported 同名衝突が stem_N 以外の方針も選べるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RESOLVE = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorImportedAssetCollisionPolicy.cpp"
)
COPY = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorImportedAsset.cpp"
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
EDITOR = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not RESOLVE.is_file():
        errors.append("missing EditorImportedAssetCollisionPolicy.cpp")
    else:
        text = RESOLVE.read_text(encoding="utf-8", errors="replace")
        if "OverwriteExisting" not in text or "TimestampSuffix" not in text:
            errors.append("collision policy must include overwrite and timestamp")
        if "BatchPrefixThenIncrement" not in text:
            errors.append("collision policy must include batch prefix mode")

    if COPY.is_file():
        if "ResolveImportedAssetDestinationFileName" not in COPY.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("CopyFileIntoImportedAssets must use resolver")
    else:
        errors.append("missing EditorImportedAsset.cpp")

    if UI.is_file():
        if "DrawDropCollisionPolicyControls" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("project panel must expose collision policy UI")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if EDITOR.is_file():
        if "SetImportedAssetCollisionPolicyForNextCopies" not in EDITOR.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("drop handler must sync collision policy from UI")
    else:
        errors.append("missing EntityEditor.cpp")

    if errors:
        print("verify_editor_imported_asset_collision_resolve: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_imported_asset_collision_resolve: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())