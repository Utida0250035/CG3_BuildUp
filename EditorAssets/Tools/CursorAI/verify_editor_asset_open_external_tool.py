#!/usr/bin/env python3
"""png/obj/gltf 等を外部ツールで開く操作があるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
OPEN_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorExternalAssetOpen.cpp"
OPEN_UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorUiDrawComponentParamsExternalOpen.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not OPEN_CPP.is_file():
        errors.append("missing EditorExternalAssetOpen.cpp")
    else:
        text = OPEN_CPP.read_text(encoding="utf-8", errors="replace")
        if "TryOpenExternalAssetWithDefaultApp" not in text:
            errors.append("must define TryOpenExternalAssetWithDefaultApp")
        if ".gltf" not in text or ".png" not in text or ".obj" not in text:
            errors.append("must list png/obj/gltf target extensions")
        if "ShellExecute" not in text and "xdg-open" not in text:
            errors.append("must launch OS default handler (ShellExecute or xdg-open)")

    if not OPEN_UI.is_file():
        errors.append("missing EntityEditorUiDrawComponentParamsExternalOpen.cpp")
    else:
        ui_text = OPEN_UI.read_text(encoding="utf-8", errors="replace")
        if "外部ツールで開く" not in ui_text:
            errors.append("inspector must show external open button")

    if UI.is_file():
        main_ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorUiDrawComponentParamsExternalOpen" not in main_ui:
            errors.append("EntityEditorUi must wire external open helper")
        if main_ui.count("EntityEditorUiDrawComponentParamsExternalOpen::DrawButton") < 3:
            errors.append("DrawButton must be used for multiple asset path fields")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_editor_asset_open_external_tool: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_asset_open_external_tool: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())