#!/usr/bin/env python3
"""起動シーン設定が SaveToJsonPath を暗黙呼び出ししない経路を検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
UI = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorUi.cpp"
MOD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorStartupSceneSetWithoutAutosave.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorStartupSceneSetWithoutAutosave.cpp")
    else:
        mod = MOD.read_text(encoding="utf-8", errors="replace")
        if "SaveToJsonPath" in mod:
            errors.append("startup module must not call SaveToJsonPath")
        if "TrySetRepositoryStartupScene" not in mod:
            errors.append("startup module must set launch_config")

    if not UI.is_file():
        errors.append("missing EntityEditorUi.cpp")
    else:
        ui = UI.read_text(encoding="utf-8", errors="replace")
        if "EntityEditorStartupSceneSetWithoutAutosave::SetOnly" not in ui:
            errors.append("UI must wire SetOnly for startup button")
        block = ui.split("起動シーンに設定", 1)[-1].split("設定して BuildUp", 1)[0]
        if "SaveToJsonPath" in block:
            errors.append("startup button block must not call SaveToJsonPath")
        buildup_block = ui.split("設定して BuildUp", 1)[-1].split("Release 起動", 1)[0]
        if re.search(r"SaveToJsonPath\s*\(", buildup_block):
            errors.append("set+buildup block must not call SaveToJsonPath")

    if errors:
        print("verify_entity_editor_startup_set_no_implicit_save: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_startup_set_no_implicit_save: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())