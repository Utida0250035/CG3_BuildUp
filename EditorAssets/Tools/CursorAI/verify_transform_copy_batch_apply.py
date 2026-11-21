#!/usr/bin/env python3
"""初期トランスフォームのコピー／一括適用 UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CORE = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorTransformCopyPaste.cpp"
)
UI_PANEL = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorInspectorTransformCopyPasteUi.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
EDITOR_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"


def main() -> int:
    errors: list[str] = []

    if not CORE.is_file():
        errors.append("missing EntityEditorTransformCopyPaste.cpp")
    else:
        text = CORE.read_text(encoding="utf-8", errors="replace")
        if "clipboardInitialTransform" not in text:
            errors.append("must use clipboardInitialTransform")
        if "ApplyClipboardTransformToInstanceIndices" not in text:
            errors.append("must apply to multiple indices")

    if not UI_PANEL.is_file():
        errors.append("missing inspector transform copy paste UI")
    else:
        panel = UI_PANEL.read_text(encoding="utf-8", errors="replace")
        if "トランスフォームをコピー" not in panel:
            errors.append("copy button required")
        if "貼り付け" not in panel:
            errors.append("paste button required")
        if "一括適用" not in panel:
            errors.append("batch apply button required")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorInspectorTransformCopyPasteUi" not in ui:
            errors.append("inspector must draw transform copy paste")
        if "クリップボードから一括適用" not in ui:
            errors.append("multi-select panel must batch apply transform")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if EDITOR_H.is_file():
        if "clipboardInitialTransform" not in EDITOR_H.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("UiState must store clipboardInitialTransform")
    else:
        errors.append("missing EntityEditor.h")

    if errors:
        print("verify_transform_copy_batch_apply: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_transform_copy_batch_apply: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())