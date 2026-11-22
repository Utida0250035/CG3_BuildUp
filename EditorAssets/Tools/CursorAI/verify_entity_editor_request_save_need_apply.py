#!/usr/bin/env python3
"""requestSaveToDisk 処理後にプレビュー再構築（needApply）へつながる経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MOD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorTickPendingRequests.cpp"
)
SCENE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorTickPendingRequests.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        if "requestSaveToDisk" not in text:
            errors.append("pending requests must handle requestSaveToDisk")
        save_block = text.split("requestSaveToDisk", 1)[-1]
        if "needApply = true" not in save_block.split("requestApplyPreview", 1)[0]:
            errors.append("save-to-disk must set needApply = true")

    if SCENE.is_file():
        scene = SCENE.read_text(encoding="utf-8", errors="replace")
        if "TickPendingRequests()" not in scene or "RebuildPreviewScene" not in scene:
            errors.append("SceneEntityEditor must rebuild preview when TickPendingRequests true")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if errors:
        print("verify_entity_editor_request_save_need_apply: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_request_save_need_apply: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())