#!/usr/bin/env python3
"""EntityEditor Dock 既定レイアウトの version 管理と再構築を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
LAYOUT = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorEntityWorkbenchDefaultDockLayout.cpp"
)
WORKBENCH = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorEntityWorkbench.cpp"
UI = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "Entity" / "EntityEditor.h"


def main() -> int:
    errors: list[str] = []

    if not LAYOUT.is_file():
        errors.append("missing EditorEntityWorkbenchDefaultDockLayout.cpp")
    else:
        text = LAYOUT.read_text(encoding="utf-8", errors="replace")
        if "kDefaultDockLayoutVersion" not in text:
            errors.append("layout must expose version constant")
        if "ApplyIfNeeded" not in text:
            errors.append("ApplyIfNeeded required")

    if WORKBENCH.is_file():
        wb = WORKBENCH.read_text(encoding="utf-8", errors="replace")
        if "defaultLayoutBuilt" in wb:
            errors.append("workbench must not use static defaultLayoutBuilt")
        if "EditorEntityWorkbenchDefaultDockLayout::ApplyIfNeeded" not in wb:
            errors.append("workbench must call ApplyIfNeeded")
        if "requestResetDefaultDockLayout" not in wb:
            errors.append("view menu must reset dock layout")
    else:
        errors.append("missing EditorEntityWorkbench.cpp")

    if UI.is_file():
        if "requestResetDefaultDockLayout" not in UI.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("UiState must have requestResetDefaultDockLayout")
    else:
        errors.append("missing EntityEditor.h")

    if errors:
        print("verify_editor_entity_workbench_default_dock_layout: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_entity_workbench_default_dock_layout: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())