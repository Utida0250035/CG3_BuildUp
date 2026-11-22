#!/usr/bin/env python3
"""Release 起動パスから BuildUp を予約できる UI を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    helper_h = (
        ROOT
        / "Project"
        / "Codes"
        / "Include"
        / "System"
        / "Editor"
        / "Entity"
        / "EntityEditorReleaseStartupBuildUp.h"
    )
    ui_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "Entity"
        / "EntityEditorUi.cpp"
    )

    if not helper_h.is_file():
        errors.append("missing EntityEditorReleaseStartupBuildUp.h")

    if ui_cpp.is_file():
        text = ui_cpp.read_text(encoding="utf-8", errors="replace")
        if "Release 起動で BuildUp" not in text:
            errors.append("EntityEditorUi: expected Release startup BuildUp button")
        if "TryQueueBuildUpFromReleaseStartup" not in text:
            errors.append("EntityEditorUi: expected TryQueueBuildUpFromReleaseStartup")

    if errors:
        print("verify_entity_editor_release_startup_launch: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_release_startup_launch: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())