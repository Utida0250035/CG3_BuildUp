#!/usr/bin/env python3
"""コンポーネント削除が単体・同型一括・種別一括に対応するか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BATCH_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorRemoveComponentSlotsBatch.cpp"
)
BATCH_UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorInspectorComponentRemoveBatchUi.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not BATCH_CPP.is_file():
        errors.append("missing EntityEditorRemoveComponentSlotsBatch.cpp")
    else:
        text = BATCH_CPP.read_text(encoding="utf-8", errors="replace")
        for sym in (
            "RemoveComponentSlotAtIndex",
            "RemoveAllComponentSlotsOfType",
            "RemoveAllComponentSlotsOfKind",
        ):
            if sym not in text:
                errors.append(f"batch helper must define {sym}")

    if not BATCH_UI.is_file():
        errors.append("missing EntityEditorInspectorComponentRemoveBatchUi.cpp")
    else:
        ui_text = BATCH_UI.read_text(encoding="utf-8", errors="replace")
        for label in (
            "選択中のコンポーネントを削除",
            "同じ型をすべて削除",
            "選択種別のコンポーネントをすべて削除",
        ):
            if label not in ui_text:
                errors.append(f"batch UI must offer: {label}")

    if UI.is_file():
        main_ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorInspectorComponentRemoveBatchUi" not in main_ui:
            errors.append("EntityEditorUi must call batch remove UI")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_component_remove_batch: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_component_remove_batch: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())