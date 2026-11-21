#!/usr/bin/env python3
"""コンポーネント設定のコピー／ペースト UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
COPY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorComponentSettingsCopyPaste.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
HEADER = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"


def main() -> int:
    errors: list[str] = []

    if not COPY.is_file():
        errors.append("missing EntityEditorComponentSettingsCopyPaste.cpp")
    else:
        text = COPY.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "CopyComponentSettingsToClipboard",
            "PasteComponentSettingsFromClipboard",
            "clipboardComponentSlot",
            "設定をコピー",
            "設定をペースト",
        ):
            if needle not in text:
                errors.append(f"copy/paste module missing: {needle}")

    if HEADER.is_file():
        if "clipboardComponentSlot" not in HEADER.read_text(encoding="utf-8", errors="replace"):
            errors.append("UiState must hold clipboardComponentSlot")
    else:
        errors.append("missing EntityEditor.h")

    if UI.is_file():
        if "DrawComponentSettingsCopyPasteButtons" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("DrawInspectorPanel must offer copy/paste buttons")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_entity_editor_component_settings_copy_paste: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_component_settings_copy_paste: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())