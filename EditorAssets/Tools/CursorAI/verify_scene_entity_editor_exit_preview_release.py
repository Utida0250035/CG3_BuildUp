#!/usr/bin/env python3
"""ExitScene がプレビュー解放と Ui 実行フラグ巻き戻しを行うことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    teardown_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorExitSceneTeardown.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if not teardown_cpp.is_file():
        errors.append("missing SceneEntityEditorExitSceneTeardown.cpp")
    else:
        text = teardown_cpp.read_text(encoding="utf-8", errors="replace")
        if "ReleasePreviewEntities" not in text:
            errors.append("teardown must release preview entities")
        if "requestPlayRuntimeScene" not in text:
            errors.append("teardown must clear requestPlayRuntimeScene")
        if "requestLaunchSceneBuildUp" not in text:
            errors.append("teardown must clear BuildUp request")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        exit_fn = text.split("void SceneEntityEditor::ExitScene", 1)
        if len(exit_fn) < 2:
            errors.append("ExitScene not found")
        else:
            body = exit_fn[1].split("void SceneEntityEditor::", 1)[0]
            if "ReleasePreviewEntities" not in body:
                errors.append("ExitScene must call ReleasePreviewEntities")
            if "ResetEditorTransientPlayUi" not in body:
                errors.append("ExitScene must reset transient play Ui")

    if errors:
        print("verify_scene_entity_editor_exit_preview_release: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_exit_preview_release: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())