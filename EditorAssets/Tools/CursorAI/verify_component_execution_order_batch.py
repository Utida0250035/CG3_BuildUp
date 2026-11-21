#!/usr/bin/env python3
"""実行順 InputInt の一括適用 UI があるか検査。"""

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
    / "EntityEditorExecutionOrderBatch.cpp"
)
BATCH_UI = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorInspectorExecutionOrderBatchUi.cpp"
)
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"


def main() -> int:
    errors: list[str] = []

    if not BATCH_CPP.is_file():
        errors.append("missing EntityEditorExecutionOrderBatch.cpp")
    else:
        text = BATCH_CPP.read_text(encoding="utf-8", errors="replace")
        for sym in (
            "ApplyExecutionOrderToAllSlotsOfType",
            "ApplyExecutionOrderToAllSlotsOfKind",
            "RenumberExecutionOrderSequentiallyForKind",
        ):
            if sym not in text:
                errors.append(f"batch helper must define {sym}")

    if not BATCH_UI.is_file():
        errors.append("missing EntityEditorInspectorExecutionOrderBatchUi.cpp")
    else:
        ui_text = BATCH_UI.read_text(encoding="utf-8", errors="replace")
        if 'ImGui::InputInt("実行順 (priority)"' not in ui_text:
            errors.append("batch UI must retain execution order InputInt")
        for label in (
            "この値を同型すべてに適用",
            "この値を選択種別すべてに適用",
            "選択種別を 0 から連番で整列",
        ):
            if label not in ui_text:
                errors.append(f"batch UI must offer: {label}")

    if UI.is_file():
        main_ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorInspectorExecutionOrderBatchUi" not in main_ui:
            errors.append("EntityEditorUi must call execution order batch UI")
        if 'ImGui::InputInt("実行順 (priority)"' in main_ui:
            errors.append("execution order InputInt should live in batch UI only")
    else:
        errors.append("missing EntityEditorUi.cpp")

    if errors:
        print("verify_component_execution_order_batch: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_component_execution_order_batch: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())