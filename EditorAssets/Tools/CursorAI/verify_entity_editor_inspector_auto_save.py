#!/usr/bin/env python3
"""インスペクタ／Authoring 編集後にディスク保存がキューされるか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
NOTIFY = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorAuthoringDocumentEditNotify.cpp"
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not NOTIFY.is_file():
        errors.append("missing EntityEditorAuthoringDocumentEditNotify.cpp")
    else:
        text = NOTIFY.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "requestApplyPreview",
            "MarkSceneDocumentDirty",
            "requestSaveToDisk",
            "jsonFilePath.empty()",
        ):
            if needle not in text:
                errors.append(f"Notify implementation missing: {needle}")

    if UI.is_file():
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorAuthoringDocumentEditNotify::Notify" not in ui:
            errors.append("DrawInspectorPanel path must call Notify on edit")
        if 'if (changed) {\n    ui_.requestApplyPreview = true;' in ui:
            errors.append("inspector changed block must not only requestApplyPreview")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_entity_editor_inspector_auto_save: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_inspector_auto_save: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())